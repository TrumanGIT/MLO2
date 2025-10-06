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

 inline void Initialize() {
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
}

 inline RE::NiPointer<RE::NiNode> cloneNiNode(std::string templatePath) {

     RE::NiPointer<RE::NiNode> loaded;
     auto args = RE::BSModelDB::DBTraits::ArgsType();

     auto result = RE::BSModelDB::Demand(templatePath.c_str(), loaded, args);

     if (result == RE::BSResource::ErrorCode::kNone && loaded) {

      //   logger::info("Loaded root node type: {}", loaded->GetRTTI()->GetName());
       //  logger::info("Loaded root children count: {}", loaded->children.size());

        // if (!loaded->children.empty() && loaded->children[0]) {
           //  auto firstChild = loaded->children[0]->AsNode();
             //logger::info("First child node type: {}", firstChild->GetRTTI()->GetName());
            // logger::info("First child children count: {}", firstChild->children.size());
        // }

         auto fadeNode = loaded->AsNode();
         if (fadeNode && !fadeNode->children.empty()) {
               auto firstChild = loaded->children[0]->AsNode();

               if (!firstChild || firstChild->children.empty()) return nullptr;
             RE::NiCloningProcess cloningProcess;
             auto cloneBase = firstChild->CreateClone(cloningProcess);
             //firstChild->ProcessClone(cloningProcess);

             if (cloneBase) {
                 auto yourGlowNodePrototype = cloneBase->AsNode();
                 if (yourGlowNodePrototype) {
                   //  logger::info("Successfully extracted and cloned prototype from NIF!");
                     return RE::NiPointer<RE::NiNode>(yourGlowNodePrototype);
                 }
                 else {
                     logger::error("Cloned object is not a NiNode!");
                     return nullptr;
                 }
             }
             else {
                 logger::error("Failed to clone prototype node!");
                 return nullptr;
             }

         }
         else {
             logger::warn("Fade node is missing or has no children");
             return nullptr;
         }

     }
     else {
         logger::warn("Failed to load NIF file {}", templatePath);
         return nullptr;
     }
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

inline std::string matchedKeyword(std::string nodeName) {
    std::string matchedKeyword;

    for (const auto& [keyword, templates] : keywordTemplateMap) {
        if (nodeName.find(keyword) != std::string::npos) {
            matchedKeyword = keyword;
            break;
        }
    }

    return matchedKeyword;
}

//we clone and store ni nodes in a bank on startup to help with performance (for nodes we can anyway)

inline RE::NiPointer<RE::NiNode>& getNextNodeFromBank(const std::string& keyword) {
    auto& bank = keywordNodeBank[keyword];

    if (bank.empty()) {
        return nullptr;
    }

    static std::size_t chandelierCount = 0;
    static std::size_t candleCount = 0;
    static std::size_t fxfirewCount = 0;
    static std::size_t campfireCount = 0;
    static std::size_t fireplacewoodCount = 0;
    static std::size_t torchSconceCount = 0;
    static std::size_t torchCount = 0;
    static std::size_t lanternCount = 0;
    static std::size_t dwecCount = 0;
    static std::size_t dwewallsCount = 0;
    static std::size_t firepitCount = 0;
    static std::size_t errorCount = 0;
    std::size_t& count = [&]() -> std::size_t& {
        if (keyword == "chandelier") return chandelierCount;
        if (keyword == "candle") return candleCount;
        if (keyword == "fxfirew") return fxfirewCount;
        if (keyword == "campfire") return campfireCount;
        if (keyword == "fireplacewood") return fireplacewoodCount;
        if (keyword == "torchsconce") return torchSconceCount;
        if (keyword == "torch") return torchCount;
        if (keyword == "lantern") return lanternCount;
        if (keyword == "dwec") return dwecCount;
        if (keyword == "dwewalls") return dwewallsCount;
        if (keyword == "firepit") return firepitCount;
        return errorCount; // fallback
        }();

    RE::NiPointer<RE::NiNode>& node = bank[count];
    count = (count + 1) % bank.size(); // this could cause infinite loop ? 

    // logger::info("Keyword '{}' used node index = {}", keyword, count);

    return node;
}

inline bool cloneAndAttachNodesForSpecificMeshes(const std::string& nodeName, RE::NiPointer<RE::NiNode>& a_root) {

   
         auto it = baseMeshesAndTemplateToAttach.find(nodeName);
    if (it != baseMeshesAndTemplateToAttach.end()) {
    
        for (const auto& exclude : exclusionList) {
            if (nodeName == exclude) {
             //   logger::info("skipping exclude {}", exclude);
                return true;
            }
        }

        for (const auto& exclude : exclusionListPartialMatch) {
            if (nodeName.find(exclude) != std::string::npos) { // must compare to npos!
                return true;
            }
        }

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

inline bool cloneAndAttachNodesForPartialMeshes(const std::string& nodeName, RE::NiPointer<RE::NiNode>& a_root) {


    auto it = keywordTemplateMap.find(nodeName);
    if (it != keywordTemplateMap.end()) {

        for (const auto& exclude : exclusionList) {
            if (nodeName == exclude) {
                //   logger::info("skipping exclude {}", exclude);
                return true;
            }
        }

        for (const auto& exclude : exclusionListPartialMatch) {
            if (nodeName.find(exclude) != std::string::npos) { // must compare to npos!
                return true;
            }
        }

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
// on startup make a bunch of cloned nodes so we dont have to clone during gameplay
inline void assignClonedNodesToBank() {
    logger::info("Assigning cloned nodes... total groups: {}", keywordNodeBank.size());

    std::string prefix = "Meshes\\MLO\\Templates\\";

    for (auto& [keyword, arrayOfNodes] : keywordNodeBank) {
        auto templateIt = keywordTemplateMap.find(keyword);
        if (templateIt != keywordTemplateMap.end()) {
            std::string templatePath = prefix + templateIt->second;
           // logger::info("Template path for {} is {}", keyword, templatePath);

            if (keyword == "candle") {
                for (size_t i = 0; i < 75; i++) {  // lots of candles lmao
                    arrayOfNodes[i] = cloneNiNode(templatePath);
                }
            }
            else {
                for (size_t i = 0; i < 25; i++) {
                    arrayOfNodes[i] = cloneNiNode(templatePath);
                }
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

// checkcs if fake lights should be disabled by checking some user settings. and excluding dynamicform lights
// or whitelisted lights by checking the plugin name or carryable or shadowcasters lol

inline bool should_disable_light(RE::TESObjectLIGH* light, RE::TESObjectREFR* ref) 
{
    if (!ref || !light || ref->IsDynamicForm()) {
        return false;
    }

    if (disableShadowCasters != 1 &&
        light->data.flags.any(RE::TES_LIGHT_FLAGS::kOmniShadow,
                              RE::TES_LIGHT_FLAGS::kHemiShadow,
                              RE::TES_LIGHT_FLAGS::kSpotShadow)) 
    {
        return false;
    }
    
    auto refOriginFile = ref->GetDescriptionOwnerFile();
    std::string modName = refOriginFile->fileName; 
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
        } /*else if (line.starts_with("forswornFires=")) {
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

inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

inline void ReadMasterListAndFillMap() {
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
            // detect section headers
            if (line.find("PARTIAL SEARCH MATCHES") != std::string::npos) {
                mapPtr = &keywordTemplateMap;
                readingSpecificMeshes = false;
                
            }
            else if (line.find("EXCLUDE SPECIFIC MESHES") != std::string::npos) {
                break;
            }
            continue;
        }

        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            (*mapPtr)[key] = value; // dereference,  [] > * in operator precedence. (*mapPtr) is evaluated first, giving you the actual map.
            
            if (readingSpecificMeshes)
                logger::info("Specific mesh and its template: {} = {}", key, value);
            else
                logger::info("Partial search and its template: {} = {}", key, value);
        }

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
            if (line.find("EXCLUDE SPECIFIC MESHES") != std::string::npos) {
                section = 1;
            }
            else if (line.find("PARTIAL SEARCH EXCLUDES") != std::string::npos) {
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


inline void dummyHandler(RE::NiNode* root, std::string nodeName)
{
    if (!root) return;

    // Only operate on nodes whose own name contains "dummy"
  
    if (nodeName.find("dummy") == std::string::npos)
        return;

    // Search children for a NiNode whose name contains "candle"
    for (auto& child : root->children) {
        if (!child) continue;

        auto niNodeChild = child->AsNode();
        if (!niNodeChild) continue;

        std::string childName = niNodeChild->name.c_str();
        toLower(childName);

        if (childName.find("chandelier") != std::string::npos) {
            RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank("chandelier");
            root->AttachChild(nodePtr.get());
            
            return; // attach only once
        }

        if (childName.find("candle") != std::string::npos) {
            RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank("candle");
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


