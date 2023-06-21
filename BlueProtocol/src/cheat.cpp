#include <pch.h>
#include "cheat.h"
#include "../menu/config.h"

#define World (*UWorld::GWorld)

namespace cheat
{
	void cheat::getActors(std::vector<AActor*>& actors)
	{
		if (!World)
			return;

		if (!globals::OwningGameInstance && World->OwningGameInstance)
			globals::OwningGameInstance = World->OwningGameInstance;

		if (!globals::OwningGameInstance)
			return;

		if (globals::LocalPlayers.Count() == 0 && globals::OwningGameInstance->LocalPlayers.Count() != 0)
			globals::LocalPlayers = globals::OwningGameInstance->LocalPlayers;

		if (globals::LocalPlayers.Count() == 0)
			return;

		if (globals::MyController != globals::LocalPlayers[0]->PlayerController && globals::LocalPlayers[0]->PlayerController)
			globals::MyController = reinterpret_cast<ASBPlayerController*>(globals::LocalPlayers[0]->PlayerController);

		if (globals::MyController != globals::LocalPlayers[0]->PlayerController || !globals::MyController)
			return;

		if (globals::MyCharacter != globals::MyController->Pawn)
			globals::MyCharacter = reinterpret_cast<ASBPlayerCharacter*>(globals::MyController->Pawn);

		if (!globals::MyCharacter)
			return;

		if (globals::MyCharacter->CurrentHitPoint <= 0) return;

		if (!globals::defaultFont)
			globals::defaultFont = UObject::FindObject<UFont>("Font Roboto.Roboto");


		if (!globals::defaultFont)
			return;

		std::vector<AActor*> tempActors;

		for (int i = 0; i < World->levels.Count(); i++)
		{
			ULevel* currentLevel = World->levels.IsValidIndex(i) ? World->levels[i] : nullptr;
			if (!currentLevel) continue;
			for (int j = 0; j < currentLevel->Actors.Count(); j++)
			{
				AActor* CurrentActor = currentLevel->Actors.IsValidIndex(j) ? currentLevel->Actors[j] : nullptr;
				if (!CurrentActor) continue;
				if (CurrentActor->IsA(ASBPlayerCharacter::StaticClass()))
				{
					ASBPlayerCharacter* CurrentPlayer = reinterpret_cast<ASBPlayerCharacter*>(CurrentActor);
					if (CurrentPlayer->SBPlayerController == globals::MyController)
						continue;
					tempActors.emplace_back(CurrentPlayer);
				}
				else if (CurrentActor->IsA(ASBEnemyCharacter::StaticClass()))
				{
					ASBEnemyCharacter* CurrentMob = reinterpret_cast<ASBEnemyCharacter*>(CurrentActor);
					tempActors.emplace_back(CurrentMob);
				}
			}
		}
		globals::actorsMutex.lock();
		globals::actors = tempActors;
		globals::actorsMutex.unlock();
	}

	void UnlimMoney(int moneyAmount)
	{
		globals::MyController->PlayerCharacterCommonComponent->Request_SaveAddMoney(moneyAmount);
		globals::MyController->PlayerCharacterCommonComponent->Server_SaveAddMoney(moneyAmount);
		//globals::MyController->QuestComponent->Server_AcceptQuest(20101001); 10101110
			//globals::MyController->QuestComponent->Request_AcceptQuest(20101001); 20101002  10103111
		/*for (int i = 20101002; i < 20101002 + 5000; i += 10)
		{
			globals::MyController->QuestComponent->Server_AcceptQuest(i);
			globals::MyController->QuestComponent->Request_AcceptQuest(i);
			globals::MyController->QuestComponent->Server_CompleteQuest(i, 0, true);
			globals::MyController->QuestComponent->Request_CompleteQuest(i, 0);
		}
		for (int i = 10101110; i < 10101110 + 5000; i += 10)
		{
			globals::MyController->QuestComponent->Server_AcceptQuest(i);
			globals::MyController->QuestComponent->Request_AcceptQuest(i);
			globals::MyController->QuestComponent->Server_CompleteQuest(i, 0, true);
			globals::MyController->QuestComponent->Request_CompleteQuest(i, 0);
		}*/
		//globals::MyController->AdventureBoardComponent->Server_CompleteBoard(120001000);
		//globals::MyController->AdventureBoardComponent->Request_CompleteBoard(120001000);
		/*for (int i = 101000; i < 101000 + 150; i += 10)
		{
			globals::MyController->QuestComponent->Server_AcceptQuest(i);
			globals::MyController->QuestComponent->Request_AcceptQuest(i);
		}
		USBPlayerWarpComponen
		*/
	}

	void SpeedHack(int speed)
	{
		static int oldSpeed = speed;
		static ASBWorldSettings* worldSettings = reinterpret_cast<ASBWorldSettings*>(World->K2_GetWorldSettings());
		if (worldSettings != World->K2_GetWorldSettings()) worldSettings = reinterpret_cast<ASBWorldSettings*>(World->K2_GetWorldSettings());
		if (!worldSettings) return;
		if (oldSpeed != speed)
		{
			oldSpeed = speed;
			worldSettings->TimeDilation = speed;
		}
	}

	void CompleteQuests()
	{
		FCompleteQuestResponseData xd;
		xd.BP = 1;
		xd.BPPoint = 1;
		xd.ClassExp = 3000;
		xd.ClearCount = 1;
		xd.Engram = 1;
		xd.Money = 1;
		xd.ResponseCode = 1;
		xd.RevivalAt = 1;
		xd.RoseOrbFree = 1;
		xd.UpdatedAt = 0;
		for (int i = 0; i < 32; i++)
		{
			xd.QuestIndex = i;
			globals::MyController->QuestComponent->Server_AcceptQuest(i);
			globals::MyController->QuestComponent->Request_AcceptQuest(i);
			globals::MyController->QuestComponent->Client_CompleteQuest(i, EQuestCompleteResult::Success, xd);
			globals::MyController->QuestComponent->Server_CompleteQuest(i, 0, true);
			globals::MyController->QuestComponent->Server_OnCompleteQuest(i, xd);
		}
	}

	void GodMode()
	{
		globals::MyCharacter->CurrentHitPoint = globals::MyCharacter->GetMaxHitPointBase();
	}

	void noCD()
	{
		globals::MyCharacter->SkillActionComponent->ClientSetNextSkillRecastZero(true);
	}

	void NoClip(float flySpeed, float& staticZ)
	{
		FVector loc = globals::MyCharacter->RootComponent->RelativeLocation;
		FRotator& rot = globals::MyCameraManager->RootComponent->RelativeRotation;
		globals::MyCharacter->TeleportTo({ loc.X, loc.Y, staticZ }, rot, false, true);
		if (GetAsyncKeyState('W') & 0x8000)
		{
			loc.X += flySpeed * cos(rot.Yaw * (3.14159f / 180.0f));
			loc.Y += flySpeed * sin(rot.Yaw * (3.14159f / 180.0f));
			globals::MyCharacter->TeleportTo({ loc.X, loc.Y, staticZ }, rot, false, true);
		}
		if (GetAsyncKeyState('S') & 0x8000)
		{
			loc.X -= flySpeed * cos(rot.Yaw * (3.14159f / 180.0f));
			loc.Y -= flySpeed * sin(rot.Yaw * (3.14159f / 180.0f));
			globals::MyCharacter->TeleportTo({ loc.X, loc.Y, staticZ }, rot, false, true);
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			float angle = rot.Yaw * (3.14159f / 180.0f) - 90.0f;
			loc.X += flySpeed * cos(angle);
			loc.Y += flySpeed * sin(angle);
			globals::MyCharacter->TeleportTo({ loc.X, loc.Y, staticZ }, rot, false, true);
		}

		if (GetAsyncKeyState('D') & 0x8000)
		{
			float angle = rot.Yaw * (3.14159f / 180.0f) + 90.0f;
			loc.X += flySpeed * cos(angle);
			loc.Y += flySpeed * sin(angle);
			globals::MyCharacter->TeleportTo({ loc.X, loc.Y, staticZ }, rot, false, true);
		}
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			staticZ += flySpeed;
			globals::MyCharacter->TeleportTo({ loc.X, loc.Y, staticZ }, rot, false, true);
		}
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			staticZ -= flySpeed;
			globals::MyCharacter->TeleportTo({loc.X, loc.Y, staticZ }, rot, false, true);
		}
	}

	void MobVaccum(ASBEnemyCharacter* Mob, FVector distance)
	{
		FRotator& rot = globals::MyCharacter->RootComponent->RelativeRotation;
		FVector loc = globals::MyCharacter->RootComponent->RelativeLocation;
		loc.X += distance.X * cos(rot.Yaw * (3.14159f / 180.0f));
		loc.Y += distance.Y * sin(rot.Yaw * (3.14159f / 180.0f));
		loc.Z += distance.Z;
		Mob->K2_TeleportTo(loc, rot);
	}

	void Others()
	{
		while (!globals::exit)
		{
			if (!globals::OwningGameInstance) continue;

			if (!globals::LocalPlayers[0]) continue;

			if (globals::MyController != globals::LocalPlayers[0]->PlayerController) continue;

			if (globals::MyCharacter != globals::MyController->Pawn) continue;

			if (!globals::MyCharacter) continue;

			if (globals::MyCharacter->CurrentHitPoint <= 0) continue;

			if (!globals::defaultFont) continue;

			if (globals::MyCameraManager != globals::MyController->PlayerCameraManager) globals::MyCameraManager = globals::MyController->PlayerCameraManager;
			if (!globals::MyCameraManager) continue;

			//globals::MyController->GetQuestComponent();//->Server_CompleteQuest(

			if (config::GodMode) GodMode();
			static float staticZ = globals::MyCharacter->RootComponent->RelativeLocation.Z;
			if (config::NoClip)
			{
				NoClip(50.f, staticZ);
			}
			else staticZ = globals::MyCharacter->RootComponent->RelativeLocation.Z;

			if (config::skillCD) noCD();

			SpeedHack(config::speed);

			Sleep(20);
		}
	}
};