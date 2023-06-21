#include <pch.h>
#include "esp.h"
#include "drawing.hpp"
#include "../menu/config.h"
#include "cheat.h"

#define World (*UWorld::GWorld)


inline wchar_t* s2wc(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	size_t convertedChars = 0;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs_s(&convertedChars, wc, cSize, c, _TRUNCATE);

	return wc;
}

namespace esp
{
	void draw(UCanvas* pCanvas)
	{
		if (!globals::OwningGameInstance) return;

		if (!globals::LocalPlayers[0]) return;

		if (globals::MyController != globals::LocalPlayers[0]->PlayerController) return;

		if (globals::MyCharacter != globals::MyController->Pawn) return;

		if (!globals::MyCharacter) return;

		if (globals::MyCharacter->CurrentHitPoint <= 0) return;

		if (!globals::defaultFont) return;

		globals::actorsMutex.lock_shared();
		for (const auto& actor : globals::actors)
		{
			if (config::PlayerEsp && actor->IsA(ASBPlayerCharacter::StaticClass()))
			{
				ASBPlayerCharacter* player = reinterpret_cast<ASBPlayerCharacter*>(actor);
				EspInfo espInfo;
				if (!getEspInfo(player, &espInfo)) continue;
				int distanceToActor = (int)globals::MyCharacter->GetDistanceTo(actor) / 100;
				drawing::DrawBox(pCanvas, FVector2D(espInfo.x, espInfo.y), espInfo.width, espInfo.height, config::playerEspBoxColor, 1.f);
				drawing::DrawDistance(pCanvas, { espInfo.feetPos.X, espInfo.feetPos.Y + 10 }, distanceToActor, config::playerEspNameColor);
				drawing::DrawPlayerName(pCanvas, { espInfo.feetPos.X, espInfo.feetPos.Y + 30 }, player, config::playerEspNameColor);
				if (player->GetGender() == ESBCharacterGender::Male)
					drawing::DrawBones(pCanvas, Maleplayer_bones, player);
				else drawing::DrawBones(pCanvas, Femaleplayer_bones, player);
				drawing::DrawHealthBar(pCanvas, player->CurrentHitPoint, player->GetMaxHitPointBase(), FVector2D(espInfo.x - 10.f, espInfo.y), FVector2D(espInfo.width, espInfo.height), 5.f);
			}
			else if (actor->IsA(ASBEnemyCharacter::StaticClass()))
			{
				ASBEnemyCharacter* mob = reinterpret_cast<ASBEnemyCharacter*>(actor);
				if (config::MonsterEsp)
				{
				EspInfo espInfo;
				if (!getEspInfo(mob, &espInfo)) continue;
				drawing::DrawHealthBar(pCanvas, mob->CurrentHitPoint, mob->GetMaxHitPointBase(), FVector2D(espInfo.x - 10.f, espInfo.y), FVector2D(espInfo.width, espInfo.height), 5.f);
				int distanceToActor = (int)globals::MyCharacter->GetDistanceTo(actor) / 100;
				drawing::DrawDistance(pCanvas, { espInfo.feetPos.X, espInfo.feetPos.Y + 10 }, distanceToActor, config::MonsterEspNameColor);
				drawing::DrawMonsterName(pCanvas, { espInfo.feetPos.X, espInfo.feetPos.Y + 30 }, mob, config::MonsterEspNameColor);
				}
				if (config::MobVaccum) cheat::MobVaccum(mob, config::VaccumDistance);
			}
		}
		globals::actorsMutex.unlock_shared();;

	}

	bool getEspInfo(AASCharacterBase* CurrentActor, EspInfo* outEspInfo)
	{
		FVector FeetLocation = CurrentActor->Mesh->GetSocketLocation(CurrentActor->Mesh->GetBoneName(0));
		FVector HeadLocation = CurrentActor->Mesh->GetSocketLocation(CurrentActor->Mesh->GetBoneName(GLOBAL_HEAD));
		FVector Position = CurrentActor->RootComponent->RelativeLocation;
		if (!FeetLocation.X || !HeadLocation.X || !Position.X) return false;
		FVector2D FeetScreenPosition, HeadScreenPosition, PositionScreenPosition;
		if (!globals::MyController->ProjectWorldLocationToScreen(FeetLocation, &FeetScreenPosition, true) ||
			!globals::MyController->ProjectWorldLocationToScreen(HeadLocation, &HeadScreenPosition, true) ||
			!globals::MyController->ProjectWorldLocationToScreen(Position, &PositionScreenPosition, true))
		{
			return false;
		}
		int height = std::abs(FeetScreenPosition.Y - HeadScreenPosition.Y);
		int width = height / 2.6;
		int x = FeetScreenPosition.X - (width / 2.f);
		int y = HeadScreenPosition.Y;

		outEspInfo->feetPos = FeetScreenPosition;
		outEspInfo->HeadPos = HeadScreenPosition;
		outEspInfo->height = height;
		outEspInfo->width = width;
		outEspInfo->x = x;
		outEspInfo->y = y;
		return true;
	}
};
