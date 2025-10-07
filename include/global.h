#pragma once
#include <unordered_set>
#include <unordered_map>

extern int disableShadowCasters;
extern int disableTorchLights;

extern uint8_t red;
extern uint8_t green;
extern uint8_t blue;

extern bool dataHasLoaded;

extern std::array<RE::FormID, 4> fireFormIDs; 

extern std::array<RE::FormID, 3> dungeonOutlierFiresIDs;

extern std::array<std::string, 26> templateNames;

extern std::vector<std::string> whitelist;

extern std::vector<std::string> exclusionList;

extern std::vector<std::string> specialNodes;

extern std::vector<std::string> exclusionListPartialMatch;

extern std::unordered_map<RE::TESModel*, std::string> ModelsAndOriginalFilePaths;

extern std::unordered_map<std::string, std::string> baseMeshesAndTemplateToAttach;

extern std::unordered_map<std::string, std::string> keywordTemplateMap;

extern std::map<std::string, std::array<RE::NiPointer<RE::NiNode>, 75>> keywordNodeBank;

extern std::unordered_set<std::string> allMeshes; 

extern std::string fireTemplate; 

extern std::string originalModelPath; 

extern std::string forswornFires;
extern std::string vampireFires;
extern std::string dungeonFires;
extern std::string dragonPriestFires;

extern RE::BGSKeyword* keywordForswornCamp;
extern RE::BGSKeyword* keywordVampireLair;
extern RE::BGSKeyword* keywordDungeon;
extern RE::BGSKeyword* keywordDragonPriestLair;
