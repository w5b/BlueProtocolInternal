#pragma once

namespace config
{
	inline bool PlayerEsp = false;
	inline bool MonsterEsp = false;
	inline bool GodMode = false;
	inline bool NoClip = false;
	inline bool MobVaccum = false;
	inline bool skillCD = false;
	inline bool logQuests = false;

	inline int speed = 1;

	inline FVector VaccumDistance(500.f, 500.f, 0.f);

	inline FLinearColor playerEspBoxColor = colors::White;
	inline FLinearColor playerEspNameColor = colors::White;

	inline FLinearColor MonsterEspBoxColor = colors::Red;
	inline FLinearColor MonsterEspNameColor = colors::Red;

	inline int noClipKey = VK_XBUTTON2;
}