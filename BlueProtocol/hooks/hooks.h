#pragma once

using namespace CG;

namespace hooks
{
	typedef void(__thiscall* tPostRender)(UGameViewportClient*, UCanvas*);
	inline tPostRender oPostRender;

	typedef void(__thiscall* tProcessEvent)(UObject* self, UFunction* function, void* params);
	inline tProcessEvent oProcessEvent;

	bool StartHooks(HMODULE hModule);
	void Unload(UGameViewportClient* viewport, HMODULE& hModule, FILE* f = nullptr);

	void HookPostRender(UGameViewportClient* viewport);
	void UnhookVFTable(void* addr, void** ret, int index);

	void HookProcessEvent(UGameViewportClient* viewport);
}