#pragma once
#include <spdlog/sinks/basic_file_sink.h>
#include "ClibUtil/EditorID.hpp"
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


inline void splitString(const std::string& input, char delimiter, std::vector<std::string>& listToSplit)
{
    std::stringstream ss(input);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        // Trim leading whitespace
        while (!item.empty() && std::isspace(static_cast<unsigned char>(item.front()))) {
            item.erase(item.begin());
        }

        // Trim trailing whitespace
        while (!item.empty() && std::isspace(static_cast<unsigned char>(item.back()))) {
            item.pop_back();
        }

        listToSplit.push_back(item);
        spdlog::info("Added '{}' to whitelist", item);
    }
}

inline bool containsAll(std::string ID, const std::vector<std::string>& group)
{
    toLower(ID);
    for (auto g : group) {
        if (ID.find(g) == std::string::npos)
            return false;
    }
    return true;
}

// Try to exclude light by editorID.
inline bool excludeLightEditorID(const RE::TESObjectLIGH* light) {

    std::string edid = clib_util::editorID::get_editorID(light);

    if (!edid.empty()) {
        for (const auto& group : keywordLightGroups) {
            if (containsAll(edid, group)) {
                logger::info("Excluding light by editorID: {}", edid);
                return true;
            }
        }
    }

    return false;
}

inline bool excludeLightFromRGB(const RE::TESObjectLIGH* light) {

    std::string edid = clib_util::editorID::get_editorID(light);

    if (!edid.empty()) {
        for (const auto& group : RGBExcludedLights) {
            if (containsAll(edid, group)) {
                logger::info("Excluding light by editorID: {}", edid);
                return true;
            }
        }
    }

    return false;
}

inline bool excludeByCellEditorID(const RE::TESObjectCELL* cell) {

    if (!cell) {
        return false;
    }


    std::string edid = clib_util::editorID::get_editorID(cell);

    if (!edid.empty()) {
        for (const auto& group : excludedCells) {
            if (containsAll(edid, group)) {
                logger::info("Excluding light by editorID: {}", edid);
                return true;
            }
        }
    }

    return false;
}

inline std::vector<std::string> SplitKeywordsByComma(const std::string& line) {
    std::vector<std::string> out;

    for (size_t start = 0; start < line.size();) {
        size_t commaPos = line.find(',', start);
        std::string keyword = (commaPos == std::string::npos) ? line.substr(start) : line.substr(start, commaPos - start);

        keyword = trim(keyword);
        if (!keyword.empty()) {
            toLower(keyword);
            out.push_back(std::move(keyword));
        }

        if (commaPos == std::string::npos)
            break;

        start = commaPos + 1;
    }

    return out;
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
                disableTorchLights = true;
            else if (value == "false" || value == "0")
                disableTorchLights = false;
            else
                spdlog::warn("Invalid value for disableTorchLights: {}", value);

            spdlog::info("INI override: disableTorchLights = {}", disableTorchLights);
        }
        else if (line.starts_with("removeFakeGlowOrbs=")) {
            std::string value = line.substr(std::string("removeFakeGlowOrbs=").length());
            toLower(value);

            if (value == "true" || value == "1")
                removeFakeGlowOrbs = true;
            else if (value == "false" || value == "0")
                removeFakeGlowOrbs = false;
            else
                spdlog::warn("Invalid value for removeFakeGlowOrbs: {}", value);

            spdlog::info("INI override: removeFakeGlowOrbs = {}", removeFakeGlowOrbs);
        }
        else if (line.starts_with("enableColorConsistency=")) {
            std::string value = line.substr(std::string("enableColorConsistency=").length());
            toLower(value);

            if (value == "true" || value == "1")
                enableColorConsistency = true;
            else if (value == "false" || value == "0")
                enableColorConsistency = false;
            else
                spdlog::warn("Invalid value for enableColorConsistency {}", value);

            spdlog::info("INI override: enableColorConsistency = {}", enableColorConsistency);
        }

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

            toLower(line);

            line.erase(0, prefix.length());

            splitString(line, ',', whitelist);
        }

        else if (line.starts_with("loggingLevel")) {
            std::string value = line.substr(std::string("loggingLevel=").length());

             auto loggingLevel = std::stoi(value);
            loggingLevel = std::clamp(loggingLevel, 0, 3);
            logger::info("Logging level set to {}", loggingLevel);
            spdlog::level::level_enum user_level = spdlog::level::info;
            switch (loggingLevel) {
            case 0:
            {
                user_level = spdlog::level::critical;
                break;
            }
            case 1:
            {
                user_level = spdlog::level::warn;
                break;
            }
            case 2:
            {
                user_level = spdlog::level::info;
                break;
            }
            case 3:
            {
                user_level = spdlog::level::debug;
                break;
            }
            }
            spdlog::set_level(user_level);
            spdlog::flush_on(user_level);
        }

    }
}

inline void ReadMasterListAndFillMaps()
{
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
    std::unordered_map<RE::BSFixedString, std::string>* mapPtr = &baseMeshesAndTemplateToAttach;

    bool readingPartialMeshes = false;
    bool excludeByEditorID = false;
    bool excludeRGBByEditorID = false;
    bool excludeByCell = false;

    while (std::getline(iniFile, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
   
        if (line.starts_with(";")) {

            readingPartialMeshes = false;
            excludeByEditorID = false;
            excludeRGBByEditorID = false;
            excludeByCell = false;

            if (line.find("keyword") != std::string::npos) {
                mapPtr = &keywordTemplateMap;
                readingPartialMeshes = true;
            }
            else if (line.find("EXCLUDE BY EDITOR ID") != std::string::npos) {
                excludeByEditorID = true;
            }
            else if (line.find("EXCLUDE LIGHT FROM RGB CHANGES BY EDITOR ID") != std::string::npos) {
                excludeRGBByEditorID = true;
            }
            else if (line.find("EXCLUDE BY CELL EDITOR ID") != std::string::npos) {
                excludeByCell = true;
            }
            else if (line.find("EXCLUDE SPECIFIC NODES BY NAME") != std::string::npos) {
                mapPtr = &baseMeshesAndTemplateToAttach;
            }

            continue;
        }

        if (excludeRGBByEditorID) {
            std::vector<std::string> keywords = SplitKeywordsByComma(line);

            keywords.erase(
                std::remove_if(keywords.begin(), keywords.end(),
                    [](const std::string& s) { return s.empty(); }),
                keywords.end()
            );

            if (!keywords.empty()) {
                RGBExcludedLights.push_back(std::move(keywords));
            }

            continue; 
        }

        if (excludeByEditorID) {
            std::vector<std::string> keywords = SplitKeywordsByComma(line);

            keywords.erase(
                std::remove_if(keywords.begin(), keywords.end(),
                    [](const std::string& s) { return s.empty(); }),
                keywords.end()
            );

            if (!keywords.empty()) {
                keywordLightGroups.push_back(std::move(keywords));
            }

            continue;
        }


        if (excludeByCell) {
            std::vector<std::string> keywords = SplitKeywordsByComma(line);

            keywords.erase(
                std::remove_if(keywords.begin(), keywords.end(),
                    [](const std::string& s) { return s.empty(); }),
                keywords.end()
            );

            if (!keywords.empty()) {
                excludedCells.push_back(std::move(keywords));
            }

            continue; 
        }


        auto pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        toLower(key);

        (*mapPtr)[key] = value;

        if (readingPartialMeshes) {
            keywordNodeBank[key] = {};
            priorityList.push_back(key);
            logger::info("Partial search and its template: {} = {}", key, value);
        }
        else {
            logger::info("Specific mesh and its template: {} = {}", key, value);
        }
    }

    iniFile.close();

    logger::info("Excluded keywords in Editor ID:");
    for (const auto& keywords : keywordLightGroups) {
        for (const auto& k : keywords) {
            logger::info("{}", k);
        }
        logger::info("");
    }

    logger::info("Excluded Cells By Editor ID:");
    for (const auto& keywords : excludedCells) {
        for (const auto& k : keywords) {
            logger::info("{}", k);
        }
        logger::info("");
    }


    logger::info("Excluded Lights from RBG Changes By Editor ID:");
    for (const auto& keywords : RGBExcludedLights) {
        for (const auto& k : keywords) {
            logger::info("{}", k);
        }
        logger::info("");
    }
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
    int section = 0; // 0=skip, 1=exact excludes, 2=partial excludes
    while (std::getline(iniFile, line)) {
        line = trim(line);
        if (line.empty())
            continue;
        if (line.starts_with(";")) {
            if (line.find("EXCLUDE SPECIFIC NODES BY NAME") != std::string::npos) {
                section = 1;
            }
            else if (line.find("EXCLUDE PARTIAL NODES BY NAME") != std::string::npos) {
                section = 2;
            }
            else if (line.find("EXCLUDE BY EDITOR ID") != std::string::npos ||
                line.find("EXCLUDE BY CELL EDITOR ID") != std::string::npos ||
                line.find("EXCLUDE LIGHT FROM RGB") != std::string::npos) {
                section = 0; // stop processing excludes
            }
            continue;
        }
        if (section == 1) {
            toLower(line);
            exclusionList.push_back(line);
            logger::info("Added exact exclude: '{}'", line);
        }
        else if (section == 2) {
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
        return false;
    }
    auto worldspace = player->GetWorldspace();
    if (!worldspace) {
        // logger::info("worldSpace not valid cant get location");
        return false;  // Not in a worldspace (probably in an interior cell)
    }

    // logger::info("current worldspace = {}", worldspace->GetFormID());

    if (worldspace->GetFormID() == apocryphaFormID || worldspace->GetFormID() == soulCairnFormID) {
        //  logger::info("is in soul cairn or apocrypha");
        return true;
    }

    return false;
}

//clones a ni node from a nif template on disk
inline RE::NiPointer<RE::NiNode> cloneNiNode(const std::string& templatePath) {
    RE::NiPointer<RE::NiNode> loaded;
    auto args = RE::BSModelDB::DBTraits::ArgsType();

    auto result = RE::BSModelDB::Demand(templatePath.c_str(), loaded, args);
    if (result != RE::BSResource::ErrorCode::kNone || !loaded) {
        logger::warn("Could not find template {}", templatePath);
        return nullptr;
    }

    auto fadeNode = loaded->AsNode();
    if (!fadeNode || fadeNode->GetChildren().empty() || !fadeNode->GetChildren()[0]) {
        logger::warn("Template Fade node is missing, has no children, or first child is null");
        return nullptr;
    }

    auto firstChild = fadeNode->GetChildren()[0]->AsNode();
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
inline void glowOrbRemover(RE::NiNode* node)
{
    if (!node)
        return;

    // Copy raw pointers to avoid iterator invalidation
    std::vector<RE::NiAVObject*> childrenCopy;
    childrenCopy.reserve(node->GetChildren().size());

    for (auto& c : node->GetChildren()) {
        childrenCopy.push_back(c.get());
    }

    for (auto& child : childrenCopy) {
        if (!child)
            continue;

        auto childAsNode = child->AsNode();
        if (!childAsNode)
            continue;

        std::string name = childAsNode->name.c_str();
        toLower(name);

        if (name.find("glow") != std::string::npos) {
            childAsNode->SetAppCulled(true);
            continue;
        }

        // Recursive call to handle nested nodes
        glowOrbRemover(childAsNode);
    }
}

inline void cullMPSGlow(RE::BSFixedString nodeName, RE::NiNode* root) {
    if (nodeName == "MPSCandleFlame01.nif" && removeFakeGlowOrbs) {

        if (!root)
            return;

        // this is to remove glow orbs from Master particle system candles
        if (auto* flameNode = root->GetObjectByName("mpscandleflame01")) {
            if (auto* flameNiNode = flameNode->AsNode()) {

                if (!flameNiNode) return;
                if (auto* glowEmitter = flameNiNode->GetObjectByName("CandleGlow01-Emitter")) {
                    if (auto* emitterNode = glowEmitter->AsNode()) {
                        emitterNode->SetAppCulled(true);
                        logger::info("Culled CandleGlow01 emitter safely (no iteration)");
                        return;
                    }
                }
            }
        }
    }
}


inline bool isExclude(const RE::BSFixedString& nodeName, const char* nifPath, RE::NiNode* root)
{
    
    // Exact matches in exclusion list
    for (const auto& exclude : exclusionList) {
        if (nodeName == exclude)
            return true;
    }

    // Partial matches in exclusion list
    for (const auto& exclude : exclusionListPartialMatch) {
        if (nodeName.contains(exclude))
            return true;
    }


    if (!nifPath)
        return false;


    std::string path = nifPath;
    toLower(path);

    // Some modded torches name "off" variants incorrectly
    if (path.find("off") != std::string::npos)
        return true;

    return false;
}

// finds if a incoming node name matches any of our partial search keywords
inline RE::BSFixedString matchedKeyword(const RE::BSFixedString& nodeName)
{

    for (const auto& keyword : priorityList) {
        if (nodeName.contains(keyword)) {
            return keyword;
        }
    }

    return {};
}

//we clone and store ni nodes in a bank on startup to help with performance 
inline RE::NiPointer<RE::NiNode> getNextNodeFromBank(const RE::BSFixedString& keyword)
{
    auto it = keywordNodeBank.find(keyword);

    if (it == keywordNodeBank.end() || it->second.empty()) {
        logger::warn("getNextNodeFromBank: '{}' has no nodes available", keyword);
        return nullptr;
    }

    auto& bank = it->second; // keywords nodebank array

    // static here means initialised once and map contents survive each call (personal note)
    static std::unordered_map<RE::BSFixedString, std::size_t> counters;
    auto& count = counters[keyword];                                 // index for the next node to use in bank

    if (count >= bank.size())
        count = 0; // resets bank to 0 if we passed the limit (likely fine to recycle the nodes)

    RE::NiPointer<RE::NiNode> node = bank[count];

    if (!node) {
      //  logger::warn("getNextNodeFromBank: '{}' node index {} is null", keyword.c_str(), count);
        return nullptr;
    }

    count++;

    return node;
}


inline bool cloneAndAttachNodesForSpecificMeshes(const RE::BSFixedString& nodeName, RE::NiPointer<RE::NiNode>& a_root, const char* nifPath) {

    auto it = baseMeshesAndTemplateToAttach.find(nodeName);

    if (it != baseMeshesAndTemplateToAttach.end()) {

        if (isExclude(nodeName, nifPath, a_root.get())) return true;

        std::string fullPath = "Meshes\\MLO\\Templates\\" + it->second;

        auto nodeClone = cloneNiNode(fullPath);
        if (!nodeClone) {
            logger::warn("Failed to clone from template for {}", nodeName);
            return false;
        }

        a_root->AttachChild(nodeClone.get());
        //logger::warn("attached node to specific mesh {} ", nodeName);

        if (removeFakeGlowOrbs) {
            glowOrbRemover(a_root.get());
        }

        return true;
    }

    return false;
}

// on startup store a bunch of cloned nodes so we dont have to clone from disk during gameplay
inline void assignClonedNodesToBank() {
    logger::info("Assigning cloned nodes... total groups: {}", keywordNodeBank.size());

    const std::string prefix = "Meshes\\MLO\\Templates\\";

    for (auto& [keyword, bankedNodes] : keywordNodeBank) {
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
                bankedNodes.push_back(clonedNode);
            }
            else {
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
    }
    else {
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
    if (disableShadowCasters == false &&
        light->data.flags.any(RE::TES_LIGHT_FLAGS::kOmniShadow,
            RE::TES_LIGHT_FLAGS::kHemiShadow, RE::TES_LIGHT_FLAGS::kSpotShadow))
    {
        return false;
    }

    if (disableTorchLights == false &&
        light->data.flags.any(RE::TES_LIGHT_FLAGS::kCanCarry))
    {
        return false;
    }

    for (const auto& whitelistedMod : whitelist) {
        if (modName.find(whitelistedMod) != std::string::npos) {
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

inline bool missivesPatch(RE::BSFixedString nodeName, RE::NiNode* root) {
    if (nodeName == "MissiveBoard_new.nif") {
        if (!root)
            return true;

        // this is to remove glow orbs from Master particle system candles
        if (auto* candleNode = root->GetObjectByName("CandleLanternWithCandle")) {
            if (auto* candleNiNode = candleNode->AsNode()) {

                RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank("candle");
                if (nodePtr) {
                    candleNiNode->AttachChild(nodePtr.get());
                    return true;
                }
            }
        }
    }


    else if (nodeName == "DiverseMissiveBoard") {
    
        std::string templatePath = "Meshes\\MLO\\Templates\\DiverseMissiveBoard_NOT Animated.nif";

        RE::NiPointer<RE::NiNode> loaded;
        auto args = RE::BSModelDB::DBTraits::ArgsType();

        auto result = RE::BSModelDB::Demand(templatePath.c_str(), loaded, args);
        if (result != RE::BSResource::ErrorCode::kNone || !loaded) {
            logger::warn("Failed to load NIF file {}", templatePath);
            return false;
        }

        auto fadeNode = loaded->AsNode();
        if (!fadeNode) {
            logger::warn("Loaded NIF has no root node: {}", templatePath);
            return false;
        }

        RE::NiCloningProcess cloneProc;

        for (const auto& child : fadeNode->GetChildren()) {
            if (!child) {
                return true;
            }

            auto childAsNode = child->AsNode();
            if (childAsNode) {
                auto clone = childAsNode->CreateClone(cloneProc);
                if (clone) {
                    root->AttachChild(clone->AsNode());
                }
            }
        }
    }

    else if (nodeName == "DiverseMissiveBoardRuins") {

        std::string templatePath = "Meshes\\MLO\\Templates\\ DiverseMissiveBoardRuins_NOT Animated.nif";

        RE::NiPointer<RE::NiNode> loaded;
        auto args = RE::BSModelDB::DBTraits::ArgsType();

        auto result = RE::BSModelDB::Demand(templatePath.c_str(), loaded, args);
        if (result != RE::BSResource::ErrorCode::kNone || !loaded) {
            logger::warn("Failed to load NIF file {}", templatePath);
            return false;
        }

        auto fadeNode = loaded->AsNode();
        if (!fadeNode) {
            logger::warn("Loaded NIF has no root node: {}", templatePath);
            return false;
        }

        RE::NiCloningProcess cloneProc;

        for (const auto& child : fadeNode->GetChildren()) {
            if (!child) {
                return true;
            }

            auto childAsNode = child->AsNode();
            if (childAsNode) {
                auto clone = childAsNode->CreateClone(cloneProc);
                if (clone) {
                    root->AttachChild(clone->AsNode());
                }
            }
        }
    }

}

// torches need special placement of light so they dont light up when not equipped. 
inline bool TorchHandler(const std::string& nodeName, RE::NiPointer<RE::NiNode>& a_root)

{
    if (nodeName == "torch") {
        RE::NiNode* attachLight = nullptr;
        RE::NiNode* torchFire = nullptr;


        auto ui = RE::UI::GetSingleton();

        if (ui && ui->IsMenuOpen("InventoryMenu")) {
            //logger::info("Inventory menu is open, skipping PostCreate processing"); // do we even need that? 
            return true;
        }

        // must null check everything or crash city. 

        for (auto& child : a_root->GetChildren()) {
            if (!child) continue; // 
            auto childNode = child->AsNode();
            if (childNode && childNode->name == "TorchFire") {
                torchFire = childNode;
                break;
            }
        }

        if (torchFire) {
            for (auto& child : torchFire->GetChildren()) {
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

inline bool applyCorrectNordicHallTemplate(std::string nodeName, RE::NiPointer<RE::NiNode>& a_root)
{
    auto it = nordicHallMeshesAndTemplates.find(nodeName);
    if (it == nordicHallMeshesAndTemplates.end() || it->second.empty()) {
        return false;
    }

    std::string templatePath = "Meshes\\MLO\\Templates\\" + it->second;

    RE::NiPointer<RE::NiNode> loaded;
    auto args = RE::BSModelDB::DBTraits::ArgsType();

    auto result = RE::BSModelDB::Demand(templatePath.c_str(), loaded, args);
    if (result != RE::BSResource::ErrorCode::kNone || !loaded) {
        logger::warn("Failed to load NIF file {}", templatePath);
        return false;
    }

    auto fadeNode = loaded->AsNode();
    if (!fadeNode) {
        logger::warn("Loaded NIF has no root node: {}", templatePath);
        return false;
    }

    RE::NiCloningProcess cloneProc;

    for (const auto& child : fadeNode->GetChildren()) {
        if (!child) {
            return true;
        }

        auto childAsNode = child->AsNode();
        if (childAsNode) {
            auto clone = childAsNode->CreateClone(cloneProc);
            if (clone) {
                a_root->AttachChild(clone->AsNode());
            }
        }
    }
    return true;
}

// some nodes are called scene root this is to take care of them. 
inline bool handleSceneRoot(const char* nifPath, RE::NiPointer<RE::NiNode>& a_root, std::string nodeName)
{
    if (nodeName.find("scene") == std::string::npos)
        return false;

    if (!nifPath) {
        return true;
    }

    std::string path = nifPath;

    toLower(path);

   // logger::info("scene root node detected, checking path: {}", path);

    std::string bankType;

    if (path.find("candlehornfloor") != std::string::npos || path.find("mwcandle01") != std::string::npos)
    {
        bankType = "candlehornfloor01";
    }


    else if (path.find("candle") != std::string::npos)
    {
        logger::info("handleSceneRootByPath: matched candlehorntable/wall or mwcandle01 in path");
        bankType = "candle";
    }

    else
    {
        return true; // not a relevant mesh
    }

    RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank(bankType);

    if (nodePtr) {
        if (removeFakeGlowOrbs)
            glowOrbRemover(a_root.get());
        a_root->AttachChild(nodePtr.get());
        logger::info("Attached '{}' node to '{}'", bankType, a_root->name.c_str());
        return true;
    }
    else
    {
        logger::warn("handleSceneRootByPath: Attach target or nodePtr was null for '{}'", bankType);
        return true;
    }
}
// some nodes are called dummy this is to take care of them.
inline void dummyHandler(RE::NiNode* root,const RE::BSFixedString& nodeName)
{
    // Only operate on nodes whose own name contains "dummy"

    if (!nodeName.contains("dummy"))
        return;

    if (removeFakeGlowOrbs)
        glowOrbRemover(root);

    // Search children for a NiNode whose name contains "candle"
    for (auto& child : root->GetChildren()) {
        if (!child) continue;

        auto childAsNode = child->AsNode();
        if (!childAsNode) {
        //    logger::info("dummy handler: child of dummy node could not be cast AsNode()");
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
        for (auto& child : node->GetChildren()) {
            DumpFullTree(child.get(), depth + 1);
        }
    }
}