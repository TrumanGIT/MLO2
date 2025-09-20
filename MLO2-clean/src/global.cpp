#include "global.h"
#include <map>
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

std::map<std::vector<RE::FormID>, RE::NiPointer<RE::NiNode>> baseMeshesAndNiNodeToAttach{
    //// Fires Blue_Animated.nif
    {{0x999153C8},  // C06FXfireBlueWithEmbersHeavy
     nullptr},

    //// Fires Small_Animated.nif
    {{
         0x0003BD2E,  // fxfirewithembers03
         0x000CD824   // FXfireWithEmbers01_Cheap
     },
     nullptr},

    //// Fires Sovngarde_Animated.nif
    {{0x001094AB},  // fxfiresovngarde.nif
     nullptr},

    //// Fires_Animated.nif
    {{
         0x0106112,   // FXfireWithEmbersHeavySCALED
         0x000CD823,  // fxFireWithEmbers01_light
         0x000AA71C,  // FXfireWithEmbers02Toggle
         0x001092E2,  // fxFireWithEmbersHeavyToggle
         0x0004318B,  // fxFireWithEmbersLogs01
         0x00033DA4,  // fxFireHeavy
         0x00033DA9,  // fxFireLight
         0x00035F47,  // Campfire01LandBurning
         0x000A9230,  // Campfire01LandBurning_heavy_SN
         0x00035F49,  // Campfire01Burning
         0x00035F4A   // FireplaceWood01Burning
     },
     nullptr}};


std::vector<std::string> templateNames = {
  /* "Candle_Animated.nif",
    "Chandelier_Animated.nif",
    "College Magic Fire Small_Animated.nif",
    "College Magic Fire_Animated.nif",
    "College Magic Pillar_Animated.nif",
    "Dark Elf Lantern_Animated.nif",
    "Dwemer Chandelier Sputter_Animated.nif",
    "Dwemer Chandelier_Animated.nif",
    "Dwemer Light Sputter_Animated.nif",
    "Dwemer Light_Animated.nif",*/ 
    "Fires Blue_Animated.nif",
    "Fires Small_Animated.nif",
    "Fires Sovngarde_Animated.nif",
    "Fires_Animated.nif",
   /*"Giant Fires_Animated.nif",
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
    "Hand Held Torch_Animated.nif"*/ 
}; 
    
std::string forswornFires = "";
std::string vampireFires = "";
std::string dungeonFires = "";
std::string dragonPriestFires = "";

std::string colorSwitch = "";

std::string originalModelPath = "";

RE::BGSKeyword* keywordForswornCamp = nullptr;
RE::BGSKeyword* keywordVampireLair = nullptr;
RE::BGSKeyword* keywordDungeon = nullptr;
RE::BGSKeyword* keywordDragonPriestLair = nullptr;