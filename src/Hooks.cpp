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

        if (!dataHasLoaded || !a_root) {
            return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
        }

        RE::BSFixedString& nodeName = a_root->name;  // grab name of NiNode (usually 1:1 with mesh names)

        if (removeFakeGlowOrbs) {
            cullMPSGlow(nodeName, a_root.get());
        }


        if (missivesPatch(nodeName, a_root.get())) {
            return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
        }


        // Try specific meshes first
        if (cloneAndAttachNodesForSpecificMeshes(nodeName, a_root, a_nifPath)) {
          //  logger::info("attached {} template to {}", nodeName.c_str());
            return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
        }
        

        auto match = matchedKeyword(nodeName);

        if (!match.empty() || nodeName.contains("nortmphallbgc") || nodeName.contains("norcathallsm") || nodeName.contains("scene")) {


            if (isExclude(nodeName, a_nifPath, a_root.get())) return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

            std::string nodeNameStr = nodeName.c_str();

            toLower(nodeNameStr);

            if (handleSceneRoot(a_nifPath, a_root, nodeNameStr))
                return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
   
            if (removeFakeGlowOrbs)
                glowOrbRemover(a_root.get());

            if (TorchHandler(nodeNameStr, a_root))
                return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

            if (applyCorrectNordicHallTemplate(nodeNameStr, a_root))
                return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

            RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank(match);
            if (nodePtr) { // scene is apart of the nodebank but we do not want to attach nodes for scene. 
                a_root->AttachChild(nodePtr.get());
                logger::info("attached {} light to keyword mesh {}", match, nodeName);
                return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
             
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