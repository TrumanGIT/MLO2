#include "Hooks.h"
#include "Functions.h"
#include "global.h"
#include <map>
#include <array>
#include <string>
#include <unordered_set>

// ADD                             = Candles_NOT Animated.nif to keywords
// ADD                  = Fires Blue_Animated.nif to specific
//add candlestick to excludes

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

        if (disableTorchLights == 1 &&
            light->data.flags.any(RE::TES_LIGHT_FLAGS::kCanCarry))
        {
            // disable lights that can be carried (i.e., torches)
            return nullptr;
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

    std::string nodeName = a_root->name.c_str();  // grab name of ni node (usually 1:1 with mesh names)
    toLower(nodeName);                            // normalize case

    // Try specific meshes first
    if (!cloneAndAttachNodesForSpecificMeshes(nodeName, a_root)) {
        // Then check for keywords to cover broader cases (candle, lantern, etc.)
        auto match = matchedKeyword(nodeName);

        if (!match.empty()) {

            for (const auto& exclude : exclusionList) {
                if (nodeName == exclude) {
                    //   logger::info("skipping exclude {}", exclude);
                    return func(a_this, a_args, a_nifPath, a_root, a_typeOut);;
                }
            }

            for (const auto& exclude : exclusionListPartialMatch) {
                if (nodeName.find(exclude) != std::string::npos) { // must compare to npos!
                    return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
                }
            }

            static const std::vector<std::string> specialNodes = {
           "norcathallsm",
           "nortmphallbgcolumn03",
           "nortmphallbgcolumn01",
           "nortmphallbgcolumnsm02",
           "nortmphallbgcolumnsm01"
            };

            for (const auto& special : specialNodes) {
                if (nodeName.find(special) != std::string::npos) {
                    for (auto& child : a_root->children) {
                        if (child) {
                            auto childAsNode = child->AsNode();
                            if (childAsNode) {
                                std::string childName = child->name.c_str();
                                if (childName.find("Glow") != std::string::npos) {
                                    RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank("candle");
                                    childAsNode->AttachChild(nodePtr.get());
                                   // logger::info("Attached candle node to {}", childName);
                                }
                            }
                        }
                    }
                    // Done handling this node, call original
                    return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
                }
            }

            if (nodeName == "torch") {
            //    logger::info("torch found");

                RE::NiNode* attachLight = nullptr;

                // Find torchFire node
                RE::NiNode* torchFire = nullptr;
                for (auto& child : a_root->children) {
                    if (auto childNode = child->AsNode()) {
                        if (childNode->name == "TorchFire") {
                            torchFire = childNode;
                            break;
                        }
                    }
                }

                if (torchFire) {
                    // Find attachLight node inside torchFire
                    for (auto& child : torchFire->children) {
                        if (auto childNode = child->AsNode()) {
                            if (childNode->name == "AttachLight") {
                                attachLight = childNode;
                                break;
                            }
                        }
                    }
                }

                if (attachLight) {
                    RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank("torch");
                    attachLight->AttachChild(nodePtr.get());
                  //  logger::info("attached light to torch at specific spot {}", nodeName);
                //    DumpFullTree(a_root.get());
                    return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
                }
                else {
                    logger::warn("hand held torch light placement failed failed for {}", nodeName);
                }
            }
            // General case
                RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank(match);
                
                if (!nodePtr) {
                    cloneAndAttachNodesForPartialMeshes(nodeName, a_root);
                }

                a_root->AttachChild(nodePtr.get());
                // logger::info("attached light to keyword mesh {}", nodeName);
        }
        // Handle dummy nodes if no keyword match
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


