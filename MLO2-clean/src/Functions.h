#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include "global.h"
#include <fstream>
#include <unordered_map>
#include <map>
#include <sstream>


namespace logger = SKSE::log;


 inline void Initialize() {
     logger::info("loading forms");
    auto dataHandler = RE::TESDataHandler::GetSingleton(); 

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

 inline RE::NiPointer<RE::NiNode> cloneNiNode(
    std::string templatePath) {  // what if i use a map here to append the correct one 

RE::NiPointer<RE::NiNode> loaded;
        auto args = RE::BSModelDB::DBTraits::ArgsType();
    
        auto result = RE::BSModelDB::Demand(templatePath.c_str(), loaded, args);
       
        if (result == RE::BSResource::ErrorCode::kNone && loaded) {

            if (loaded) {
                logger::info("Loaded root node type: {}", loaded->GetRTTI()->GetName());
                logger::info("Loaded root children count: {}", loaded->children.size());
                if (!loaded->children.empty() && loaded->children[0]) {
                    auto firstChild = loaded->children[0]->AsNode();
                    logger::info("First child node type: {}", firstChild->GetRTTI()->GetName());
                    logger::info("First child children count: {}", firstChild->children.size());
                }
            }

            auto fadeNode = loaded->AsFadeNode();
            if (fadeNode && fadeNode->children.size() > 0) {
                auto desiredNode = fadeNode->children[0];
                if (desiredNode) {
                    auto tempPrototype = desiredNode->AsNode();
                    if (tempPrototype) {
                        RE::NiCloningProcess cloningProcess{};
                        auto cloneBase = tempPrototype->CreateClone(cloningProcess);
                        if (cloneBase) {
                           auto yourGlowNodePrototype = cloneBase->AsNode();
                            if (yourGlowNodePrototype) {
                                return RE::NiPointer<RE::NiNode>(yourGlowNodePrototype);; 
                                logger::info("Successfully extracted and cloned prototype from NIF!");
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
                        logger::warn("Desired node was not a NiNode");
                         return nullptr; 
                    }
                }
                else {
                    logger::warn("Desired node was null");
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

inline void assignClonedNodes() {
    logger::info("assigning cloned nodes... total groups: {}", baseMeshesAndNiNodeToAttach.size());

    std::string prefix = "Meshes\\NEW\\SSE\\";
    int templateFilePathIterator = 0;

    for (auto& [formIDs, nodePtr] : baseMeshesAndNiNodeToAttach) {
        logger::info("Processing group #{} with {} formIDs", templateFilePathIterator, formIDs.size());

        // Print the formIDs in this group
        for (auto id : formIDs) {
            logger::info("  -> formID: {:08X}", id);
        }

        if (templateFilePathIterator >= templateNames.size()) {
            logger::warn("Ran out of template names for map entries at index {}", templateFilePathIterator);
            break;
        }

        auto templatePath = prefix + templateNames[templateFilePathIterator];
        logger::info("Loading template path [{}]", templatePath);

        auto clonedNode = cloneNiNode(templatePath);

        if (clonedNode) {
            logger::info("  ✔ Successfully cloned node for [{}], assigning to group #{}", templatePath, templateFilePathIterator);
        } else {
            logger::error("  ✖ Failed to clone node for [{}], leaving nodePtr as nullptr", templatePath);
        }

        nodePtr = clonedNode;  // store one copy for the group
        templateFilePathIterator++;
    }
    logger::info("Finished assignClonedNodes");
}

// this is only if we need to overwrite 6 bits of memory instead of the default 5.... currently not used ive tried so many hooks xD 
template <class T, std::size_t size = 5>  
inline void write_thunk_call(std::uintptr_t a_src) {
    auto& trampoline = SKSE::GetTrampoline();
    if (size == 6) {
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
// heres where we attach the node failed becasue get3d isent ready in the latest hook clone3d()
inline void AttachNodeToReference(RE::TESObjectREFR* ref, RE::NiNode* nodeToAttach) {
    if (!ref) {
        logger::info("AttachNodeToReference: ref is null");
        return;
    }
    if (!nodeToAttach) {
        logger::info("AttachNodeToReference: nodeToAttach is null");
        return;
    }

    RE::NiAVObject* root3D = ref->Get3D();
    if (!root3D) {
        logger::info("AttachNodeToReference: ref.Get3D() returned null");
        return;
    }

    RE::NiNode* rootNode = root3D->AsNode();
    if (!rootNode) {
        logger::info("AttachNodeToReference: root3D is not a NiNode");
        return;
    }

    rootNode->AttachChild(nodeToAttach);
    logger::info("AttachNodeToReference: Node attached successfully");
}

// this is the core of the mod where we change meshes by adding glow nodes from nidify templates or swap fire colors based on players current location

inline void ProcessReference(RE::TESObjectREFR* a_ref) {
    const auto refid = a_ref->GetFormID();
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

    for (auto& [formIDs, nodePtr] : baseMeshesAndNiNodeToAttach) {
        if (std::find(formIDs.begin(), formIDs.end(), baseFormID) != formIDs.end()) {
            // the map says its a mesh that needs light from the template
            AttachNodeToReference(a_ref, nodePtr.get());
            break;
        }

        auto player = RE::PlayerCharacter::GetSingleton();
        if (!player) return;
            // change fires based on location
        RE::BGSLocation* currentLocation = player->GetCurrentLocation();
        if (!currentLocation) return;

        if (currentLocation->HasKeyword(keywordVampireLair)) {
            colorSwitch = vampireFires;
        } else if (currentLocation->HasKeyword(keywordForswornCamp)) {
            colorSwitch = forswornFires;
        } else if (currentLocation->HasKeyword(keywordDungeon)) {
            colorSwitch = dungeonFires;
        } else if (currentLocation->HasKeyword(keywordDragonPriestLair)) {
            colorSwitch = dragonPriestFires;
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

        if (const auto bm = base->As<RE::TESModel>()) {  // Change the fire to correct color and save the original model
                                                         // path so we can restore later.
            auto originalModelPath = bm->GetModel();

            auto foundModel = ModelsAndOriginalFilePaths.find(bm);

            if (foundModel == ModelsAndOriginalFilePaths.end()) { // no duplicates
                ModelsAndOriginalFilePaths[bm] = originalModelPath;
            }

            logger::info("Applying model path {} to ref {:X}", colorSwitch, refid);
            bm->SetModel(colorSwitch.c_str());
        }
    }
}

inline void splitString(const std::string& input, char delimter, std::vector<std::string>& listToSplit) {
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, delimter)) {
        while (!item.empty() && std::isspace(item.front())) {
            item.erase(item.begin());  // begin() =  (position)  front() = (value) erase takes a pos i dident know this
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
            disableShadowCasters = std::stoi(line.substr(std::string("disableShadowCasters=").length()));
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
        }

        else if (line.starts_with("whitelist=")) {
            std::string prefix = "whitelist=";

            line.erase(0, prefix.length());

            splitString(line, ',', whitelist);
        }
    }
}
