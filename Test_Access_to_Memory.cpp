// Test_Access_to_Memory.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#include <iostream>
#include "windows.h"
#include "tlhelp32.h"
#include "FindOffsets.cpp"
#include <psapi.h>

class Process {
private:
    DWORD ProcessID;
    HANDLE handle;
public:
    int Search_Process(LPCSTR WindowName = "UnrealWindow")
    {
      HWND hwnd = FindWindowA(WindowName, NULL);
      if (hwnd == NULL)
          return -1;
      else
         {
           ProcessID = GetWindowThreadProcessId(hwnd, &ProcessID);
           handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);

           if (ProcessID == NULL) return -1;
           else return 0;
         }
     }
    DWORD get_ProcessID() { return this->ProcessID; }
    HANDLE get_handle() { return this->handle; }
};

Process process;
std::string version = "0.1b1";

int cheating_BP(int quantity_BPs, uint32_t UWorld_Offset = 0x3A3A4B0);
int NoExistFile();
int validOffsets(uint32_t* Array_offsets);
int ExistFile(OffsetsFinder* FindOffset);

int main(int argc, const char* argv[])
{
    setlocale(0, "");
    OffsetsFinder* FindOffset = new OffsetsFinder;

    if (!FindOffset->LoadFile(argv[1])) {
        if (NoExistFile() == 0) {
            std::cout << "\nOK";
            Beep(523, 100);
        }
        else std::cout << "invalid Offset\n";
    }
    else {
        if (ExistFile(FindOffset) == 0) {
            std::cout << "\nOK";
            Beep(523, 100);
        }
        else std::cout << "invalid Offset\n";
    }
    Sleep(3000);
    return 0;
}

int NoExistFile()
{
    std::cout << "Pls run the game.\nWait...\n";

    while (process.Search_Process() != 0)//while window not found
    {
        Sleep(5000);
    }
    std::cout << "Game is found..\n";
    std::cout << "Input file not found. Offset cannot be found\nUsing standart offsets\n";
    std::cout << "Bloodpoints modifier v" << version << "\nInput BPs quantity: "; int quantity_BPs; std::cin >> quantity_BPs;
    if (cheating_BP(quantity_BPs) == 0)
        return 0;
    else return -1;
}

int ExistFile(OffsetsFinder* FindOffset)
{
    std::cout << "Input file is found.\nSearching for Offsets..\n";
    uint32_t Array_offsets[4] = { FindOffset->GetGWorld(), FindOffset->GetGObjects(), FindOffset->GetGNames(), FindOffset->GetNamePool() };
    int ValidateOffsets = validOffsets(Array_offsets);
    delete FindOffset;
    if (ValidateOffsets == 0) return 0;
    else return -1;
}

int validOffsets(uint32_t* Array_offsets)
{
    std::cout << "Offset adress List:\n";
    printf("%-17s|\t%.7X\n", "UWorld", Array_offsets[0]);
    printf("%-17s|\t%.7X\n", "TUObjectArray", Array_offsets[1]);
    printf("%-17s|\t%.7X\n", "TNameEntryArray", Array_offsets[2]);
    printf("%-17s|\t%.7X\n", "FNamePool", Array_offsets[3]);

    std::cout << "Pls run the game.\nWait...\n";
    while (process.Search_Process() != 0)//пока окно не найдено
    {
        Sleep(5000);
    }
    std::cout << "Game is found..\n";

    std::cout << "Bloodpoints modifier v" << version << "\nInput BPs quantity: "; int quantity_BPs; std::cin >> quantity_BPs;
    if (cheating_BP(quantity_BPs, Array_offsets[0]) == 0)
        return 0;
    else return -1;
}
  


int cheating_BP(int quantity_BPs, uint32_t UWorld_offset)
{ 
        float Read = 0; // Read value to memory
        uint32_t offsetBP = 0x6EF8DA08;//offset
        uint32_t offsetAdress = UWorld_offset + offsetBP;//Base+offset
        ReadProcessMemory(process.get_handle(), (PBYTE*)offsetAdress, &Read, sizeof(Read), 0); //Check valid read
        if (Read == 0)//if Read=0 then invalid offset
            return -1;
        else {//ok
            WriteProcessMemory(process.get_handle(), (LPVOID)offsetAdress, &quantity_BPs, sizeof(quantity_BPs), 0);//write value Read to memory
            return 0;
        }
}