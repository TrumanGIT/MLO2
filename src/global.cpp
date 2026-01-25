#include "global.h"
#include <map>
#include <array>
#include <string>
#include <unordered_set>
#include <unordered_map>

bool disableShadowCasters = true;
bool disableTorchLights = true;
bool removeFakeGlowOrbs = true;
bool enableColorConsistency = true;

uint8_t red = 255;
uint8_t green = 161;
uint8_t blue = 60;

bool dataHasLoaded = false;

RE::FormID soulCairnFormID = 0x2001408;
RE::FormID apocryphaFormID = 0x0401C0B2;

std::vector<std::string> whitelist;

std::vector<RE::BSFixedString> exclusionList; 

std::vector<RE::BSFixedString> exclusionListPartialMatch;

std::vector<RE::BSFixedString> priorityList = {};

// nodeName (lowercased) -> template mesh path
std::unordered_map<RE::BSFixedString, std::string>
    baseMeshesAndTemplateToAttach = {};

std::unordered_map<RE::BSFixedString, std::string> keywordTemplateMap = {};

std::unordered_map<RE::BSFixedString, std::vector<RE::NiPointer<RE::NiNode>>> keywordNodeBank = {};

std::vector<std::vector<std::string>> keywordLightGroups;

std::vector<std::vector<std::string>> RGBExcludedLights;

std::vector<std::vector<std::string>> excludedCells;

//std::unordered_map<RE::TESModel*, std::string> ModelsAndOriginalFilePaths;

/*std::string forswornFires = "";
std::string vampireFires = "";
std::string dungeonFires = "";
std::string dragonPriestFires = "";

std::string originalModelPath = "";

RE::BGSKeyword* keywordForswornCamp = nullptr;
RE::BGSKeyword* keywordVampireLair = nullptr;
RE::BGSKeyword* keywordDungeon = nullptr;
RE::BGSKeyword* keywordDragonPriestLair = nullptr;*/

// std::unordered_set<RE::FormID> excludedFormIDs = {
//      0x051471E7, 0x0508C86C, 0x0508BD01, 0x00105303,
//      0x00105300, 0x000F4851, 0x00094E6B, 0x00092ADD, 0x0009250B, 0x0008E7A0
//};

//std::array<RE::FormID, 4> fireFormIDs = {0x00033DA4, 0x00033DA9, 0x000CD823, 0x000CD824};

/*std::array<RE::FormID, 3> dungeonOutlierFiresIDs = {
    0x0008A7FA,  // Bleak Falls Barrow Bandit Campfire
    0x000A45CA,  // Korvanjund Bandit Campfire
    0x000B715E   // Angarvunde Bandit Campfire
};  */