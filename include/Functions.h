#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include "global.h"
#include <fstream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <sstream>
#include <iostream>

namespace logger = SKSE::log;

inline void toLower(std::string& str) {
    for (auto& c : str) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
}

inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

inline void splitString(const std::string& input, char delimter, std::vector<std::string>& listToSplit) {
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, delimter)) {
        while (!item.empty() && std::isspace(item.front())) {
            item.erase(item.begin());  // begin() =  (position)  front() = (value) erase takes a pos
        }

        while (!item.empty() && std::isspace(item.back())) {
            item.pop_back();
        }

        listToSplit.push_back(item);

        spdlog::info("added {} to whitelist", item);
    }
}

inline void IniParser() {
    std::ifstream iniFile("Data\\SKSE\\Plugins\\MLO.ini");
    std::string line;

    while (std::getline(iniFile, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == ';') continue;

        if (line.starts_with("disableShadowCasters=")) {
            std::string value = line.substr(std::string("disableShadowCasters=").length());
            toLower(value);

            if (value == "true" || value == "1")
                disableShadowCasters = 1;
            else if (value == "false" || value == "0")
                disableShadowCasters = 0;
            else
                spdlog::warn("Invalid value for disableShadowCasters: {}", value);

            spdlog::info("INI override: disableShadowCasters = {}", disableShadowCasters);

        }
        else if (line.starts_with("disableTorchLights=")) {
            std::string value = line.substr(std::string("disableTorchLights=").length());
            toLower(value);

            if (value == "true" || value == "1")
                disableTorchLights = 1;
            else if (value == "false" || value == "0")
                disableTorchLights = 0;
            else
                spdlog::warn("Invalid value for disableTorchLights: {}", value);

            spdlog::info("INI override: disableTorchLights = {}", disableTorchLights);
        }
        
        /*else if (line.starts_with("forswornFires=")) {
            forswornFires = line.substr(std::string("forswornFires=").length());
            spdlog::info("INI override: forswornFires = {}", forswornFires);

        } else if (line.starts_with("vampireFires=")) {
            vampireFires = line.substr(std::string("vampireFires=").length());
            spdlog::info("INI override: vampireFires = {}", vampireFires);

        } else if (line.starts_with("dungeonFires=")) {
            dungeonFires = line.substr(std::string("dungeonFires=").length());
            spdlog::info("INI override: dungeonFires = {}", dungeonFires);

        } else if (line.starts_with("dragonPriestFires=")) {
            dragonPriestFires = line.substr(std::string("dragonPriestFires=").length());
            spdlog::info("INI override: dragonPriestFires = {}", dragonPriestFires);
        }*/
        else if (line.starts_with("RGB Values=")) {
            auto values = line.substr(std::string("RGB Values=").length());

            // Find each color
            auto rPos = values.find("Red:");
            auto gPos = values.find("Green:");
            auto bPos = values.find("Blue:");

            if (rPos != std::string::npos) {
                red = static_cast<std::uint8_t>(std::stoi(values.substr(rPos + 4)));
            }
            if (gPos != std::string::npos) {
                green = static_cast<std::uint8_t>(std::stoi(values.substr(gPos + 6)));
            }
            if (bPos != std::string::npos) {
                blue = static_cast<std::uint8_t>(std::stoi(values.substr(bPos + 5)));
            }

            spdlog::info("INI override: Bulb RGB values set to R:{} G:{} B:{}", red, green, blue);
        }

        else if (line.starts_with("whitelist=")) {
            std::string prefix = "whitelist=";

            line.erase(0, prefix.length());

            splitString(line, ',', whitelist);
        }


    }
}

inline void ReadMasterListAndFillMaps() {
    std::string path = "Data\\SKSE\\Plugins\\Masterlist.ini";

    if (!std::filesystem::exists(path)) {
        std::cerr << "INI file not found: " << path << std::endl;
        return;
    }

    std::ifstream iniFile(path);
    if (!iniFile.is_open()) {
        logger::warn("INI file not found or failed to open, using defaults.");
        return;
    }

    std::string line;
    std::unordered_map<std::string, std::string>* mapPtr = &baseMeshesAndTemplateToAttach;
    bool readingSpecificMeshes = true;

    while (std::getline(iniFile, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line.starts_with(";")) {
            if (line.find("PARTIAL SEARCH STRING NODE MATCHES") != std::string::npos) {
                mapPtr = &keywordTemplateMap;
                readingSpecificMeshes = false;
            } else if (line.find("EXCLUDE SPECIFIC NODES BY NAME") != std::string::npos) {
                break;
            }
            continue;
        }

        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            toLower(key);
            (*mapPtr)[key] = value;
            
            if (!readingSpecificMeshes) {
                keywordNodeBank[key] = {}; /// Initialize the bank dynamically 
                priorityList.push_back(key);
            }

            if (readingSpecificMeshes)
                logger::info("Specific mesh and its template: {} = {}", key, value);
            else
                logger::info("Partial search and its template: {} = {}", key, value);
        }
    }

    for (const auto& [key, value] : keywordNodeBank){
        logger::info(" NodeBank Setup for partial search {}", key);
    }

    iniFile.close();
}


inline void ReadMasterListAndFillExcludes() {
    std::string path = "Data\\SKSE\\Plugins\\Masterlist.ini";

    if (!std::filesystem::exists(path)) {
        logger::warn("INI file not found: {}", path);
        return;
    }

    std::ifstream iniFile(path);
    if (!iniFile.is_open()) {
        logger::warn("Failed to open INI file: {}", path);
        return;
    }

    std::string line;
    int section = 0; // 0=normal, 1=exact excludes, 2=partial excludes

    while (std::getline(iniFile, line)) {
        line = trim(line);
        if (line.empty())
            continue;

        if (line.starts_with(";")) {
            // detect section headers
            if (line.find("EXCLUDE SPECIFIC NODES BY NAME") != std::string::npos) {
                section = 1;
            }
            else if (line.find("EXCLUDE PARTIAL NODES BY NAME") != std::string::npos) {
                section = 2;
            }
            continue;
        }

        if (section == 1) {
            line = trim(line);  // already trimming, good
            toLower(line);      // lowercase for consistency
            exclusionList.push_back(line);
            logger::info("Added exact exclude: '{}'", line);  // wrap in quotes to see trailing whitespace
        }
        else if (section == 2) {
            line = trim(line);
            toLower(line);
            exclusionListPartialMatch.push_back(line);
            logger::info("Added partial exclude: '{}'", line);
        }
    }

    iniFile.close();
}

/* inline void Initialize() {
     logger::info("loading forms");
    auto dataHandler = RE::TESDataHandler::GetSingleton(); // single instance

    keywordForswornCamp = dataHandler->LookupForm<RE::BGSKeyword>(0x000130EE, "Skyrim.esm");
    if (!keywordForswornCamp) {
        logger::info("BGSKeyword LocTypeForswornCamp (0x000130EE) not found");
    }

    keywordVampireLair = dataHandler->LookupForm<RE::BGSKeyword>(0x000130EB, "Skyrim.esm");
    if (!keywordVampireLair) {
        logger::info("BGSKeyword LocTypeVampireLair (0x000130EB) not found");
    }

    keywordDungeon = dataHandler->LookupForm<RE::BGSKeyword>(0x000130DB, "Skyrim.esm");
    if (!keywordDungeon) {
        logger::info("BGSKeyword LocTypeDungeon (0x000130DB) not found");
    }

    keywordDragonPriestLair = dataHandler->LookupForm<RE::BGSKeyword>(0x000130E1, "Skyrim.esm");
    if (!keywordDragonPriestLair) {
        logger::info("BGSKeyword LocTypeDragonPriestLair (0x000130E1) not found");
    }
} */

 inline bool IsInSoulCairnOrApocrypha(RE::PlayerCharacter* player) {
     if (!player) {
                 logger::info(" IsInSoulCairnOrApocrypha: player not valid, cant get location");
         return false;
     }
     auto worldspace = player->GetWorldspace();
     if (!worldspace) {
       // logger::info("worldSpace not valid cant get location");
         return false;  // Not in a worldspace (probably in an interior cell)
     }

    // logger::info("current worldspace = {}", worldspace->GetFormID());

     if (worldspace->GetFormID() == apocryphaFormID || worldspace->GetFormID() == soulCairnFormID){
       //  logger::info("is in soul cairn or apocrypha");
         return true;
     }
     
         return false;
 }

inline RE::NiPointer<RE::NiNode> cloneNiNode(const std::string& templatePath) {
    RE::NiPointer<RE::NiNode> loaded;
    auto args = RE::BSModelDB::DBTraits::ArgsType();

    auto result = RE::BSModelDB::Demand(templatePath.c_str(), loaded, args);
    if (result != RE::BSResource::ErrorCode::kNone || !loaded) {
        logger::warn("Could not find template {}", templatePath);
        return nullptr;
    }

    auto fadeNode = loaded->AsNode();
    if (!fadeNode || fadeNode->children.empty() || !fadeNode->children[0]) {
        logger::warn("Template Fade node is missing, has no children, or first child is null");
        return nullptr;
    }

    auto firstChild = fadeNode->children[0]->AsNode();
    if (!firstChild) {
        logger::info("cloneNiNode: first child cast as node failed");
        return nullptr;
    }

    RE::NiCloningProcess cloningProcess;
    auto cloneBase = firstChild->CreateClone(cloningProcess);
    if (!cloneBase) {
        logger::error("Failed to clone template node!");
        return nullptr;
    }

    auto yourGlowNodePrototype = cloneBase->AsNode();
    if (!yourGlowNodePrototype) {
        logger::error("Cloned object is not a NiNode!");
        return nullptr;
    }

    // Successfully cloned node
    return RE::NiPointer<RE::NiNode>(yourGlowNodePrototype);
}


 // sort out what arrays of meshes get what template node. NOTUSED I failed in cloning a clone to use as a template rather then load from disk to save perf. clones came out bugged and shining blue light 
/*inline void assignClonedNodes() {
    logger::info("assigning cloned nodes... total groups: {}", baseMeshesAndNiNodeToAttach.size());

    std::string prefix = "Meshes\\NEW\\SSE\\";
    int templateFilePathIterator = 0;

    for (auto& [meshPaths, nodePtr] : baseMeshesAndNiNodeToAttach) {
      //  logger::info("cloneing nodes for group #{}", templateFilePathIterator);

        if (templateFilePathIterator >= templateNames.size()) {
            logger::warn("Ran out of template names for map entries at index {}", templateFilePathIterator);
            break;
        }

        auto templatePath = prefix + templateNames[templateFilePathIterator];
        logger::info("Loading template path [{}]", templatePath);

        auto clonedNode = cloneNiNode(templatePath);

        nodePtr = clonedNode;  // store one copy for the group

        templateFilePathIterator++;
    }

    logger::info("Finished assignClonedNodes");
}*/


// this mod checks for lights to edit by node name. some light nodes are called dummy thanks bethesda

inline bool isExclude(const std::string& nodeName) { 
    for (const auto& exclude : exclusionList) {
                if (nodeName == exclude) {
                    // logger::info("skipping exclude {}", exclude);
                    return true;
                }
            }

            // Exclude partial matches
            for (const auto& exclude : exclusionListPartialMatch) {
                if (nodeName.find(exclude) != std::string::npos) {
                    return true;
                }
            }
            return false;
}

inline std::string matchedKeyword(const std::string& nodeName)
{

    for (const auto& keyword : priorityList) {
        if (nodeName.find(keyword) != std::string::npos) {
            return keyword;
        }
    }

    return {};
}

//we clone and store ni nodes in a bank on startup to help with performance (for nodes we can anyway)

inline RE::NiPointer<RE::NiNode> getNextNodeFromBank(const std::string& keyword)
{
    auto it = keywordNodeBank.find(keyword);

    if (it == keywordNodeBank.end() || it->second.empty()) {
        logger::warn("getNextNodeFromBank: '{}' has no nodes available", keyword);
        return nullptr;
    }

    auto& bank = it->second; // keywords nodebank array

   // static here means initialised once and map contents survive each call
    static std::unordered_map<std::string, std::size_t> counters;
    auto& count = counters[keyword];                                 // index for the next node to use in bank

    if (count >= bank.size())
        count = 0; // resets bank to 0 if we passed the limit (likely fine to recycle the nodes)

    RE::NiPointer<RE::NiNode> node = bank[count];

    if (!node) {
        logger::warn("getNextNodeFromBank: '{}' node index {} is null", keyword, count);
        return nullptr;
    }

        count++;

    return node;
}

inline bool cloneAndAttachNodesForSpecificMeshes(const std::string& nodeName, RE::NiPointer<RE::NiNode>& a_root) {

    auto it = baseMeshesAndTemplateToAttach.find(nodeName);

    if (it != baseMeshesAndTemplateToAttach.end()) {
    
        if (isExclude(nodeName)) return true; 

        std::string fullPath = "Meshes\\MLO\\Templates\\" + it->second;

        auto nodeClone = cloneNiNode(fullPath);
        if (!nodeClone) {
            logger::warn("Failed to clone from template for {}", nodeName);
            return false;
        }

        a_root->AttachChild(nodeClone.get());
        //logger::warn("attached node to specific mesh {} ", nodeName);

        return true;
    }

    return false;
}

// on startup make a bunch of cloned nodes so we dont have to clone from disk during gameplay
inline void assignClonedNodesToBank() {
    logger::info("Assigning cloned nodes... total groups: {}", keywordNodeBank.size());

    const std::string prefix = "Meshes\\MLO\\Templates\\";

    for (auto& [keyword, arrayOfNodes] : keywordNodeBank) {
        auto templateIt = keywordTemplateMap.find(keyword);
        if (templateIt == keywordTemplateMap.end()) {
            logger::warn("AssignClonedNodesToBank: keyword without a template found: {}", keyword);
            continue; // no template for this keyword
        }
        const std::string templatePath = prefix + templateIt->second;
        const size_t maxNodes = (keyword == "candle") ? 75 : 25;

        for (size_t i = 0; i < maxNodes; ++i) {
            auto clonedNode = cloneNiNode(templatePath);
            if (clonedNode) {
                arrayOfNodes[i] = clonedNode;
            } else {
                logger::warn("Skipping node {} for keyword '{}', clone failed", i, keyword);
            }
        }
    }

    logger::info("Finished assignClonedNodes");
}

// stole this from somewhere Po3 mabye

template <class T, std::size_t size = 5>  
inline void write_thunk_call(std::uintptr_t a_src) {
    auto& trampoline = SKSE::GetTrampoline();
    if constexpr (size == 6) {
        T::func = *(uintptr_t*)trampoline.write_call<6>(a_src, T::thunk);
    } else {
        T::func = trampoline.write_call<size>(a_src, T::thunk);
    }
}

// checs if fake lights should be disabled by checking some user settings. and excluding dynamicform lights
// or whitelisted lights by checking the plugin name or carryable or shadowcasters lol

inline bool should_disable_light(RE::TESObjectLIGH* light, RE::TESObjectREFR* ref, std::string modName)
{
    if (!ref || !light || ref->IsDynamicForm()) {
        return false;
    }

	auto player = RE::PlayerCharacter::GetSingleton();

    if (IsInSoulCairnOrApocrypha(player)) {
        logger::info("player is in apocrypha or soul cairn so we should not disable light");
        return false;
    }
        if (disableShadowCasters == 0 &&
            light->data.flags.any(RE::TES_LIGHT_FLAGS::kOmniShadow,
                RE::TES_LIGHT_FLAGS::kHemiShadow, RE::TES_LIGHT_FLAGS::kSpotShadow ))
        {
            return false;
        }

        if (disableTorchLights == 0 &&
            light->data.flags.any(RE::TES_LIGHT_FLAGS::kCanCarry))
        {
            return false;
        }

        for (const auto& whiteListedMod : whitelist) {
            if (modName.find(whiteListedMod) != std::string::npos) {
                return false;
            }
        }

    return true;
}
// method to swap fire color models not used anymore see below

/*inline void ApplyColorSwitch(RE::TESModel* bm, const std::string& newPath) {
    if (!bm) return;
    auto currentModel = bm->GetModel();
    if (currentModel != newPath) {
        if (ModelsAndOriginalFilePaths.find(bm) == ModelsAndOriginalFilePaths.end()) {
            ModelsAndOriginalFilePaths[bm] = currentModel;
        }
        bm->SetModel(newPath.c_str());
    }
}*/

/// this isent used anymore. it was to change colors of fires in certain locations but the feature got deprecated from the mod
/*inline void ProcessReference(RE::TESObjectREFR* a_ref) {
   // const auto refid = a_ref->GetFormID();
    const auto base = a_ref->GetBaseObject();

    if (!base) return;

    const auto baseFormID = base->GetFormID();

    bool foundMatch = false;

    for (auto id : fireFormIDs) {  // find if the ref is a fire and should be swapped
        if (id == baseFormID) {
            foundMatch = true;
            break;
        }
    }

    if (!foundMatch) return;

        auto player = RE::PlayerCharacter::GetSingleton();
        if (!player) return;

        RE::BGSLocation* currentLocation = player->GetCurrentLocation();
        if (!currentLocation) return;

        const auto bm = base->As<RE::TESModel>(); 
        if (!bm) return; 

        if (currentLocation->HasKeyword(keywordVampireLair)) {
            ApplyColorSwitch(bm, vampireFires ); 
        } else if (currentLocation->HasKeyword(keywordForswornCamp)) {
          ApplyColorSwitch(bm, forswornFires ); 
        } else if (currentLocation->HasKeyword(keywordDungeon)) {
            ApplyColorSwitch(bm, dungeonFires ); 
        } else if (currentLocation->HasKeyword(keywordDragonPriestLair)) {
            ApplyColorSwitch(bm, dragonPriestFires ); 
        } else {  // must restore default fires IF they have been changed.  or will get color fires everywhere.

            logger::info("Location has no matching keyword, restting to default fire, IF we need to");

            if (!ModelsAndOriginalFilePaths.empty()) {
                for (const auto& [TESModel, ModelPath] : ModelsAndOriginalFilePaths) {
                    if (TESModel) {
                        TESModel->SetModel(ModelPath.c_str());
                    }
                }
            }
            ModelsAndOriginalFilePaths.clear();
            return;
        }
    } */

// this also adds items to whitelist 


inline bool TorchHandler(const std::string& nodeName, RE::NiPointer<RE::NiNode>& a_root)
{
    if (nodeName == "torch") {
        RE::NiNode* attachLight = nullptr;
        RE::NiNode* torchFire = nullptr;

        // must null check everything or crash city. 

        for (auto& child : a_root->children) {
            if (!child) continue; // 
            auto childNode = child->AsNode();
            if (childNode && childNode->name == "TorchFire") {
                torchFire = childNode;
                break;
            }
        }
    
        if (torchFire) {
            for (auto& child : torchFire->children) {
                if (!child) continue;  
                auto childNode = child->AsNode();
                if (childNode && childNode->name == "AttachLight") {
                    attachLight = childNode;
                    break;
                }
            }
        }

        if (attachLight) {
            RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank("torch");
            if (nodePtr) {
                attachLight->AttachChild(nodePtr.get());
                // logger::info("attached light to torch at specific spot {}", nodeName);
                return true;
            }
        }
        else {
            logger::warn("hand held torch light placement failed for {}", nodeName);
        }
    } 
    return false;
}


inline bool candlesFusedInWallMeshesHandler(const std::string& nodeName, RE::NiPointer<RE::NiNode>& a_root)
{

    // 1Special case for "norcathallsm"
    if (nodeName.find("norcathallsm") != std::string::npos) {
        for (auto& child : a_root->children) {
            if (auto childAsNode = child->AsNode(); childAsNode) {
                std::string childName = childAsNode->name.c_str();
                if (childName.find("Glow") != std::string::npos) {
                    RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank("candle");
                    if (!nodePtr){
                        logger::info("northcallhallsm handler: candle node ptr from bank is invalid");
                        return false; 
                    }
                    childAsNode->AttachChild(nodePtr.get());
                    return true;
                }
            }
        }
    }

    //  For meshes matching anything in specialNodes
    for (const auto& special : specialNodes) {
        if (nodeName.find(special) != std::string::npos) {
            const std::string templatePath = "Meshes\\MLO\\Templates\\Nordic Column Candles 01 03_NOT Animated.nif"; 
            RE::NiPointer<RE::NiNode> loaded;
            auto args = RE::BSModelDB::DBTraits::ArgsType();

            auto result = RE::BSModelDB::Demand(templatePath.c_str(), loaded, args);
            if (result != RE::BSResource::ErrorCode::kNone || !loaded) {
                logger::warn("Failed to load NIF file {}", templatePath);
                continue;
            }

            auto fadeNode = loaded->AsNode();
            if (!fadeNode || fadeNode->children.size() < 3) {
                logger::warn(" Template {} is null or has insufficient child nodes", templatePath);
                continue;
            }

            // Clone the first three children
            RE::NiCloningProcess cloneProc1;
            RE::NiCloningProcess cloneProc2;
            RE::NiCloningProcess cloneProc3;

            auto clone1 = fadeNode->children[0]->CreateClone(cloneProc1);
            auto clone2 = fadeNode->children[1]->CreateClone(cloneProc2);
            auto clone3 = fadeNode->children[2]->CreateClone(cloneProc3);

            if (clone1 && clone2 && clone3) {
                a_root->AttachChild(clone1->AsNode());
                a_root->AttachChild(clone2->AsNode());
                a_root->AttachChild(clone3->AsNode());
                return true;
                logger::info("Attached 3 cloned nodes from template to {}", nodeName);
            }
            else {
                logger::error("Failed to clone one or more child nodes from {}", templatePath);
            }
        }
    }

    return false;
}

inline bool handleSceneRoot(RE::NiNode* root, const std::string& nodeName)
{
    if (!root)
        return false;

    // --- Check for "scene" keyword in node name ---
    if (nodeName.find("scene") == std::string::npos)
        return false;

    // --- Early out for NPC meshes ---
    if (!root->children.empty()) {
        auto firstChild = root->children[0];
        if (firstChild) {
            auto firstNode = firstChild->AsNode();
            if (firstNode) {
                std::string firstName = firstNode->name.c_str();
                toLower(firstName); // optional: for case-insensitive search
                if (firstName.find("npc") != std::string::npos) {
                    logger::info("NPC mesh found, exiting handleSceneRoot");
                    return true;
                }
            }
        }
    }

    bool sceneRootFoundCandlehornfloor = false;
    bool sceneRootFoundCandle = false;

    // --- Search for child nodes safely ---
    for (auto& child : root->children) {
        if (!child)
            continue;

        auto node = child->AsNode();
        if (!node)
            continue;

        const auto& name = node->name;
        if (name.empty())
            continue;

        std::string childName = name.c_str(); // safe conversion
        toLower(childName); // optional if you want case-insensitive search

        if (childName.find("candlehornfloor") != std::string::npos) {
            sceneRootFoundCandlehornfloor = true;
            logger::info("candlehornfloor node found under scene node!");
        }
        else if (childName.find("candle") != std::string::npos) {
            sceneRootFoundCandle = true;
            logger::info("candle node found under scene node!");
        }

        if (sceneRootFoundCandlehornfloor || sceneRootFoundCandle)
            break;
    }

    // --- Exit if neither node type was found ---
    if (!sceneRootFoundCandlehornfloor && !sceneRootFoundCandle)
        return false;

    std::string bankType = sceneRootFoundCandlehornfloor ? "candlehornfloor" : "candle";
    RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank(bankType);

    if (nodePtr) {
        root->AttachChild(nodePtr.get());
        logger::info("Attached '{}' node to '{}'", bankType, root->name.c_str());
        return true;
    }
    else {
        logger::warn("handleSceneRoot: Attach target or nodePtr was null for '{}'", bankType);
    }

    return false;
}




inline void dummyHandler(RE::NiNode* root, std::string nodeName)
{
    if (!root) return;

    // Only operate on nodes whose own name contains "dummy"
   
    if (nodeName.find("dummy") == std::string::npos)
        return;

    // Search children for a NiNode whose name contains "candle"
    for (auto& child : root->children) {
        if (!child) continue;

        auto childAsNode = child->AsNode();
        if (!childAsNode) {
            logger::info("dummy handler: child of dummy node could not be cast AsNode()");
        continue;
        } 

        std::string childName = childAsNode->name.c_str();
        toLower(childName);

        if (childName.find("chandel") != std::string::npos) { // skyrim spells chandelier wrong sometimes so "chandel" (for example sometimes chandelier has 2 'L's in its name, thanks bethesda)
            RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank("chandel");
            if (!nodePtr) {
                logger::info("DummyHandler: chandelier node from bank was null");
                return;
            }
            root->AttachChild(nodePtr.get());
            return; 
        }

        if (childName.find("ruins_floorcandlelampmid") != std::string::npos) {
            RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank("ruinsfloorcandlelampmidon");
              if (!nodePtr) {
                logger::info("DummyHandler: ruinsfloorcandlelampmidon node from bank was null");
                return;
            }
            root->AttachChild(nodePtr.get());
            return;
        }

        if (childName.find("candle") != std::string::npos) {
            RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank("candle");
               if (!nodePtr) {
                logger::info("DummyHandler: candle node from bank was null");
                return;
            }
            root->AttachChild(nodePtr.get());
            return;
        }

    }
}

inline void DumpFullTree(RE::NiAVObject* obj, int depth = 0)
{
    if (!obj) return;

    std::string indent(depth * 2, ' ');

    logger::info("{}- {} [{}]", indent, obj->name.c_str(), obj->GetRTTI() ? obj->GetRTTI()->name : "unknown");

    // if geometry, dump alpha + shader via GEOMETRY_RUNTIME_DATA
    if (auto geom = obj->AsGeometry()) {
        auto& runtime = geom->GetGeometryRuntimeData();
        if (runtime.properties[RE::BSGeometry::States::kProperty]) {
            logger::info("{}  * alphaProperty present", indent);
        }
        if (runtime.properties[RE::BSGeometry::States::kEffect]) {
            logger::info("{}  * shaderProperty present", indent);
        }
    }

    // recurse if node
    if (auto node = obj->AsNode()) {
        for (auto& child : node->children) {
            DumpFullTree(child.get(), depth + 1);
        }
    }
}


