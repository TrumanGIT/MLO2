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
   RE::NiPointLight* TESObjectLIGH_GenDynamic::thunk(
    RE::TESObjectLIGH* light,
    RE::TESObjectREFR* ref,
    RE::NiNode* node,
    bool forceDynamic,
    bool useLightRadius,
    bool affectRequesterOnly)
{
    // Validate pointers early
    if (!ref || !light)
        return func(light, ref, node, forceDynamic, useLightRadius, affectRequesterOnly);

    auto base = ref->GetBaseObject();
    if (!base)
        return func(light, ref, node, forceDynamic, useLightRadius, affectRequesterOnly);

    RE::FormID formID = base->GetFormID();
    if (!formID)
        return func(light, ref, node, forceDynamic, useLightRadius, affectRequesterOnly);

    // get the name of the mod owning the light
    const RE::TESFile* refOriginFile = ref->GetDescriptionOwnerFile();
    std::string modName = refOriginFile ? refOriginFile->fileName : "";

    // Early exits: don't disable sunlight or touch RaceMenu lights
    if (std::find(excludedSunlightFormIDs.begin(), excludedSunlightFormIDs.end(), formID) != excludedSunlightFormIDs.end())/* ||
        modName.find("RaceMenu") != std::string::npos)*/
    {
        // logger::info("Excluded light from {} (FormID {:X})", modName, formID);
        return func(light, ref, node, forceDynamic, useLightRadius, affectRequesterOnly);
    }
    
    if (should_disable_light(light, ref, modName))
        return nullptr;

    // Only modify RGB values for lights that are NOT shadow casters
    if (
        modName.find("EnhancedLightsandFX") == std::string::npos &&
        modName.find("ELFX") == std::string::npos &&
        modName.find("Window Shadows Ultimate") == std::string::npos &&
        modName.find("Lux") == std::string::npos)
    {
        light->data.color.red   = 255;
        light->data.color.green = 161;
        light->data.color.blue  = 60;

        logger::info("Changed color for light {:X} from {}", formID, modName);
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
        auto ui = RE::UI::GetSingleton();

        if (!dataHasLoaded || !a_root) {
            return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
        }

        if (ui && ui->IsMenuOpen("InventoryMenu")) {
			logger::info("Inventory menu is open, skipping PostCreate processing"); // do we even need that? 
            return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
        }
        
        std::string nodeName = a_root->name.c_str();  // grab name of NiNode (usually 1:1 with mesh names)
        toLower(nodeName);

        // Try specific meshes first
        if (!cloneAndAttachNodesForSpecificMeshes(nodeName, a_root)) {
            auto match = matchedKeyword(nodeName);

            if (!match.empty()) {

                if (isExclude(nodeName)) return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

                if (TorchHandler(nodeName, a_root))
                    return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

                if (candlesFusedInWallMeshesHandler(nodeName, a_root))
                    return func(a_this, a_args, a_nifPath, a_root, a_typeOut);

                if (handleSceneRoot(a_root.get(), nodeName))
                    return func(a_this, a_args, a_nifPath, a_root, a_typeOut);
                
                RE::NiPointer<RE::NiNode> nodePtr = getNextNodeFromBank(match);
                if (nodePtr && match != "scene") { // scene is apart of the nodebank but we do not want to attach nodes for scene. 
                    a_root->AttachChild(nodePtr.get());
                //    logger::info("attached light to keyword mesh {}", nodeName);
                }
                else {
                    logger::warn("Light node we tried to attach was null OR this is a npcs face '{}', skipping attachment", match);
                }
            }

            dummyHandler(a_root.get(), nodeName);
        }
        
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


