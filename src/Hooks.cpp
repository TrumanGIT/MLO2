#include "Hooks.h"
#include "Functions.h"
#include "global.h"
#include <map>
#include <array>
#include <string>
#include <unordered_set>


namespace Hooks {

    // Thiago99s

    /*bool Hooks::ReplaceTextureOnObjectsHook::ShouldBackgroundClone(RE::TESObjectREFR* ref) {
       if (ref){
           ProcessReference(ref);
       }
   return originalFunction(ref);
}

    void ReplaceTextureOnObjectsHook::Install() {
    logger::info("Installed ReplaceTextureOnObjectsHook");
        originalFunction =
            REL::Relocation<std::uintptr_t>(RE::TESObjectREFR::VTABLE[0]).write_vfunc(0x6D, ShouldBackgroundClone);
    }
    */

    //Po3's

    RE::NiPointLight* TESObjectLIGH_GenDynamic::thunk(RE::TESObjectLIGH* light, RE::TESObjectREFR* ref,
        RE::NiNode* node, bool forceDynamic, bool useLightRadius,
        bool affectRequesterOnly) {

        if (should_disable_light(light, ref)) return nullptr;

      

        if (light->data.flags.none(RE::TES_LIGHT_FLAGS::kOmniShadow,
            RE::TES_LIGHT_FLAGS::kHemiShadow,
            RE::TES_LIGHT_FLAGS::kSpotShadow))
        {
            light->data.color.red = red;
            light->data.color.green = green;
            light->data.color.blue = blue;

        }

        return func(light, ref, node, forceDynamic, useLightRadius, affectRequesterOnly);
   
    }
  
    void TESObjectLIGH_GenDynamic::Install() {
        std::array targets{
            std::make_pair(RELOCATION_ID(17206, 17603), 0x1D3),  // TESObjectLIGH::Clone3D
            std::make_pair(RELOCATION_ID(19252, 19678), 0xB8),   // TESObjectREFR::AddLight
        };

        for (const auto& [address, offset] : targets) {
            REL::Relocation<std::uintptr_t> target{ address, offset };
            write_thunk_call<TESObjectLIGH_GenDynamic>(target.address());
        }

        logger::info("Installed TESObjectLIGH::GenDynamic patch");
    }

    //Po3's

   void PostCreate::thunk(
    RE::TESModelDB::TESProcessor* a_this,
    const RE::BSModelDB::DBTraits::ArgsType& a_args,
    const char* a_nifPath,
    RE::NiPointer<RE::NiNode>& a_root,
    std::uint32_t& a_typeOut)
{
    if (!dataHasLoaded || !a_root) {
        // logger::info("no root or data hasn't loaded, cancelling hook (intentional)");
        return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
    }

    std::string nodeName = a_root->name.c_str();  // grab name of NiNode (usually 1:1 with mesh names)
    toLower(nodeName);                            // normalize case

    // Try specific meshes first
    if (!cloneAndAttachNodesForSpecificMeshes(nodeName, a_root)) {
        // Then check for keywords to cover broader cases (candle, lantern, etc.)
        auto match = matchedKeyword(nodeName);

        if (!match.empty()) {
            // Exclude exact matches
           if (isExclude(nodeName))
                return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

            // Handheld torches need special node placement so the light shuts off when extinguished
            if (TorchHandler(nodeName, a_root))
                return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

            // Some Nordic ruin meshes have candles embedded in walls that need special placement
            if (candlesFusedInWallMeshesHandler(nodeName, a_root))
                return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

            // General case
            RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank(match);

            a_root->AttachChild(nodePtr.get());
            // logger::info("attached light to keyword mesh {}", nodeName);
        }

        // Some nodenames are improperly named "dummy" by the base game
        dummyHandler(a_root.get(), nodeName);
    }

    // Always call original func if nothing handled
    func(a_this, a_args, a_nifPath, a_root, a_typeOut);
}



void PostCreate::Install() {
    // Get TESProcessor's vtable
    REL::Relocation<std::uintptr_t> vtable(RE::TESModelDB::TESProcessor::VTABLE[0]);

    // Replace the vfunc at index 'size' with our thunk
    func = vtable.write_vfunc(size, thunk);

    logger::info("Installed TESModelDB::TESProcessor hook");
}

    void Install() {
    SKSE::AllocTrampoline(1 << 8);
    TESObjectLIGH_GenDynamic::Install();
  //  ReplaceTextureOnObjectsHook::Install();
    PostCreate::Install();
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
}
