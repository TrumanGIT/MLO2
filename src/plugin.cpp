
#include "hooks.h"
#include "Functions.h"

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{

    if (message->type == SKSE::MessagingInterface::kDataLoaded) {

       logger::info("kdata is loaded attempting to initialise and install");
                 //   Initialize();
       IniParser();
                    assignClonedNodesToBank();
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