#pragma once
//#include "Manager.h"
//#include "SaveLoadManager.h"

namespace Hooks {

    struct ReplaceTextureOnObjectsHook {
        static inline REL::Relocation<bool(RE::TESObjectREFR*)> originalFunction;
        static bool ShouldBackgroundClone(RE::TESObjectREFR* ref);

        static void Install();
    };


    struct TESObjectLIGH_GenDynamic {
        static RE::NiPointLight* thunk(RE::TESObjectLIGH* light, RE::TESObjectREFR* ref, RE::NiNode* node,
                                       bool forceDynamic, bool useLightRadius, bool affectRequesterOnly);

        static inline REL::Relocation<decltype(thunk)> func;
        static void Install();
    };

         void Install();
   }


