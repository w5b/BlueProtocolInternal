#include <pch.h>
#include "menu.h"
#include "ZeroGUI.h"
#include "config.h"
#include <src/cheat.h>

namespace Menu
{
	void Tick()
	{
		ZeroGUI::Input::Handle();

		static bool menu_opened = true;
		static FVector2D pos = { 1280.0f, 1280.f };
		if (GetAsyncKeyState(VK_F2) & 1) menu_opened = !menu_opened;

		if (ZeroGUI::Window("BlueProtocol | UnknownCheats.me", &pos, FVector2D{ 500.0f, 400.0f }, menu_opened))
		{
			static int tab = 0;
			if (ZeroGUI::ButtonTab("Visuals", FVector2D{ 110, 25 }, tab == 0)) tab = 0;
			if (ZeroGUI::ButtonTab("Player", FVector2D{ 110, 25 }, tab == 1)) tab = 1;
			if (ZeroGUI::ButtonTab("Miscellaneous", FVector2D{ 110, 25 }, tab == 2)) tab = 2; 
			if (ZeroGUI::ButtonTab("Colors", FVector2D{ 110, 25 }, tab == 3)) tab = 3;
			if (ZeroGUI::ButtonTab("HotKeys", FVector2D{ 110, 25 }, tab == 4)) tab = 4;
			ZeroGUI::NextColumn(130.0f);
			//static FLinearColor test_color{ 0.0f, 0.0f, 1.0f, 1.0f };
			//ZeroGUI::ColorPicker("Color Picker", &test_color);
			switch (tab)
			{
			case 0:
				ZeroGUI::Checkbox("Player ESP", &config::PlayerEsp);
				ZeroGUI::Checkbox("Monster ESP", &config::MonsterEsp);
			break;
			case 1:
				ZeroGUI::Checkbox("GodMode", &config::GodMode);
				ZeroGUI::Checkbox("NoClip", &config::NoClip);
				ZeroGUI::SliderInt("SpeedHack", &config::speed, 1, 50.f);
				ZeroGUI::Checkbox("No Skill CD", &config::skillCD);
				ZeroGUI::Checkbox("MobVaccum", &config::MobVaccum);
				if (config::MobVaccum)
				{
					ZeroGUI::SliderFloat("X", &config::VaccumDistance.X, 100.f, 2000.f);
					ZeroGUI::SliderFloat("Y", &config::VaccumDistance.Y, 100.f, 2000.f);
					ZeroGUI::SliderFloat("Z", &config::VaccumDistance.Z, 0.f, 2000.f);
				}
				break;
			case 2:
				if (ZeroGUI::Button("1M+ Money", { 110, 25 }))
				{
					cheat::UnlimMoney(1000000);
				}
				ZeroGUI::Checkbox("Log Quests", &config::logQuests);
				if (config::logQuests)
				{
				if (ZeroGUI::Button("Complete quests in queue", { 110, 25 }))
				{
					for (int i = 0; i < globals::questToComplete.size(); i++)
					{
						for (int j = 0; j < 5; j++)
						{
							globals::MyController->QuestComponent->Server_CompleteStep(globals::questToComplete[i], j, -1);
							globals::MyController->QuestComponent->Request_CompleteStep(globals::questToComplete[i], j, -1);
						}
						globals::MyController->QuestComponent->Server_CompleteQuest(globals::questToComplete[i], 0, true);
					}
				}
				}
				break;
			case 3:
				ZeroGUI::ColorPicker("Player Box Color", &config::playerEspBoxColor);
				ZeroGUI::ColorPicker("Player Name Color", &config::playerEspNameColor);
				ZeroGUI::ColorPicker("Monster Box Color", &config::MonsterEspBoxColor);
				ZeroGUI::ColorPicker("Monster Name Color", &config::MonsterEspNameColor);
				break;
			case 4:
				ZeroGUI::Hotkey("NoClip", { 110, 25 }, &config::noClipKey);
				break;
			}

		}
		if (GetAsyncKeyState(config::noClipKey) & 1) config::NoClip = !config::NoClip;
		ZeroGUI::Render();;
		ZeroGUI::Draw_Cursor(menu_opened);
	}
	void SetupCanvas(UCanvas* pCanvas)
	{
		ZeroGUI::SetupCanvas(pCanvas);
	}
};