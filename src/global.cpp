#include "global.h"
#include <map>
#include <array>
#include <string>
#include <unordered_set>
#include <unordered_map>

int disableShadowCasters = 1;
int disableTorchLights = 1;

int red = 255;
int green = 161; 
int blue = 60; 

bool dataHasLoaded = false;

std::array<RE::FormID, 4> fireFormIDs = {0x00033DA4, 0x00033DA9, 0x000CD823, 0x000CD824};

std::array<RE::FormID, 3> dungeonOutlierFiresIDs = {
    0x0008A7FA,  // Bleak Falls Barrow Bandit Campfire
    0x000A45CA,  // Korvanjund Bandit Campfire
    0x000B715E   // Angarvunde Bandit Campfire
};  

std::vector<std::string> whitelist;

std::unordered_map<RE::TESModel*, std::string> ModelsAndOriginalFilePaths;

// nodeName (lowercased) -> template mesh path
std::unordered_map<std::string, std::string> baseMeshesAndTemplateToAttach = {


    {"mgmagicfirepillarsmall", "College Magic Fire Small_Animated.nif"},
    {"mgmagicfirepillarmidden01", "College Magic Fire_Animated.nif"},
    {"mgmagicfirepillar01", "College Magic Pillar_Animated.nif"},
    {"darkelflantern", "Dark Elf Lantern_Animated.nif"},

        {"impcandelabracandle01.nif", "Candle Bra_NOT Animated.nif"},
    {"impcandelabracandle02.nif", "Candle Bra_NOT Animated.nif"},  // order matters these need to load before the keywords
    {"candlehornfloor01", "Candle Bra_NOT Animated.nif"},

    {"dwefloorlanternon", "Dwemer Light_Animated.nif"},
    {"dwewallsconce01on", "Dwemer Light_Animated.nif"},//  these need to load before the keywords left half of their variants to keywords
    {"dwewallsconce02on", "Dwemer Light_Animated.nif"},
    {"dwewallsconcesmall01on", "Dwemer Light_Animated.nif"},
    {"dwewallsconcesmall02on", "Dwemer Light_Animated.nif"},
    {"fxdwegreenflame", "Dwemer Light_Animated.nif"},
    {"fxdwegreenflamecalm", "Dwemer Light_Animated.nif"},

    {"c06fxfirebluewithembersheavy", "Fires Blue_Animated.nif"},
    {"fxfirewithembers01_cheap", "Fires Small_Animated.nif"},//  these need to load before the keywords
    {"fxfirewithembers03", "Fires Small_Animated.nif"},
    {"fxfiresovngarde", "Fires Sovngarde_Animated.nif"},

    {"giantcampfire01burningnoland", "Giant Fires_Animated.nif"}, //  these need to load before the keywords

    {"dlc2castlekarstaagtorch", "Karstaag Ice Fires_Animated.nif"},
    {"tg09nocturnalfxfirepurple", "Nocturnal Purple Fire_Animated.nif"},
    {"cascoffinpuzzlebrazier01", "Puzzle Purple Fire_Animated.nif"}, 

    {"sc_crater01", "Soul Cairn_Animated.nif"},
    {"sc_lightwell01", "Soul Cairn_Animated.nif"},
    {"aposcryetrigger01", "Apoc Trigger Lamp_Animated.nif"},

    {"apolightpost01", "Apoc Lamp_Animated.nif"},
    {"apolightpost02", "Apoc Lamp_Animated.nif"},
    {"apoheadscone02", "Apoc Head Sconce Green.nif"},
    {"apomouthsconce02", "Apoc Head Sconce Green.nif"},
    {"apoheadscone03", "Apoc Head Sconce Blue.nif"},
    {"apomouthsconce03", "Apoc Head Sconce Blue.nif"}
};


std::unordered_map<std::string, std::string> keywordTemplateMap = {
    {"chandeliers", "Chandeliers_NOT Animated.nif"},
    {"candle", "Candles_NOT Animated.nif"},
    {"fxfirew", "Fires_Animated.nif"},
    {"campfire", "Fires_Animated.nif"},
    {"fireplacewood", "Fires_Animated.nif"},
    {"firepit","Fires_Animated.nif"},
        {"torchsconce", "Torch Sconce_Animated.nif"},
    {"torch", "Hand Held Torch_Animated.nif"},
    {"lantern", "Candles_NOT Animated.nif"},
    {"dwec", "Dwemer Chandelier Sputter_Animated.nif"},
    {"dwewalls", "Dwemer Light Sputter_Animated.nif"},
   
};

std::map<std::string, std::array<RE::NiPointer<RE::NiNode>, 75>> keywordNodeBank = {
    {"chandeliers", {}}, // "Chandeliers_NOT_Animated.nif"
    {"candle", {}},      // "Candles_NOT_Animated.nif"
    {"fxfirew", {}},     // "Fires_Animated.nif"
    {"campfire", {}},    // "Fires_Animated.nif"
    {"fireplacewood", {}},
    {"firepit", {}},// "Fires_Animated.nif"
        {"torchsconce", {}}, // "Torch Sconce_Animated.nif"    
    {"torch", {}}, // Hand Held Torch_Animated.nif
    {"lantern", {}},     // "Candles_NOT_Animated.nif"
    {"dwec", {}},        // "Dwemer Chandelier Sputter_Animated.nif"
    {"dwewalls", {}},   // "Dwemer Light Sputter_Animated.nif"
    { "dummy", {}},
   
     
};

std::string forswornFires = "";
std::string vampireFires = "";
std::string dungeonFires = "";
std::string dragonPriestFires = "";

std::string originalModelPath = "";

RE::BGSKeyword* keywordForswornCamp = nullptr;
RE::BGSKeyword* keywordVampireLair = nullptr;
RE::BGSKeyword* keywordDungeon = nullptr;
RE::BGSKeyword* keywordDragonPriestLair = nullptr;

 
