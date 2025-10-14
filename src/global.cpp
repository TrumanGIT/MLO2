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


RE::FormID soulCairnFormID = 0x2001408;
RE::FormID apocryphaFormID = 0x0401C0B2;



 std::vector<RE::FormID> excludedSunlightFormIDs = {
        0x0001950C, 0x000172C4, 0x0002EDEC, 0x0002EDED, 0x0002EDF5, 0x0002EDF7,
        0x0002F37B, 0x00036AA4, 0x000395FE, 0x0003C5A5, 0x0003C5A6, 0x0003CDFB,
        0x0006ED65, 0x00075489, 0x000838A6, 0x000877A6, 0x00088245, 0x00088246,
        0x00088247, 0x00088248, 0x00088249, 0x0008824A, 0x0008824B, 0x0008E79A,
        0x0008E7DC, 0x00094E6B, 0x00094E6E, 0x000958C7, 0x000B7475, 0x000B79AF,
        0x000B7DBC, 0x000B7DE5, 0x000B7DE6, 0x000B872E, 0x000C59DA, 0x000C59DB,
        0x000C6527, 0x000650C, 0x000FFF46, 0x000FFF48
 };
 
std::vector<std::string> whitelist;

std::vector<std::string> exclusionList; 

std::vector<std::string> exclusionListPartialMatch;

std::array<std::string, 2> specialNodes = {
           "nortmphallbgcolumn03",
           "nortmphallbgcolumn01",
            };

std::vector<std::string> priorityList = {};

std::unordered_map<RE::TESModel*, std::string> ModelsAndOriginalFilePaths;

// nodeName (lowercased) -> template mesh path
std::unordered_map<std::string, std::string> baseMeshesAndTemplateToAttach = {};

std::unordered_map<std::string, std::string> keywordTemplateMap = {};

std::map<std::string, std::array<RE::NiPointer<RE::NiNode>, 75>> keywordNodeBank = {};

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
