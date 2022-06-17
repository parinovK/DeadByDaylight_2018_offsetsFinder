// Test_Access_to_Memory.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#include <iostream>
#include "windows.h"
#include "tlhelp32.h"
#include "FindOffsets.cpp"
#include <psapi.h>

DWORD ProcessID;
HANDLE handle;
std::string version = "0.1b1";

int Search_Process();
int cheating_BP(int quantity_BPs, uint32_t UWorld_Offset = 0x3A3A4B0);
void NoExistFile();
void invalidOffsets();
void validOffsets(uint32_t* Array_offsets);
void ExistFile(OffsetsFinder* FindOffset);
void OK_operation();

int main(int argc, const char* argv[])
{
    setlocale(0, "");
    int sel;

    OffsetsFinder* FindOffset = new OffsetsFinder;
    if (!FindOffset->LoadFile(argv[1]))
        sel = 0;
    else sel = 1;

    switch (sel){
        case 0:
            NoExistFile();
            break;
        case 1:
            ExistFile(FindOffset);
            break;
    }
    return 0;
}

void NoExistFile()
{
    std::cout << "Pls run the game.\nWait...\n";
    while (Search_Process() != 0)//while window not found
    {
        Sleep(5000);
    }
    std::cout << "Game is found..\n";
    std::cout << "Input file not found. Offset cannot be found\nUsing standart offsets\n";
    std::cout << "Bloodpoints modifier v" << version << "\nInput BPs quantity: "; int quantity_BPs; std::cin >> quantity_BPs;
    if (cheating_BP(quantity_BPs) == 0)
    {
        OK_operation();
    }
    else std::cout << "invalid Offset\n";
    Sleep(4000);
}

void ExistFile(OffsetsFinder* FindOffset)
{
    std::cout << "Input file is found.\nSearching for Offsets..\n";
    const int n = 4;
    int k = 0;
    uint32_t Array_offsets[n] = { FindOffset->GetGWorld(), FindOffset->GetGObjects(), FindOffset->GetGNames(), FindOffset->GetNamePool() };
    for (int i = 0; i < n; i++)
        if (!Array_offsets[i]) k++;
    if (k == n)//если все оффсеты = 0, значит устарели адреса, либо не тот экзешник. Будем использовать встроенные оффсеты
        invalidOffsets();
    else {
        validOffsets(Array_offsets);
        delete FindOffset;
    }
}

void invalidOffsets()
{
    std::cout << "Pls run the game.\nWait...\n";
    while (Search_Process() != 0)
    {
        Sleep(5000);
    }
    std::cout << "Game is found..\n";
    std::cout << "Offsets cannot be found.\nCheck the name of the input file \"Dead-by-Daylight-Win64-Shipping.exe\"\nUsing standart offsets\n";
    std::cout << "Bloodpoints modifier v" << version << "\nInput BPs quantity: "; int quantity_BPs; std::cin >> quantity_BPs;
    if (cheating_BP(quantity_BPs) == 0)
    {
        OK_operation();
    }
    else std::cout << "invalid Offset\n";
    Sleep(5000);
}

void validOffsets(uint32_t* Array_offsets)
{
    std::cout << "Offset adress List:\n";
    printf("%-17s|\t%.7X\n", "UWorld", Array_offsets[0]);
    printf("%-17s|\t%.7X\n", "TUObjectArray", Array_offsets[1]);
    printf("%-17s|\t%.7X\n", "TNameEntryArray", Array_offsets[2]);
    printf("%-17s|\t%.7X\n", "FNamePool", Array_offsets[3]);

    std::cout << "Pls run the game.\nWait...\n";
    while (Search_Process() != 0)//пока окно не найдено
    {
        Sleep(5000);
    }
    std::cout << "Game is found..\n";

    std::cout << "Bloodpoints modifier v" << version << "\nInput BPs quantity: "; int quantity_BPs; std::cin >> quantity_BPs;
    if (cheating_BP(quantity_BPs, Array_offsets[0]) == 0)
    {
        OK_operation();
    }
    else std::cout << "invalid Offset\n";
    Sleep(5000);
}

void OK_operation()
{
    std::cout << "\nOK";
    Beep(523, 100);
    Sleep(3000);
}
  
int Search_Process()
{
    HWND hwnd = FindWindowA("UnrealWindow", NULL);

    if (hwnd == NULL)
        return -1;
    else
    {
        GetWindowThreadProcessId(hwnd, &ProcessID);
        handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);

        if (ProcessID == NULL) return -1;
        else return 0;
    }
}

int cheating_BP(int quantity_BPs, uint32_t UWorld_offset)
{
        float Read = 0; // Read value to memory
        uint32_t offsetBP = 0x6EF8DA08;//offset
        uint32_t offsetAdress = UWorld_offset + offsetBP;//Base+offset
        ReadProcessMemory(handle, (PBYTE*)offsetAdress, &Read, sizeof(Read), 0); //Check valid read
        if (Read == 0)//if Read=0 then invalid offset
            return -1;
        else {//ok
            WriteProcessMemory(handle, (LPVOID)offsetAdress, &quantity_BPs, sizeof(quantity_BPs), 0);//write value Read to memory
            return 0;
        }
}