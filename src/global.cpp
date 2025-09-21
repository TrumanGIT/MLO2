#include "global.h"
#include <map>
#include <array>
#include <string>
#include <unordered_set>

float disableShadowCasters = 1.0f;
float disableTorchLights = 1.0f;

std::array<RE::FormID, 4> fireFormIDs = {0x00033DA4, 0x00033DA9, 0x000CD823, 0x000CD824};

std::array<RE::FormID, 3> dungeonOutlierFiresIDs = {
    0x0008A7FA,  // Bleak Falls Barrow Bandit Campfire
    0x000A45CA,  // Korvanjund Bandit Campfire
    0x000B715E   // Angarvunde Bandit Campfire
};  

std::vector<std::string> whitelist;

std::unordered_map<RE::TESModel*, std::string> ModelsAndOriginalFilePaths;

// nodeName (lowercased) -> template mesh path
std::unordered_map<std::string, std::string> baseMeshesAndNiNodeToAttach = {
    {"candlehornfloor01", "Candle_Animated.nif"},
    {"candlehorntable01", "Candle_Animated.nif"},
    {"candlehornwall01", "Candle_Animated.nif"},
    {"candlelanternwithcandle01", "Candle_Animated.nif"},
    {"impcandelabracandle01", "Candle_Animated.nif"},
    {"impcandelabracandle02", "Candle_Animated.nif"},
    {"impcandle01", "Candle_Animated.nif"},
    {"impwallsconce02candleon01", "Candle_Animated.nif"},
    {"impwallsconcecandle01", "Candle_Animated.nif"},
    {"impwallsconcecandle02", "Candle_Animated.nif"},
    {"glazedcandlesstatic02", "Candle_Animated.nif"},
    {"glazedcandles01", "Candle_Animated.nif"},
    {"mpscandleflame01.nif", "Candle_Animated.nif"},

    {"candlehornchandelier01", "Chandelier_Animated.nif"},
    {"candlehornchandelier02", "Chandelier_Animated.nif"},
    {"impchandelliercandle01", "Chandelier_Animated.nif"},
    {"impchandelliercandle02", "Chandelier_Animated.nif"},

    {"mgmagicfirepillarsmall", "College Magic Fire Small_Animated.nif"},
    {"mgmagicfirepillarmidden01", "College Magic Fire_Animated.nif"},
    {"mgmagicfirepillar01", "College Magic Pillar_Animated.nif"},

    {"darkelflantern", "Dark Elf Lantern_Animated.nif"},

    {"dwechandelier01bareonsputter", "Dwemer Chandelier Sputter_Animated.nif"},
    {"dwechandelier01onsputter", "Dwemer Chandelier Sputter_Animated.nif"},
    {"dwechandelier01simpleonsputter", "Dwemer Chandelier Sputter_Animated.nif"},
    {"dwechandelier02onsputter", "Dwemer Chandelier Sputter_Animated.nif"},

    {"dwechandelier01bareon", "Dwemer Chandelier_Animated.nif"},
    {"dwechandelier01on", "Dwemer Chandelier_Animated.nif"},
    {"dwechandelier01simpleon", "Dwemer Chandelier_Animated.nif"},
    {"dwechandelier02on", "Dwemer Chandelier_Animated.nif"},

    {"dwefloorlanternonsputter", "Dwemer Light Sputter_Animated.nif"},
    {"dwewallsconce01onsputter", "Dwemer Light Sputter_Animated.nif"},
    {"dwewallsconce02onsputter", "Dwemer Light Sputter_Animated.nif"},
    {"dwewallsconcesmall01onsputter", "Dwemer Light Sputter_Animated.nif"},
    {"dwewallsconcesmall02onsputter", "Dwemer Light Sputter_Animated.nif"},
    {"fxdwegreenflamesputter", "Dwemer Light Sputter_Animated.nif"},

    {"dwefloorlanternon", "Dwemer Light_Animated.nif"},
    {"dwewallsconce01on", "Dwemer Light_Animated.nif"},
    {"dwewallsconce02on", "Dwemer Light_Animated.nif"},
    {"dwewallsconcesmall01on", "Dwemer Light_Animated.nif"},
    {"dwewallsconcesmall02on", "Dwemer Light_Animated.nif"},
    {"fxdwegreenflame", "Dwemer Light_Animated.nif"},
    {"fxdwegreenflamecalm", "Dwemer Light_Animated.nif"},

    {"c06fxfirebluewithembersheavy", "Fires Blue_Animated.nif"},
    {"fxfirewithembers01_cheap", "Fires Small_Animated.nif"},
    {"fxfirewithembers03", "Fires Small_Animated.nif"},
    {"fxfiresovngarde", "Fires Sovngarde_Animated.nif"},

    {"fxfirewithembers01", "Fires_Animated.nif"},
    {"fxfirewithembers01_lite", "Fires_Animated.nif"},
    {"fxfirewithembers02", "Fires_Animated.nif"},
    {"fxfirewithembers02toggle", "Fires_Animated.nif"},
    {"fxfirewithembersheavytoggle", "Fires_Animated.nif"},
    {"fxfirewithemberslogs01", "Fires_Animated.nif"},
    {"campfire01burning", "Fires_Animated.nif"},
    {"giantcampfire01burning", "Fires_Animated.nif"},
    {"fireplacewood01burning", "Fires_Animated.nif"},
    {"campfire01landburning", "Fires_Animated.nif"},

    {"giantcampfire01burningnoland", "Giant Fires_Animated.nif"},

    {"dlc2castlekarstaagtorch", "Karstaag Ice Fires_Animated.nif"},
    {"tg09nocturnalfxfirepurple", "Nocturnal Purple Fire_Animated.nif"},
    {"cascoffinpuzzlebrazier01", "Puzzle Purple Fire_Animated.nif"},

    {"removabletorch01", "Torch Sconce_Animated.nif"},
    {"torchpermanent01", "Torch Sconce_Animated.nif"},
    {"torchsconce01", "Torch Sconce_Animated.nif"},

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


/*
std::array<std::string, 26> templateNames = {
   "Candle_Animated.nif",
    "Chandelier_Animated.nif",
    "College Magic Fire Small_Animated.nif",
    "College Magic Fire_Animated.nif",
    "College Magic Pillar_Animated.nif",
    "Dark Elf Lantern_Animated.nif",
    "Dwemer Chandelier Sputter_Animated.nif",
    "Dwemer Chandelier_Animated.nif",
    "Dwemer Light Sputter_Animated.nif",
    "Dwemer Light_Animated.nif",
    "Fires Blue_Animated.nif",
    "Fires Small_Animated.nif",
    "Fires Sovngarde_Animated.nif",
    "Fires_Animated.nif",
   "Giant Fires_Animated.nif",
    "Karstaag Ice Fires_Animated.nif",
    "Nocturnal Purple Fire_Animated.nif",
    "Puzzle Purple Fire_Animated.nif",
    "Torch Sconce_Animated.nif",
    "Soul Cairn_Animated.nif",
    "Apoc Trigger Lamp_Animated.nif",
    "Apoc Lamp_Animated.nif",
    "Apoc Head Sconce Green.nif",
    "Apoc Head Sconce Blue.nif",
    "College Magic Light_Animated.nif",
    "Hand Held Torch_Animated.nif"
}; 

std::unordered_set<std::string> allMeshes{
    "candlehornfloor01",
    "candlehorntable01",
    "candlehornwall01",
    "glazedcandlesstatic02",
    "glazedcandles01",
    "mpscandleflame01.nif",
    "candlelanternwithcandle01",
    "impcandelabracandle01",
    "impcandelabracandle02",
    "impcandle01",
    "impwallsconce02candleon01",
    "impwallsconcecandle01",
    "impwallsconcecandle02",
    "candlehornchandelier01",
    "candlehornchandelier02",
    "impchandelliercandle01",
    "impchandelliercandle02",
    "mgmagicfirepillarsmall",
    "mgmagicfirepillarmidden01",
    "mgmagicfirepillar01",
    "darkelflantern",
    "dwechandelier01bareonsputter",
    "dwechandelier01onsputter",
    "dwechandelier01simpleonsputter",
    "dwechandelier02onsputter",
    "dwechandelier01bareon",
    "dwechandelier01on",
    "dwechandelier01simpleon",
    "dwechandelier02on",
    "dwefloorlanternonsputter",
    "dwewallsconce01onsputter",
    "dwewallsconce02onsputter",
    "dwewallsconcesmall01onsputter",
    "dwewallsconcesmall02onsputter",
    "fxdwegreenflamesputter",
    "dwefloorlanternon",
    "dwewallsconce01on",
    "dwewallsconce02on",
    "dwewallsconcesmall01on",
    "dwewallsconcesmall02on",
    "fxdwegreenflame",
    "fxdwegreenflamecalm",
    "c06fxfirebluewithembersheavy",
    "fxfirewithembers01_cheap",
    "fxfirewithembers03",
    "fxfiresovngarde",
    "fxfirewithembers01",
    "fxfirewithembers01_lite",
    "fxfirewithembers02",
    "fxfirewithembers02toggle",
    "fxfirewithembersheavytoggle",
    "fxfirewithemberslogs01",
    "campfire01burning",
    "giantcampfire01burning",
    "fireplacewood01burning",
    "campfire01landburning",
    "giantcampfire01burningnoland",
    "dlc2castlekarstaagtorch",
    "tg09nocturnalfxfirepurple",
    "cascoffinpuzzlebrazier01",
    "removabletorch01",
    "torchpermanent01",
    "torchsconce01",
    "sc_crater01",
    "sc_lightwell01",
    "aposcryetrigger01",
    "apolightpost01",
    "apolightpost02",
    "apoheadscone02",
    "apomouthsconce02",
    "apoheadscone03",
    "apomouthsconce03"
};*/
    
std::string forswornFires = "";
std::string vampireFires = "";
std::string dungeonFires = "";
std::string dragonPriestFires = "";

std::string originalModelPath = "";

RE::BGSKeyword* keywordForswornCamp = nullptr;
RE::BGSKeyword* keywordVampireLair = nullptr;
RE::BGSKeyword* keywordDungeon = nullptr;
RE::BGSKeyword* keywordDragonPriestLair = nullptr;

int dataHasLoaded = 0; 
