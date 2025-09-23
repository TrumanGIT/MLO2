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

        auto refOriginFile = ref->GetDescriptionOwnerFile();

        if (std::string(refOriginFile->fileName) != "Window Shadows Ultimate.esp") {
            light->data.color.red = 255;
            light->data.color.green = 161;
            light->data.color.blue = 60;
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
            return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
            logger::info("no root or data hasent loaded cancelling hook(intentional)");
        }

        std::string nodeName = a_root->name.c_str(); // grab name of ni node usually 1:1 with mesh names thank god. 
        toLower(nodeName);                           //some nodes are called dummy you can see in dummyhandler() how i deal with that. (thx bethesda)
    //    logger::info("incoming node = {}", nodeName);
        if (!cloneAndAttachNodesForSpecificMeshes(nodeName, a_root)) { // look for specific meshes first

            auto match = matchedKeyword(nodeName); // then check for keywords to cover a large net ie candle, lantern ect

            if (!match.empty()) {

                dummyHandler(a_root.get(), nodeName); // then deal with dummy nodes. I should mention,
                                                      // file paths with this hook are always null except for loose files. thats why we check by node name. 
                                                      //     
                if (nodeName != "lantern") { // nodes with just lantern are a empty lantern so we need to exclude them getting light

                    RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank(match);
                    AttachNodeToMesh(a_root.get(), nodePtr.get());
                }
            }
        }

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


