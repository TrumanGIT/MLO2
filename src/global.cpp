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


std::map<std::array<std::string, 13>, RE::NiPointer<RE::NiNode>> baseMeshesAndNiNodeToAttach{
    
    // Candle_Animated.nif
{{
   "meshes/clutter/candles/candlehornfloor01.nif",
   "meshes/clutter/candles/candlehorntable01.nif",
   "meshes/clutter/candles/candlehornwall01.nif",
   "meshes/clutter/common/candlelanternwithcandle01.nif",
   "meshes/clutter/imperial/impcandelabracandle01.nif",
   "meshes/clutter/imperial/impcandelabracandle02.nif",
   "meshes/clutter/imperial/impcandle01.nif",
   "meshes/clutter/imperial/impwallsconce02candleon01.nif",
   "meshes/clutter/imperial/impwallsconcecandle01.nif",
   "meshes/clutter/imperial/impwallsconcecandle02.nif",
   "meshes/clutter/candles/glazedcandlesstatic02.nif",
   "meshes/clutter/candles/glazedcandles01.nif",
   "meshes/clutter/candles/mpscandleflame01.nif"
}, nullptr},


    // Chandelier_Animated.nif
    {{"meshes/clutter/candles/candlehornchandelier01.nif",
      "meshes/clutter/candles/candlehornchandelier02.nif",
      "meshes/clutter/imperial/impchandelliercandle01.nif",
      "meshes/clutter/imperial/impchandelliercandle02.nif"}, nullptr},

    // College Magic Fire Small_Animated.nif
    {{"meshes/effects/mgmagicfirepillarsmall.nif"}, nullptr},

    // College Magic Fire_Animated.nif
    {{"meshes/effects/mgmagicfirepillarmidden01.nif"}, nullptr},

    // College Magic Pillar_Animated.nif
    {{"meshes/effects/mgmagicfirepillar01.nif"}, nullptr},

    // Dark Elf Lantern_Animated.nif
    {{"meshes/dlc02/clutter/darkelflantern.nif"}, nullptr},  

    // Dwemer Chandelier Sputter_Animated.nif
    {{"meshes/dungeons/dwemer/dwechandelier01bareonsputter.nif",
      "meshes/dungeons/dwemer/dwechandelier01onsputter.nif",
      "meshes/dungeons/dwemer/dwechandelier01simpleonsputter.nif",
      "meshes/dungeons/dwemer/dwechandelier02onsputter.nif"}, nullptr},

    // Dwemer Chandelier_Animated.nif
    {{"meshes/dungeons/dwemer/dwechandelier01bareon.nif",
      "meshes/dungeons/dwemer/dwechandelier01on.nif",
      "meshes/dungeons/dwemer/dwechandelier01simpleon.nif",
      "meshes/dungeons/dwemer/dwechandelier02on.nif"}, nullptr},

    // Dwemer Light Sputter_Animated.nif
    {{"meshes/dungeons/dwemer/dwefloorlanternonsputter.nif",
      "meshes/dungeons/dwemer/dwewallsconce01onsputter.nif",
      "meshes/dungeons/dwemer/dwewallsconce02onsputter.nif",
      "meshes/dungeons/dwemer/dwewallsconcesmall01onsputter.nif",
      "meshes/dungeons/dwemer/dwewallsconcesmall02onsputter.nif",
      "meshes/effects/fxdwegreenflamesputter.nif"}, nullptr},

    // Dwemer Light_Animated.nif
    {{"meshes/dungeons/dwemer/dwefloorlanternon.nif",
      "meshes/dungeons/dwemer/dwewallsconce01on.nif",
      "meshes/dungeons/dwemer/dwewallsconce02on.nif",
      "meshes/dungeons/dwemer/dwewallsconcesmall01on.nif",
      "meshes/dungeons/dwemer/dwewallsconcesmall02on.nif",
      "meshes/effects/fxdwegreenflame.nif",
      "meshes/effects/fxdwegreenflamecalm.nif"}, nullptr},

         // Fires Blue_Animated.nif
    {{"meshes/effects/c06fxfirebluewithembersheavy.nif"}, nullptr},

    // Fires Small_Animated.nif
    {{"meshes/effects/fxfirewithembers01_cheap.nif",
      "meshes/effects/fxfirewithembers03.nif"}, nullptr},

    // Fires Sovngarde_Animated.nif
    {{"meshes/effects/fxfiresovngarde.nif"}, nullptr},

    // Fires_Animated.nif
    {{"meshes/effects/fxfirewithembers01.nif",
      "meshes/effects/fxfirewithembers01_lite.nif",
      "meshes/effects/fxfirewithembers02.nif",
      "meshes/effects/fxfirewithembers02toggle.nif",
      "meshes/effects/fxfirewithembersheavytoggle.nif",
      "meshes/effects/fxfirewithemberslogs01.nif",
      "meshes/clutter/woodfires/campfire01burning.nif",
      "meshes/clutter/giant/giantcampfire01burning.nif",
      "meshes/clutter/woodfires/fireplacewood01burning.nif",
      "meshes/clutter/woodfires/campfire01landburning.nif"}, nullptr},

   // Giant Fires_Animated.nif
    {{
    "meshes/clutter/giant/giantcampfire01burning.nif",
    "meshes/clutter/giant/giantcampfire01burningnoland.nif"
    }, nullptr},

// Karstaag Ice Fires_Animated.nif
    {{ "meshes/dlc02/effects/dlc2castlekarstaagtorch.nif" }, nullptr},

// Nocturnal Purple Fire_Animated.nif
    {{ "meshes/effects/tg09nocturnalfxfirepurple.nif" }, nullptr},

// Puzzle Purple Fire_Animated.nif
    {{ "meshes/dlc01/dungeons/castle/animated/cascoffinpuzzlebrazier/cascoffinpuzzlebrazier01.nif" }, nullptr},

// Torch Sconce_Animated.nif
    {{
    "meshes/clutter/common/removabletorch01.nif",
    "meshes/clutter/common/torchpermanent01.nif",
    "meshes/clutter/common/torchsconce01.nif"
    }, nullptr},

// Soul Cairn_Animated.nif
    {{
    "meshes/dlc01/soulcairn/sc_crater01.nif",
    "meshes/dlc01/soulcairn/sc_lightwell01.nif"
    }, nullptr},

// Apoc Trigger Lamp_Animated.nif
    {{ "meshes/dlc02/dungeons/apocrypha/animated/aposcryetrigger01/aposcryetrigger01.nif" }, nullptr},

// Apoc Lamp_Animated.nif
    {{
    "meshes/dlc02/dungeons/apocrypha/clutter/apolightpost01.nif",
    "meshes/dlc02/dungeons/apocrypha/clutter/apolightpost02.nif"
    }, nullptr},

// Apoc Head Sconce Green_Animated.nif
    {{
    "meshes/dlc02/dungeons/apocrypha/clutter/apoheadscone02.nif",
    "meshes/dlc02/dungeons/apocrypha/clutter/apomouthsconce02.nif"
    }, nullptr},

// Apoc Head Sconce Blue_Animated.nif
    {{
    "meshes/dlc02/dungeons/apocrypha/clutter/apoheadscone03.nif",
    "meshes/dlc02/dungeons/apocrypha/clutter/apomouthsconce03.nif"
    }, nullptr},
    
};

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
    "meshes/clutter/candles/candlehornfloor01.nif",
    "meshes/clutter/candles/candlehorntable01.nif",
    "meshes/clutter/candles/candlehornwall01.nif",
    "meshes/clutter/common/candlelanternwithcandle01.nif",
    "meshes/clutter/imperial/impcandelabracandle01.nif",
    "meshes/clutter/imperial/impcandelabracandle02.nif",
    "meshes/clutter/imperial/impcandle01.nif",
    "meshes/clutter/imperial/impwallsconce02candleon01.nif",
    "meshes/clutter/imperial/impwallsconcecandle01.nif",
    "meshes/clutter/imperial/impwallsconcecandle02.nif",

    "meshes/clutter/candles/candlehornchandelier01.nif",
    "meshes/clutter/candles/candlehornchandelier02.nif",
    "meshes/clutter/imperial/impchandelliercandle01.nif",
    "meshes/clutter/imperial/impchandelliercandle02.nif",

    "meshes/effects/mgmagicfirepillarsmall.nif",
    "meshes/effects/mgmagicfirepillarmidden01.nif",
    "meshes/effects/mgmagicfirepillar01.nif",
    "meshes/dlc02/clutter/darkelflantern.nif",

    "meshes/dungeons/dwemer/dwechandelier01bareonsputter.nif",
    "meshes/dungeons/dwemer/dwechandelier01onsputter.nif",
    "meshes/dungeons/dwemer/dwechandelier01simpleonsputter.nif",
    "meshes/dungeons/dwemer/dwechandelier02onsputter.nif",

    "meshes/dungeons/dwemer/dwechandelier01bareon.nif",
    "meshes/dungeons/dwemer/dwechandelier01on.nif",
    "meshes/dungeons/dwemer/dwechandelier01simpleon.nif",
    "meshes/dungeons/dwemer/dwechandelier02on.nif",

    "meshes/dungeons/dwemer/dwefloorlanternonsputter.nif",
    "meshes/dungeons/dwemer/dwewallsconce01onsputter.nif",
    "meshes/dungeons/dwemer/dwewallsconce02onsputter.nif",
    "meshes/dungeons/dwemer/dwewallsconcesmall01onsputter.nif",
    "meshes/dungeons/dwemer/dwewallsconcesmall02onsputter.nif",
    "meshes/effects/fxdwegreenflamesputter.nif",

    "meshes/dungeons/dwemer/dwefloorlanternon.nif",
    "meshes/dungeons/dwemer/dwewallsconce01on.nif",
    "meshes/dungeons/dwemer/dwewallsconce02on.nif",
    "meshes/dungeons/dwemer/dwewallsconcesmall01on.nif",
    "meshes/dungeons/dwemer/dwewallsconcesmall02on.nif",
    "meshes/effects/fxdwegreenflame.nif",
    "meshes/effects/fxdwegreenflamecalm.nif",

    "meshes/effects/c06fxfirebluewithembersheavy.nif",

    "meshes/effects/fxfirewithembers01_cheap.nif",
    "meshes/effects/fxfirewithembers03.nif",

    "meshes/effects/fxfiresovngarde.nif",

    "meshes/effects/fxfirewithembers01.nif",
    "meshes/effects/fxfirewithembers01_lite.nif",
    "meshes/effects/fxfirewithembers02.nif",
    "meshes/effects/fxfirewithembers02toggle.nif",
    "meshes/effects/fxfirewithembersheavytoggle.nif",
    "meshes/effects/fxfirewithemberslogs01.nif",
    "meshes/clutter/woodfires/campfire01burning.nif",
    "meshes/clutter/giant/giantcampfire01burning.nif",
    "meshes/clutter/woodfires/fireplacewood01burning.nif",
    "meshes/clutter/woodfires/campfire01landburning.nif",

    "meshes/clutter/giant/giantcampfire01burning.nif",
    "meshes/clutter/giant/giantcampfire01burningnoland.nif",

    "meshes/dlc02/effects/dlc2castlekarstaagtorch.nif",
    "meshes/effects/tg09nocturnalfxfirepurple.nif",
    "meshes/dlc01/dungeons/castle/animated/cascoffinpuzzlebrazier/cascoffinpuzzlebrazier01.nif",

    "meshes/clutter/common/removabletorch01.nif",
    "meshes/clutter/common/torchpermanent01.nif",
    "meshes/clutter/common/torchsconce01.nif",

    "meshes/dlc01/soulcairn/sc_crater01.nif",
    "meshes/dlc01/soulcairn/sc_lightwell01.nif",

    "meshes/dlc02/dungeons/apocrypha/animated/aposcryetrigger01/aposcryetrigger01.nif",

    "meshes/dlc02/dungeons/apocrypha/clutter/apolightpost01.nif",
    "meshes/dlc02/dungeons/apocrypha/clutter/apolightpost02.nif",

    "meshes/dlc02/dungeons/apocrypha/clutter/apoheadscone02.nif",
    "meshes/dlc02/dungeons/apocrypha/clutter/apomouthsconce02.nif",

    "meshes/dlc02/dungeons/apocrypha/clutter/apoheadscone03.nif",
    "meshes/dlc02/dungeons/apocrypha/clutter/apomouthsconce03.nif"
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