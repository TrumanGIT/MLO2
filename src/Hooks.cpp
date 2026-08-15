#include "Hooks.h"
#include "Functions.h"
#include "global.h"
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
    RE::NiPointLight* TESObjectLIGH_GenDynamic::thunk(
        RE::TESObjectLIGH* light,
        RE::TESObjectREFR* ref,
        RE::NiNode* node,
        bool forceDynamic,
        bool useLightRadius,
        bool affectRequesterOnly)
    {

       auto player = RE::PlayerCharacter::GetSingleton();

       auto currentCell = player->GetParentCell(); 
      
        if (!ref || !light)
            return func(light, ref, node, forceDynamic, useLightRadius, affectRequesterOnly);

        if (excludeLightEditorID(light))
            return func(light, ref, node, forceDynamic, useLightRadius, affectRequesterOnly);

        if (excludeByCellEditorID(currentCell))
            return func(light, ref, node, forceDynamic, useLightRadius, affectRequesterOnly);

        // get the name of the mod owning the light
        const RE::TESFile* refOriginFile = ref->GetDescriptionOwnerFile();
        std::string modName = refOriginFile ? refOriginFile->fileName : "";

		toLower(modName);

        if (should_disable_light(light, ref, modName))
            return nullptr;

        //doint want to change color of lights that change color based on time of day. 
        if (modName.find("window shadows ultimate") == std::string::npos && enableColorConsistency)
        {
            if (!excludeLightFromRGB(light)) {
                light->data.color.red = red;
                light->data.color.green = green;
                light->data.color.blue = blue;
            }
      
        }
        // else {
        //     logger::info("Excluding shadow caster or mod light {:X}", formID);
        // }

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

        if (!dataHasLoaded || !a_root || isMLOTemplatePath(a_nifPath)) {
            return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
        }

        const auto& nodeName = a_root->name;  // Preserve the scene node's original name.
        std::string normalizedNodeNameString = nodeName.c_str();
        toLower(normalizedNodeNameString);
        const RE::BSFixedString normalizedNodeName(normalizedNodeNameString);

        if (removeFakeGlowOrbs) {
            cullMPSGlow(nodeName, a_root.get());
        }


        if (missivesPatch(nodeName, a_root.get())) {
            return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
        }


        // Try specific meshes first
        if (cloneAndAttachNodesForSpecificMeshes(normalizedNodeName, a_root, a_nifPath)) {
          //  logger::info("attached {} template to {}", nodeName.c_str());
            return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
        }
        

        auto match = matchedKeyword(normalizedNodeName, a_nifPath);

        if (!match.empty() || normalizedNodeName.contains("nortmphallbgc") || normalizedNodeName.contains("norcathallsm") || normalizedNodeName.contains("scene")) {


            if (isExclude(normalizedNodeName, a_nifPath, a_root.get())) return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

            if (handleSceneRoot(a_nifPath, a_root, normalizedNodeNameString))
                return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
   
            if (removeFakeGlowOrbs)
                glowOrbRemover(a_root.get());

            if (TorchHandler(normalizedNodeNameString, a_nifPath, a_root))
                return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

            if (applyCorrectNordicHallTemplate(normalizedNodeNameString, a_root))
                return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

            if (!match.empty()) {
                if (attachKeywordParticleLight(match, normalizedNodeNameString, a_nifPath, a_root)) {
                    return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
                }
            }
          //  else {
               // logger::warn("Light node we tried to attach was null', skipping attachment", match);
          //  }
        }

        dummyHandler(a_root.get(), nodeName);

        // Always call original func if nothing handled
        return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
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
