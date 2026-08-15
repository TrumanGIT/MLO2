
#include "hooks.h"
#include "Functions.h"

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{

    if (message->type == SKSE::MessagingInterface::kDataLoaded) {

        logger::info("kdata is loaded attempting to initialise and install");
        // Initialize();
        IniParser();

        // We need to create a fresh clone for every attachment. 
        // Scene nodes are mutable and have a single parent,
        // so reusing a cached node can reparent it or retain state from a previous owner.
        // Thus, assignClonedNodesToBank() is removed.

        dataHasLoaded = true;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    {
        REL::Module::reset();

        auto g_messaging = reinterpret_cast<SKSE::MessagingInterface*>(skse->QueryInterface(SKSE::LoadInterface::kMessaging));

        if (!g_messaging) {
            logger::critical("Failed to load messaging interface! This error is fatal, plugin will not load.");
            return false;
        }


        SKSE::Init(skse);

        ReadMasterListAndFillMaps();
        ReadMasterListAndFillExcludes();
        SKSE::AllocTrampoline(1 << 10);
        g_messaging->RegisterListener("SKSE", SKSEMessageHandler);
        Hooks::Install();

        // logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());
        return true;
    }
}
