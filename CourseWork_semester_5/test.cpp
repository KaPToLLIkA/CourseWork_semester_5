#include "test.h"
/*
 * Author:  David Robert Nadeau
 * Site:    http://NadeauSoftware.com/
 * License: Creative Commons Attribution 3.0 Unported License
 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
 */
#if defined(_WIN32)
#include <Windows.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

#else
#error "Unable to define getCPUTime( ) for an unknown OS."
#endif

 /**
  * Returns the amount of CPU time used by the current process,
  * in seconds, or -1.0 if an error occurred.
  */
double getCPUTime()
{
#if defined(_WIN32)
    /* Windows -------------------------------------------------- */
    FILETIME createTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if (GetProcessTimes(GetCurrentProcess(),
        &createTime, &exitTime, &kernelTime, &userTime) != -1)
    {
        SYSTEMTIME userSystemTime;
        if (FileTimeToSystemTime(&userTime, &userSystemTime) != -1)
            return (double)userSystemTime.wHour * 3600.0 +
            (double)userSystemTime.wMinute * 60.0 +
            (double)userSystemTime.wSecond +
            (double)userSystemTime.wMilliseconds / 1000.0;
    }

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
    /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */

#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    /* Prefer high-res POSIX timers, when available. */
    {
        clockid_t id;
        struct timespec ts;
#if _POSIX_CPUTIME > 0
        /* Clock ids vary by OS.  Query the id, if possible. */
        if (clock_getcpuclockid(0, &id) == -1)
#endif
#if defined(CLOCK_PROCESS_CPUTIME_ID)
            /* Use known clock id for AIX, Linux, or Solaris. */
            id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined(CLOCK_VIRTUAL)
            /* Use known clock id for BSD or HP-UX. */
            id = CLOCK_VIRTUAL;
#else
            id = (clockid_t)-1;
#endif
        if (id != (clockid_t)-1 && clock_gettime(id, &ts) != -1)
            return (double)ts.tv_sec +
            (double)ts.tv_nsec / 1000000000.0;
    }
#endif

#if defined(RUSAGE_SELF)
    {
        struct rusage rusage;
        if (getrusage(RUSAGE_SELF, &rusage) != -1)
            return (double)rusage.ru_utime.tv_sec +
            (double)rusage.ru_utime.tv_usec / 1000000.0;
    }
#endif

#if defined(_SC_CLK_TCK)
    {
        const double ticks = (double)sysconf(_SC_CLK_TCK);
        struct tms tms;
        if (times(&tms) != (clock_t)-1)
            return (double)tms.tms_utime / ticks;
    }
#endif

#if defined(CLOCKS_PER_SEC)
    {
        clock_t cl = clock();
        if (cl != (clock_t)-1)
            return (double)cl / (double)CLOCKS_PER_SEC;
    }
#endif

#endif

    return -1;      /* Failed. */
}

double getAllocatedMemorySize(double k)
{
    DWORD processID = GetCurrentProcessId();
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;
    APP_MEMORY_INFORMATION ami;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID);
    if (NULL == hProcess)
        return -1;



    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        return pmc.WorkingSetSize / k;
    }

    CloseHandle(hProcess);
    return -1;
}

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

        //std::cout << "Page fault count: \t\t" << pmc.PageFaultCount << " \n";
        std::cout << "Working set size: \t\t" << pmc.WorkingSetSize / k << suffix << "\n";
    }


    if (GetProcessInformation(
        hProcess,
        PROCESS_INFORMATION_CLASS::ProcessAppMemoryInfo,
        &ami,
        sizeof(ami)
    )) {
        //std::cout << "Total commit usage: \t\t" << ami.TotalCommitUsage / k << suffix << "\n";
    }

    CloseHandle(hProcess);
}


void memory_usage_statistic(int delta, int tests_count, bool csv)
{
    std::ofstream fmy;
    std::ofstream fstd;
    if (csv) {
        fmy.open("tests\\memory_usage_statistic_my_set.csv", std::ios_base::app);
        fstd.open("tests\\memory_usage_statistic_std_set.csv", std::ios_base::app);

        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        fmy << "MEMORY," << std::ctime(&time) << ",\n";
        fstd << "MEMORY," << std::ctime(&time) << ",\n";
    }

    if (csv) {
        fmy << "ITEMS,TIME,\n";
        fstd << "ITEMS,TIME,\n";
    }

    for (int i = 1; i <= tests_count; ++i) {
        backed_up_set<int> my_set;


       
        for (int ii = 0; ii < delta * i; ++ii) {
            my_set.insert(ii);
        }
        double memoryMB = getAllocatedMemorySize(1024 * 1024);
        std::cout << "INSERT MY SET\t\t" << delta * (i) 
            << " ITEMS, MEMORY: " << memoryMB << "\n";
        
        if (csv) fmy << delta * i << "," << memoryMB << ",\n";

    }

    for (int i = 1; i <= tests_count; ++i) {
       

        std::set<int> set;

        for (int ii = 0; ii < delta * i; ++ii) {
            set.insert(ii);
        }
        double memoryMB = getAllocatedMemorySize(1024 * 1024);
        std::cout << "INSERT STD SET\t\t" << delta * (i)
            << " ITEMS, MEMORY: " << memoryMB << "\n";
        
        if (csv) fstd << delta * i << "," << memoryMB << ",\n";
    }

    if (csv) {
        fmy.close();
        fstd.close();
    }

}


void complex_time_test(int delta, int tests_count, bool csv)
{
    std::ofstream fi;
    std::ofstream fr;
    if (csv) {
        fi.open("tests\\complex_time_test_insert.csv", std::ios_base::app);
        fr.open("tests\\complex_time_test_remove.csv", std::ios_base::app);

        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        fi << "INSERT," << std::ctime(&time) << ",\n";
        fr << "REMOVE," << std::ctime(&time) << ",\n";
    }

    if (csv) {
        fi << "MY_SET,\n";
        fi << "ITEMS,TIME,\n";
        fr << "MY_SET,\n";
        fr << "ITEMS,TIME,\n";
    }
    for (int i = 1; i <= tests_count; ++i) {
        backed_up_set<int> my_set;


        double start = getCPUTime();
        for (int ii = 0; ii < delta * i; ++ii) {
            my_set.insert(ii);
        }
        double end = getCPUTime();
        std::cout << "INSERT MY SET\t\t" << delta * (i) << " ITEMS:\t\t"
            << end - start << "s\n";
        if (csv) fi << delta * i << "," << end - start << ",\n";


        start = getCPUTime();
        for (int ii = 0; ii < delta * i; ++ii) {
            my_set.remove(ii);
        }
        end = getCPUTime();
        std::cout << "REMOVE MY SET\t\t" << delta * (i) << " ITEMS:\t\t"
            << end - start << "s\n";
        if (csv) fr << delta * i << "," << end - start << ",\n";
    }

    if (csv) {
        fi << "STD_SET,\n";
        fi << "ITEMS,TIME,\n";
        fr << "STD_SET,\n";
        fr << "ITEMS,TIME,\n";
    }
    for (int i = 1; i <= tests_count; ++i) {
        

        std::set<int> set;
        double start = getCPUTime();
        for (int ii = 0; ii < delta * i; ++ii) {
            set.insert(ii);
        }
        double end = getCPUTime();
        std::cout << "INSERT STD SET\t\t" << delta * (i) << " ITEMS:\t\t"
            << end - start << "s\n";
        if (csv) fi << delta * i << "," << end - start << ",\n";


        start = getCPUTime();
        for (int ii = 0; ii < delta * i; ++ii) {
            set.erase(ii);
        }
        end = getCPUTime();
        std::cout << "REMOVE STD SET\t\t" << delta * (i) << " ITEMS:\t\t"
            << end - start << "s\n";
        if (csv) fr << delta * i << "," << end - start << ",\n";
    }

    if (csv) {
        fi.close();
        fr.close();
    }
}

void recovery_time_test(int delta, int tests_count, bool csv)
{
    std::ofstream fn;
    std::ofstream fy;
    if (csv) {
        fn.open("tests\\recovery_time_test_without_remove.csv", std::ios_base::app);
        fy.open("tests\\recovery_time_test_with_remove.csv", std::ios_base::app);
    
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        fn << "WITHOUT_REMOVE_UNUSED," << std::ctime(&time) << ",\n";
        fy << "WITH_REMOVE_UNUSED," << std::ctime(&time) << ",\n";
    }

    if (csv) {
        fn << "ITEMS,TIME,\n";
        fy << "ITEMS,TIME,\n";
    }
    for (int i = 1; i <= tests_count; ++i) {
        {
            backed_up_set<int> my_set;

            my_set.insert(0);
            my_set.save_backup();
            for (int ii = 1; ii < delta * i; ++ii) {
                my_set.insert(ii);
            }
            double start = getCPUTime();
            my_set.go_to_backup(0);
            double end = getCPUTime();
            std::cout << "RECOVERY NO REMOVE\t\t" << delta * (i) << " ITEMS:\t\t"
                << end - start << "s\n";
            if (csv) fn << delta * i << "," << end - start << ",\n";
        }
        
        {
            backed_up_set<int> my_set;

            my_set.insert(0);
            my_set.save_backup();
            for (int ii = 1; ii < delta * i; ++ii) {
                my_set.insert(ii);
            }
            double start = getCPUTime();
            my_set.go_to_backup(0, true);
            double end = getCPUTime();
            std::cout << "RECOVERY REMOVE\t\t\t" << delta * (i) << " ITEMS:\t\t"
                << end - start << "s\n";
            if (csv) fy << delta * i << "," << end - start << ",\n";

        }
    }

    if (csv) {
        fn.close();
        fy.close();
    }
}


#ifdef _DEBUG
void print_test()
{
    backed_up_set<int> set;

    for (int i = 0; i < 20000; ++i) {
        set.insert(i);
    }

    std::ofstream f("result.text", std::ios_base::trunc);

    set.print(f, states::ADD);
}

void remove_test()
{
    backed_up_set<int> set;
    for (int i = 1; i < 21; ++i) {
        set.insert(i);
    }
    set.print(std::cout, states::ADD);
    set.remove(16);
    set.insert(16);

    set.remove(16);
    set.insert(16);
    
    set.remove(16);
    set.insert(16);

    set.remove(16);
    set.print(std::cout, states::ADD);
    set.insert(16);
    for (int i = 21; i < 25; ++i) {
        set.insert(i);
    }
    set.print(std::cout, states::ADD);
}

void rand_insert_test()
{
    std::vector<int> nums;
    backed_up_set<int> set;

    for (int i = 1; i < 37; ++i) {
        nums.push_back(i);
    }

    for (int i = 1; i < 37; ++i) {
        int idx = rand() % nums.size();
        set.insert(nums[idx]);
        nums.erase(nums.begin() + idx);
    }

    set.print(std::cout, states::ADD);
}






#endif // _DEBUG
