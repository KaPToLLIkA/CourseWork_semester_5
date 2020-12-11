#pragma once
#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>

#include "backed_up_set.h"




double getCPUTime();

double getAllocatedMemorySize(double k);

void PrintMemoryInfo(int k, const char* suffix);

void memory_usage_statistic(int delta, int tests_count, bool csv);

void complex_time_test(int delta, int tests_count, bool csv);

void recovery_time_test(int delta, int tests_count, bool csv);

#ifdef _DEBUG

void print_test();

void remove_test();

void rand_insert_test();

#endif // _DEBUG