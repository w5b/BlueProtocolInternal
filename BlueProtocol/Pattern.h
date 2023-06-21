#pragma once
#include <cstdint>
#include <Windows.h>
#include <Psapi.h>
#include <tchar.h>
#include <process.h>
//https://github.com/RussellJerome/UnrealModLoader/blob/main/UnrealEngineModLoader/UnrealEngineModLoader/Utilities/Pattern.h

#define INRANGE(x,a,b)      (x >= a && x <= b)
#define getBits(x)          (INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte(x)          (getBits(x[0]) << 4 | getBits(x[1]))

static PBYTE BaseAddress = nullptr;
static MODULEINFO ModuleInfo;

inline  PBYTE FindPattern(const char* pattern, PBYTE rangeStart = nullptr, PBYTE rangeEnd = nullptr)
{
    if (BaseAddress == nullptr)
    {
        BaseAddress = reinterpret_cast<PBYTE>(GetModuleHandle(nullptr));
        GetModuleInformation(GetCurrentProcess(), reinterpret_cast<HMODULE>(BaseAddress), &ModuleInfo,
            sizeof(ModuleInfo));
    }

    if (rangeStart == nullptr) rangeStart = BaseAddress;
    if (rangeEnd == nullptr) rangeEnd = rangeStart + ModuleInfo.SizeOfImage;

    const auto* pat = reinterpret_cast<const unsigned char*>(pattern);
    PBYTE first_match = 0;
    for (PBYTE p_cur = rangeStart; p_cur < rangeEnd; ++p_cur)
    {
        if (*const_cast<PBYTE>(pat) == (BYTE)'\?' || *p_cur == getByte(pat))
        {
            if (!first_match) first_match = p_cur;
            pat += (*PWORD(pat) == static_cast<WORD>('\?\?') || *const_cast<PBYTE>(pat) != static_cast<BYTE>('\?'))
                ? 2
                : 1;
            if (!*pat) return first_match;
            pat++;
            if (!*pat) return first_match;
        }
        else if (first_match)
        {
            p_cur = first_match;
            pat = reinterpret_cast<const unsigned char*>(pattern);
            first_match = 0;
        }
    }
    return nullptr;
}
inline uint8_t __declspec(noinline)* GetAddressPTR(uint8_t* ptr, uint8_t offset, uint8_t instr_size)
{
    return (ptr + *(int32_t*)(ptr + offset) + instr_size);
};
inline bool DataCompare(PBYTE pData, PBYTE bSig, char* szMask)
{
    for (; *szMask; ++szMask, ++pData, ++bSig)
    {
        if (*szMask == 'x' && *pData != *bSig)
            return false;
    }
    return (*szMask) == 0;
}
inline PBYTE FindPattern2(PBYTE dwAddress, DWORD dwSize, PBYTE pbSig, char* szMask, long offset)
{
    size_t length = strlen(szMask);
    for (size_t i = NULL; i < dwSize - length; i++)
    {
        if (DataCompare(dwAddress + i, pbSig, szMask))
            return dwAddress + i + offset;
    }
    return nullptr;
}
