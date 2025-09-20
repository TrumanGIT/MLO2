#pragma once

extern float disableShadowCasters;
extern float disableTorchLights; 

extern std::array<RE::FormID, 4> fireFormIDs; 

extern std::array<RE::FormID, 3> dungeonOutlierFiresIDs;

extern std::vector<std::string> whitelist;

extern std::vector<std::string> templateNames;

extern std::unordered_map<RE::TESModel*, std::string> ModelsAndOriginalFilePaths;

extern std::map<std::vector<RE::FormID>, RE::NiPointer<RE::NiNode>> baseMeshesAndNiNodeToAttach;

extern std::string fireTemplate; 

extern std::string colorSwitch;

extern std::string originalModelPath; 

extern std::string forswornFires;
extern std::string vampireFires;
extern std::string dungeonFires;
extern std::string dragonPriestFires;

extern RE::BGSKeyword* keywordForswornCamp;
extern RE::BGSKeyword* keywordVampireLair;
extern RE::BGSKeyword* keywordDungeon;
extern RE::BGSKeyword* keywordDragonPriestLair;
