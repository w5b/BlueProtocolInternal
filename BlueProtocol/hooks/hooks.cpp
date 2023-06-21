#include <pch.h>
#include "hooks.h"
#include "../src/cheat.h"
#include "../src/esp.h"
#include "../menu/menu.h"
#include "includes/MinHook.h"
#include <menu/config.h>

#pragma comment(lib, "includes/MinHook.lib")

#define World (*UWorld::GWorld)

//#define DEBUG

void WINAPI hPostRender(UGameViewportClient* aUTGameViewportClient, UCanvas* pCanvas)
{
    Menu::SetupCanvas(pCanvas);
    Menu::Tick();
    esp::draw(pCanvas);
    return hooks::oPostRender(aUTGameViewportClient, pCanvas);
}

void WINAPI hProcessEvent(UObject* self, UFunction* function, void* params)
{
    //std::string funcName = function->GetFullName();
    if (config::logQuests)
    {
    if (function->InternalIndex == 11902) //server_acceptquest
    {
        auto parms = reinterpret_cast<USBQuestComponent_Server_AcceptQuest_Params*>(params);
        globals::questToComplete.emplace_back(parms->QuestIndex);
#ifdef DEBUG
        printf("Quest Added to QUEUE: %i\n", parms->QuestIndex);
#endif
        if (globals::questIdsSet.find(parms->QuestIndex) == globals::questIdsSet.end())
        {
            globals::questIdsSet.insert(parms->QuestIndex);
            globals::logfile << parms->QuestIndex << "\n";
        }
    }
    }
    if (config::skillCD)
    {
    if (function->InternalIndex == 23454) //ClientSetGaugeAmountUnreliable
    {
        static auto specialSkill = globals::MyCharacter->GetSpecialSkill();
        if (specialSkill != globals::MyCharacter->GetSpecialSkill()) specialSkill = globals::MyCharacter->GetSpecialSkill();
        if (!specialSkill) return hooks::oProcessEvent(self, function, params);
        specialSkill->ServerSetGauge(1000.f);
    }
    }
  /* if (funcName.find("Skill") != std::string::npos && funcName.find("Tick") == std::string::npos && funcName.find("GetVisibility_1") == std::string::npos && funcName.find("Animation") == std::string::npos && funcName.find("ClientSetGaugeAmountUnreliable") == std::string::npos && funcName.find("OnUpdatePercent") == std::string::npos && funcName.find("PlayAnim") == std::string::npos) 
   {
       for (int i = 0; i < 10; i++)
       {
        printf("%s\n", funcName.c_str()); 
       }
   }*/
   /*if (function->GetFullName().find("Dungeon") != std::string::npos && function->GetFullName().find("Tick") == std::string::npos
       && function->GetFullName().find("UMG_DungeonFinder_Button") == std::string::npos)
   {
       
       printf("%s\n", function->GetFullName().c_str());
   }*/
      /*if (funcName.find("ShowQuestRequestMenu") != std::string::npos && funcName.find("Tick") == std::string::npos && funcName.find("Ubergraph") == std::string::npos)
       {
           auto xd = reinterpret_cast<ABP_ScriptActor_C_BP_ShowQuestRequestMenu_Params*>(params);
           printf("%i\n", xd->InQuestIndex);
       }*/
    //if (funcName.find("Cast") != std::string::npos && funcName.find("ReceiveTick") == std::string::npos)
    //{
     //   printf("%s\n", funcName.c_str());
   // }
   /* if (funcName.find("GatherPoint") != std::string::npos)
    {
        for (int i = 0; i < 10; i++)
        {
            hooks::oProcessEvent(self, function, params);
       }
    }*/
    /*if (function->GetFullName().find("Server_ChangeActiveBoard") != std::string::npos)
    {
        printf("yer");
        auto p = reinterpret_cast<USBPlayerAdventureBoardComponent_Server_ChangeActiveBoard_Params*>(params);
        globals::MyController->AdventureBoardComponent->Request_CompleteBoard(p->InBoardId);
        globals::MyController->AdventureBoardComponent->Server_CompleteBoard(p->InBoardId);
    }*/
    
     return hooks::oProcessEvent(self, function, params);
}

bool hooks::StartHooks(HMODULE hModule)
{

#ifdef DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif

    if (!InitSdk())
    {
    #ifdef DEBUG
            FreeConsole();
    #endif
        FreeLibraryAndExitThread(hModule, 0);
    }

    UGameViewportClient* viewport = (*UWorld::GWorld)->OwningGameInstance->LocalPlayers[0]->ViewportClient;
    HookPostRender(viewport);
    HookProcessEvent(viewport);


    std::thread OthersThread(cheat::Others);    

    while (!globals::exit)
    {
       if (GetAsyncKeyState(VK_END) & 1)
           globals::exit = true;
        cheat::getActors(globals::actors);
        Sleep(20);
    }
    OthersThread.join();
    hooks::Unload(viewport, hModule);
    return true;
}

void hooks::Unload(UGameViewportClient* viewport, HMODULE& hModule, FILE* f)
{
    hooks::UnhookVFTable(viewport, reinterpret_cast<void**>(&hooks::oPostRender), POST_RENDER_INDEX);
#ifdef DEBUG
    FreeConsole();
#endif
    MH_Uninitialize();
    MH_DisableHook(MH_ALL_HOOKS);
    globals::logfile.close();
    FreeLibraryAndExitThread(hModule, 0);
}

void hooks::HookPostRender(UGameViewportClient* viewport)
{
    uintptr_t* vtable = reinterpret_cast<uintptr_t*>(*(uintptr_t**)viewport);

    DWORD oldProtect;
    VirtualProtect(vtable, sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect);

    oPostRender = (tPostRender)vtable[POST_RENDER_INDEX];

    vtable[POST_RENDER_INDEX] = reinterpret_cast<uintptr_t>(&hPostRender);

    DWORD unused;
    VirtualProtect(vtable, sizeof(uintptr_t), oldProtect, &unused);
}
void hooks::UnhookVFTable(void* addr, void** ret, int index)
{
    auto vtable = *(uintptr_t**)addr;

    DWORD oldProtect;
    VirtualProtect(&vtable[index], sizeof(uintptr_t), PAGE_EXECUTE_READWRITE, &oldProtect);

    vtable[index] = reinterpret_cast<uintptr_t>(*ret);

    DWORD unused;
    VirtualProtect(&vtable[index], sizeof(uintptr_t), oldProtect, &unused);
}

void hooks::HookProcessEvent(UGameViewportClient* viewport)
{
    MH_Initialize();
    auto vtable = reinterpret_cast<tProcessEvent>(reinterpret_cast<void**>(UObject::StaticClass()->VfTable)[PROCESS_EVENTS_INDEX]);
    if (MH_CreateHook((LPVOID)vtable, &hProcessEvent, reinterpret_cast<void**>(&oProcessEvent)) != MH_OK)
    {
#ifdef DEBUG
        printf("failed to hook processevent\n");
#endif

    }
    MH_EnableHook(MH_ALL_HOOKS);
}



