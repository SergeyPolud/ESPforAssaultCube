#pragma once
#include <iostream>
#include <windows.h>
#include <TlHelp32.h>


class MemoryControl
{
public:
	MemoryControl(const wchar_t* moduleName)
	{
		this->moduleName = moduleName;
	}
	
	MODULEENTRY32* moduleEntry32= new MODULEENTRY32();
	DWORD moduleBaseAdress = 0;
	const wchar_t* moduleName;
	DWORD processID;
	HANDLE hSnapshot = 0;
	HANDLE hProcess = 0;
	HWND hWnd;
	void Attach()
	{
		try 
		{
			this->hWnd = FindWindow(0, L"AssaultCube");
			GetWindowThreadProcessId(hWnd, &processID);
			this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DWORD(processID));
			if (hWnd == NULL || processID == NULL || hProcess == INVALID_HANDLE_VALUE)
			{
				throw 1;
			}
			std::cout << "Attaching Success" << std::endl;
		}
		catch (int ex)
		{
			std::cerr << "Attaching Failed: " << GetLastError() << std::endl;
		}
	}
	void GetModuleBaseAddress()
	{
		moduleEntry32->dwSize = sizeof(MODULEENTRY32);
		this->hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
		if (hSnapshot == INVALID_HANDLE_VALUE)
		{
			std::cerr << "Getting Base Adress Failed: " << GetLastError() << std::endl;
		}
		if (Module32First(hSnapshot, moduleEntry32))
		{
			do
			{
				if (!_wcsicmp((const wchar_t*)moduleEntry32->szModule, moduleName))
				{
					moduleBaseAdress = reinterpret_cast<DWORD>(moduleEntry32->modBaseAddr);
				}
			} while (Module32Next(hSnapshot, moduleEntry32));
		}
	}
	template<typename T>
	T ReadFromAddress(LPCVOID address)
	{
		T value;
		ReadProcessMemory(this->hProcess, address, &value, sizeof(T), NULL);
		return value;
	};
	template<typename F>
	F ReadFromAddress(LPCVOID address, SIZE_T size)
	{
		F value;
		ReadProcessMemory(this->hProcess, address, &value, size, NULL);
		return value;
	}
	template<typename T>
	T WriteToAddress(LPVOID address, T value)
	{
		T val = value;
		WriteProcessMemory(this->hProcess, address, &val, sizeof(T), NULL);
	}


	~MemoryControl()
	{
		CloseHandle(this->hProcess);
		delete moduleEntry32;
	}
};