#include <iostream>
#include <windows.h>
#include "MemoryControl.h"
#include "PData.h"
#include "Vector3.h"


const DWORD viewMatrixOffset = 0x501AE8;
float w;


void Draw(float x, float y, HDC hDC, HBRUSH brush)
{
	int width = 10;
	int height = 10;
	RECT rectangle = { (LONG) x, (LONG) y, (LONG) x + width, (LONG) y + height };
	FillRect(hDC, &rectangle, brush); 
}
bool TransformCoordinates(Vector3 INPUT, PData& OUTPUT, float* viewMatrix)
{
	OUTPUT.X = INPUT.x * viewMatrix[0] + INPUT.y * viewMatrix[4] + INPUT.z * viewMatrix[8]  + viewMatrix[12];
	OUTPUT.Y = INPUT.x * viewMatrix[1] + INPUT.y * viewMatrix[5] + INPUT.z * viewMatrix[9]  + viewMatrix[13];
	OUTPUT.Z = INPUT.x * viewMatrix[2] + INPUT.y * viewMatrix[6] + INPUT.z * viewMatrix[10] + viewMatrix[14];
	w	     = INPUT.x * viewMatrix[3] + INPUT.y * viewMatrix[7] + INPUT.z * viewMatrix[11] + viewMatrix[15];

	if (w < 0.0)
	{
		return false;
	}
	else 
	{
		OUTPUT.X /= w;
		OUTPUT.Y /= w;
		OUTPUT.Z /= w;

		OUTPUT.X *= 800 / 2.0f;
		OUTPUT.X += 800 / 2.0f;

		OUTPUT.Y *= -600 / 2.0f;
		OUTPUT.Y += 600 / 2.0f;
		return true;
	}
}




int main()
{
	
	Vector3 INPUT;
	float viewMatrix[16];
	float* viewM = viewMatrix;
	bool check;
	MemoryControl* memControl = new MemoryControl(L"ac_client.exe");
	Localplayer localplayer;
	memControl->Attach();
	memControl->GetModuleBaseAddress();
	HDC hDC = GetDC(memControl->hWnd);
	std::cout << std::hex << memControl->moduleBaseAdress << std::endl;

	HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0));
	localplayer.localPlayerAddress = memControl->ReadFromAddress<DWORD>(LPCVOID(localplayer.playerOffset + memControl->moduleBaseAdress));

	PData Enemy;
	PData& En = Enemy;

	DWORD tmp = memControl->ReadFromAddress<DWORD>(LPCVOID(Enemy.EntityList));
	std::cout << std::hex << tmp << std::endl;
	Enemy.address = memControl->ReadFromAddress<DWORD>(LPCVOID(tmp + 0x4));
	std::cout << std::hex << Enemy.address << std::endl;

	
		
	while (1)
	{
		Sleep(15);
		ReadProcessMemory(memControl->hProcess, LPCVOID(viewMatrixOffset), &viewMatrix, sizeof(viewMatrix), NULL);
		for (int i = 0; i < 16; i++)
		{
			std::cout << viewMatrix[i] << std::endl;
		}
		std::cout << "==================" << std::endl;
		INPUT.x = memControl->ReadFromAddress<float>(LPCVOID(Enemy.address + 0x4));
		INPUT.y = memControl->ReadFromAddress<float>(LPCVOID(Enemy.address + 0x8));
		INPUT.z = memControl->ReadFromAddress<float>(LPCVOID(Enemy.address + 0xC));
		std::cout << "Input X: " << INPUT.x << std::endl;
		std::cout << "Input Y: " << INPUT.y << std::endl;
		bool check = TransformCoordinates(INPUT, En, viewMatrix);
		std::cout << "Transformed X: " << Enemy.X << std::endl;
		std::cout << "Transformed Y: " << Enemy.Y << std::endl;
		std::cout << "ViewMatrix current parametr: " << w << std::endl;
		std::cout << "==================" << std::endl;
		Draw(Enemy.X, Enemy.Y, hDC, brush);
	}

	return 0;
}