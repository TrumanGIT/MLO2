
#include "hooks.h"
#include "Functions.h"

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{

    if (message->type == SKSE::MessagingInterface::kDataLoaded) {

       logger::info("kdata is loaded attempting to initialise and install");
                    Initialize();

                    assignClonedNodesToBank();
                       dataHasLoaded = true; 
    }
}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
    REL::Module::reset();

    auto g_messaging = reinterpret_cast<SKSE::MessagingInterface*>(a_skse->QueryInterface(SKSE::LoadInterface::kMessaging));

    if (!g_messaging) {
        logger::critical("Failed to load messaging interface! This error is fatal, plugin will not load.");
        return false;
    }

    SKSE::Init(a_skse);
    SKSE::AllocTrampoline(1 << 10);
    g_messaging->RegisterListener("SKSE", SKSEMessageHandler);
    Hooks::Install(); 
    IniParser();
      
   // logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());
    return true;
}
