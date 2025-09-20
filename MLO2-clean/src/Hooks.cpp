#include "Hooks.h"
#include "Functions.h"
#include "global.h"

namespace Hooks {

     bool Hooks::ReplaceTextureOnObjectsHook::ShouldBackgroundClone(RE::TESObjectREFR* ref) {
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

    RE::NiPointLight* TESObjectLIGH_GenDynamic::thunk(RE::TESObjectLIGH* light, RE::TESObjectREFR* ref,
                                                      RE::NiNode* node, bool forceDynamic, bool useLightRadius,
                                                      bool affectRequesterOnly) {

        if (should_disable_light(light, ref)) return nullptr;

        light->data.color.red = 255; 

        light->data.color.green = 161;

        light->data.color.blue = 60;  

        return func(light, ref, node, forceDynamic, useLightRadius, affectRequesterOnly);
    }

    void TESObjectLIGH_GenDynamic::Install() {
        std::array targets{
            std::make_pair(RELOCATION_ID(17206, 17603), 0x1D3),  // TESObjectLIGH::Clone3D
            std::make_pair(RELOCATION_ID(19252, 19678), 0xB8),   // TESObjectREFR::AddLight
        };

        for (const auto& [address, offset] : targets) {
            REL::Relocation<std::uintptr_t> target{address, offset};
            write_thunk_call<TESObjectLIGH_GenDynamic>(target.address());
        }

        logger::info("Installed TESObjectLIGH::GenDynamic patch");
    }

    void Install() {
        SKSE::AllocTrampoline(1 << 8);
        TESObjectLIGH_GenDynamic::Install();
        ReplaceTextureOnObjectsHook::Install();
    }

};
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        