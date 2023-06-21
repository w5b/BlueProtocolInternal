#pragma once

using namespace CG;

namespace cheat
{
	void getActors(std::vector<AActor*>& actors);
	void UnlimMoney(int moneyAmount);
	void SpeedHack(int speed);
	void CompleteQuests();
	void GodMode();
	void NoClip(float flySpeed, float& staticZ);
	void MobVaccum(ASBEnemyCharacter* Mob, FVector Distance);
	void Others();
}