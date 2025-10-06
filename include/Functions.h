#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include "global.h"
#include <fstream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <sstream>

namespace logger = SKSE::log;

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
 // clode a ni node froma  template ahead of time and assign as a value to a array of meshes
 inline RE::NiPointer<RE::NiNode> cloneNiNode(std::string templatePath) {

     RE::NiPointer<RE::NiNode> loaded;
     auto args = RE::BSModelDB::DBTraits::ArgsType();

     auto result = RE::BSModelDB::Demand(templatePath.c_str(), loaded, args);

     if (result == RE::BSResource::ErrorCode::kNone && loaded) {

         logger::info("Loaded root node type: {}", loaded->GetRTTI()->GetName());
         logger::info("Loaded root children count: {}", loaded->children.size());

         if (!loaded->children.empty() && loaded->children[0]) {
             auto firstChild = loaded->children[0]->AsNode();
             logger::info("First child node type: {}", firstChild->GetRTTI()->GetName());
             logger::info("First child children count: {}", firstChild->children.size());
         }

         auto fadeNode = loaded->AsNode();
         if (fadeNode && !fadeNode->children.empty()) {

             RE::NiCloningProcess cloningProcess;
             auto cloneBase = fadeNode->CreateClone(cloningProcess);
             fadeNode->ProcessClone(cloningProcess);

             if (cloneBase) {
                 auto yourGlowNodePrototype = cloneBase->AsNode();
                 if (yourGlowNodePrototype) {
                     logger::info("Successfully extracted and cloned prototype from NIF!");
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
         logger::warn("Failed to load NIF file");
         return nullptr;
     }
 }


inline void AttachNodeToMesh(RE::NiNode* root, RE::NiNode* nodeToAttach) {
    if (!root) {
        logger::warn("AttachNodeToMesh: root is null");
        return;
    }

    if (!nodeToAttach) {
        logger::warn("AttachNodeToMesh: nodeToAttach is null");
        return;
    }

    root->InsertChildAt(1,nodeToAttach); // append to the mesh's root
    logger::info("AttachNodeToMesh: inserted child at 1 successfully successfully");
}

// this is only if we need to overwrite 6 bits of memory instead of the default 5.... currently not used. ive tried alot of hooks lmao

template <class T, std::size_t size = 5>  
inline void write_thunk_call(std::uintptr_t a_src) {
    auto& trampoline = SKSE::GetTrampoline();
    if constexpr (size == 6) {
        T::func = *(uintptr_t*)trampoline.write_call<6>(a_src, T::thunk);
    } else {
        T::func = trampoline.write_call<size>(a_src, T::thunk);
    }
}

// makes sure were not disabling dynamically spawned lights for quests like the moondial light in volkihar courtyard in DLC1 
// or whitelisted lights by checking the plugin name or carryable or shadowcasters lol

inline bool should_disable_light(RE::TESObjectLIGH* light, RE::TESObjectREFR* ref) 
{
    if (!ref || !light || ref->IsDynamicForm()) {
        return false;
    }

    if (!disableShadowCasters &&
        light->data.flags.any(RE::TES_LIGHT_FLAGS::kOmniShadow,
                              RE::TES_LIGHT_FLAGS::kHemiShadow,
                              RE::TES_LIGHT_FLAGS::kSpotShadow)) 
    {
        return false;
    }

    if (!disableTorchLights &&
        light->data.flags.any(RE::TES_LIGHT_FLAGS::kCanCarry)) 
    {
        // disable lights that can be carried (i.e., torches)
        return false;
    }

    auto refOriginFile = ref->GetDescriptionOwnerFile();
    for (auto fileName : whitelist) {
        if (fileName == refOriginFile->fileName) {
            logger::info("found whitelisted light");
            return false;
        }
    }

    return true;
}

// method to swap fire color models

inline void ApplyColorSwitch(RE::TESModel* bm, const std::string& newPath) {
    if (!bm) return;
    auto currentModel = bm->GetModel();
    if (currentModel != newPath) {
        if (ModelsAndOriginalFilePaths.find(bm) == ModelsAndOriginalFilePaths.end()) {
            ModelsAndOriginalFilePaths[bm] = currentModel;
        }
        bm->SetModel(newPath.c_str());
    }
}

inline void ProcessReference(RE::TESObjectREFR* a_ref) {
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
    std::ifstream iniFile("Data\\SKSE\\Plugins\\MLO2.ini");
    std::string line;

    while (std::getline(iniFile, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == ';') continue;

        if (line.starts_with("disableShadowCasters=")) {
            disableShadowCasters = std::stof(line.substr(std::string("disableShadowCasters=").length()));
            spdlog::info("INI override: disableShadowCasters = {}", disableShadowCasters);

        } else if (line.starts_with("disableTorchLights=")) {
            disableTorchLights = std::stoi(line.substr(std::string("disableTorchLights=").length()));
            spdlog::info("INI override: disableTorchLights = {}", disableTorchLights);

        } else if (line.starts_with("forswornFires=")) {
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
        } else if (line.starts_with("RGB Values=")){
            Red = std::stoi(line.substr(std::string("red:").length()));
            blue = std::stoi(line.substr(std::string("blue:").length()));
            green = std::stoi(line.substr(std::string("green:").length()));
             spdlog::info("INI override: Bulb RBG values set to R:{} G:{} B:{}", red, green, blue);
        } 

        else if (line.starts_with("whitelist=")) {
            std::string prefix = "whitelist=";

            line.erase(0, prefix.length());

            splitString(line, ',', whitelist);
        }
    }

    iniFile.close(); 
}

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

inline void BuildMasterList() {
    std::string path = "Data\\SKSE\\Plugins\\MLO2.ini";

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
    int commentCount = 0;

    while (std::getline(iniFile, line)) {
        line = trim(line);

        if (line.empty()) continue;

        if (line.starts_with(";")) {
            commentCount++;
            if (commentCount >= 2) mapPtr = &keywordTemplateMap; // stop reading after second comment block
            continue; 
        }

        auto pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            (*mapPtr)[key] = value; // dereference,  [] > * in operator precedence. (*mapPtr) is evaluated first, giving you the actual map.
        }
    }

    iniFile.close();
}


inline void toLower(std::string& str) { 
    for (auto& c : str) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
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


