#pragma once
#include <Windows.h>
class PData
{
public:
	float X;
	float Y;
	float Z;
	DWORD address;
	const DWORD EntityList = 0x0050F4F8;
	const DWORD posXoffset = 0x4;
	const DWORD posYoffset = 0x8;
};
class Localplayer : public PData
{
public:
	DWORD localPlayerAddress;
	const DWORD playerOffset = 0x10F4F4;
};