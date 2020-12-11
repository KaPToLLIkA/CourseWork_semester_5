#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include <iostream>
#include <vector>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

void PrintMemoryInfo(int k, const char* suffix)
{
    DWORD processID = GetCurrentProcessId();
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;
    APP_MEMORY_INFORMATION ami;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID);
    if (NULL == hProcess)
        return;



    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {

        std::cout << "Page fault count: \t\t" << pmc.PageFaultCount << " \n";
        std::cout << "Working set size: \t\t" << pmc.WorkingSetSize / k << suffix <<"\n";
    }


    if (GetProcessInformation(
        hProcess,
        PROCESS_INFORMATION_CLASS::ProcessAppMemoryInfo,
        &ami,
        sizeof(ami)
    )) {
        std::cout << "Total commit usage: \t\t" << ami.TotalCommitUsage / k << suffix << "\n";
    }

    CloseHandle(hProcess);
}

int main(void)
{
    //int k = 100000;
    //std::vector<int> v;
    //
    //for (int i = 0; i < 20; ++i) {
    //    v.resize(k * i);
    //    //Sleep(3000);
    //    PrintMemoryInfo(1024 * 1024);
    //}
    //v.clear();
    //v.resize(0);
    //PrintMemoryInfo(1024 * 1024);

    system("pause");
    return 0;
}