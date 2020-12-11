#include <iostream>
#include <string>
#include <limits>
#include "backed_up_set.h"
#include "test.h"


uint32_t get_uint32t_from_stdin();

#define ENABLE_PERFORMANCE_TESTS

int main() {
#ifdef ENABLE_PERFORMANCE_TESTS
	recovery_time_test(100 * 1000, 9, true);
	//recovery_time_test(1000 * 1000, 10, true);

	memory_usage_statistic(100 * 1000, 9, true);
	//memory_usage_statistic(1000*1000, 10, false);
	
	complex_time_test(100 * 1000, 9, true);
	//complex_time_test(1000 * 1000, 10, true);
#endif // ENABLE_PERFORMANCE_TESTS

	
#ifdef _DEBUG
	print_test();
	remove_test();
	rand_insert_test();
#endif // _DEBUG

	backed_up_set<std::string> set;

	bool exit = false;
	while (!exit) {
		std::string input;
		std::cout
			<< "\n\n\n\n\n\n\n"
			<< "1 - ADD\n"
			<< "2 - REMOVE\n"
			<< "3 - REMOVE UNUSED\n"
			<< "4 - SAVE BACKUP\n"
			<< "5 - GO TO BACKUP\n"
			<< "6 - CLEAN\n"
#ifdef _DEBUG
			<< "7 - PRINT ADDED\n"
			<< "8 - PRINT REMOVED\n"
			<< "9 - PRINT UNUSED\n"
#endif // _DEBUG
			<< "0 - EXIT\n"
			<< "PRINT COMMAND: "
			;

		std::getline(std::cin, input, '\n');

		switch (input[0]) {
		case '0':
			exit = true;
			break;
		case '1': {
			std::string value;
			std::cout << "PRINT ITEM: ";
			std::getline(std::cin, value, '\n');
			set.insert(value);
			break;
		}
		case '2': {
			std::string value;
			std::cout << "PRINT ITEM: ";
			std::getline(std::cin, value, '\n');
			set.remove(value);
			break;
		}
		case '3': {
			set.remove_unused_items();
			break;
		}
		case '4': {
			set.save_backup();
			break;
		}
		case '5': {
			std::cout << "PRINT VERSION, MAX - " << set.get_current_version() << ": ";
			uint32_t version = get_uint32t_from_stdin();
			set.go_to_backup(version, true);
			break;
		}
		case '6': {
			set.clean();
			break;
		}
#ifdef _DEBUG
		case '7': {
			set.print(std::cout, states::ADD);
			break;
		}
		case '8': {
			set.print(std::cout, states::REMOVE);
			break;
		}
		case '9': {
			set.print(std::cout, states::UNUSED);
			break;
		}
#endif // _DEBUG

		default: {
			std::cout << "Input error!\n";
			break;
		}
		}
	}
}

uint32_t get_uint32t_from_stdin()
{
#undef max
	while (true) {
		uint32_t t;
		std::cin >> t;

		if (std::cin.fail()) {
			std::cout << "Input error!\n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
		else {
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			return t;
		}
	}
}
