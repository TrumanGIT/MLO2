#include "global.h"
#include <map>
#include <array>
#include <string>
#include <unordered_set>
#include <unordered_map>

int disableShadowCasters = 1;
int disableTorchLights = 1;

uint8_t red = 255;
uint8_t green = 161;
uint8_t blue = 60;


bool dataHasLoaded = false;

std::array<RE::FormID, 4> fireFormIDs = {0x00033DA4, 0x00033DA9, 0x000CD823, 0x000CD824};

std::array<RE::FormID, 3> dungeonOutlierFiresIDs = {
    0x0008A7FA,  // Bleak Falls Barrow Bandit Campfire
    0x000A45CA,  // Korvanjund Bandit Campfire
    0x000B715E   // Angarvunde Bandit Campfire
};  

std::vector<std::string> whitelist;

std::vector<std::string> exclusionList; 

std::vector<std::string> exclusionListPartialMatch;

std::vector<std::string> specialNodes = {
           "norcathallsm",
           "nortmphallbgcolumn03",
           "nortmphallbgcolumn01",
           "nortmphallbgcolumnsm02",
           "nortmphallbgcolumnsm01"
            };

std::unordered_map<RE::TESModel*, std::string> ModelsAndOriginalFilePaths;

// nodeName (lowercased) -> template mesh path
std::unordered_map<std::string, std::string> baseMeshesAndTemplateToAttach = {};

std::unordered_map<std::string, std::string> keywordTemplateMap = {};

std::map<std::string, std::array<RE::NiPointer<RE::NiNode>, 75>> keywordNodeBank = {};

std::string forswornFires = "";
std::string vampireFires = "";
std::string dungeonFires = "";
std::string dragonPriestFires = "";

std::string originalModelPath = "";

RE::BGSKeyword* keywordForswornCamp = nullptr;
RE::BGSKeyword* keywordVampireLair = nullptr;
RE::BGSKeyword* keywordDungeon = nullptr;
RE::BGSKeyword* keywordDragonPriestLair = nullptr;
