#pragma once
#include <unordered_set>
#include <unordered_map>

extern bool disableShadowCasters;
extern bool disableTorchLights;
extern bool removeFakeGlowOrbs;
extern bool enableColorConsistency;
extern bool dataHasLoaded;

extern uint8_t red;
extern uint8_t green;
extern uint8_t blue;

extern RE::FormID soulCairnFormID;

extern RE::FormID apocryphaFormID;

extern std::vector<std::string> whitelist;

extern std::vector<std::string> exclusionList;

extern std::vector<std::string> exclusionListPartialMatch;

extern std::unordered_set<RE::FormID> excludedLightFormIDs;

extern std::vector<std::string> priorityList;

extern std::unordered_map<std::string, std::string> baseMeshesAndTemplateToAttach;

extern std::unordered_map<std::string, std::string> keywordTemplateMap;

extern std::map<std::string, std::vector<RE::NiPointer<RE::NiNode>>> keywordNodeBank;

// defined as static const for potential caching
static const std::unordered_map<std::string, std::string> nordicHallMeshesAndTemplates = {
	{ "norcathallsm1way01", "Nordic Cata 1way01 Candles_NOT Animated.nif" },
	{ "norcathallsm1way02", "Nordic Cata 1way02 Candles_NOT Animated.nif" },
	{ "norcathallsm1way03", "Nordic Cata 1way03 Candles_NOT Animated.nif" },
	{ "norcathallsm2way01", "Nordic Cata 2way01 Candles_NOT Animated.nif" },
	{ "norcathallsm3way01", "Nordic Cata 3way01 Candles_NOT Animated.nif" },
	{ "norcathallsm3way02", "Nordic Cata 3way02 Candles_NOT Animated.nif" },
	{ "norcathallsm4way01", "Nordic Cata 4way01 Candles_NOT Animated.nif" },
	{ "norcathallsm4way02", "Nordic Cata 4way02 Candles_NOT Animated.nif" },
	{ "nortmphallbgcolumnsm01", "Nordic ColumnSM01 Candles_NOT Animated.nif" },
	{ "nortmphallbgcolumnsm02", "Nordic ColumnSM02 Candles_NOT Animated.nif" },
	{ "nortmphallbgcolumn01", "Nordic Column Candles 01 03_NOT Animated.nif" },
	{ "nortmphallbgcolumn03", "Nordic Column Candles 01 03_NOT Animated.nif"}
};

extern std::vector<std::vector<std::string>> keywordLightGroups;

extern std::vector<std::vector<std::string>> excludedCells;
/*

 //extern std::unordered_map<RE::TESModel*, std::string> ModelsAndOriginalFilePaths;
//extern std::array<std::string, 26> templateNames;

//extern std::unordered_set<RE::FormID> excludedFormIDs;

extern std::array<RE::FormID, 4> fireFormIDs; 

extern std::array<RE::FormID, 3> dungeonOutlierFiresIDs;

extern std::string fireTemplate; 

extern std::string originalModelPath; 

extern std::string forswornFires;
extern std::string vampireFires;
extern std::string dungeonFires;
extern std::string dragonPriestFires;

extern RE::BGSKeyword* keywordForswornCamp;
extern RE::BGSKeyword* keywordVampireLair;
extern RE::BGSKeyword* keywordDungeon;
extern RE::BGSKeyword* keywordDragonPriestLair;*/