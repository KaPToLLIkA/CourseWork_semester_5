#pragma once
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include "backed_up_set.h"




double getCPUTime();

#ifdef _DEBUG

void complex_time_test();

void print_test();

void remove_test();

void rand_insert_test();

#endif // _DEBUG