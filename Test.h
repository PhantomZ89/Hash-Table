#ifndef TEST_H
#define TEST_H

#include <iostream>
#include <string>
#include <sstream>
#include "ece250.h"


template <class Class_name>
class test {
	protected:
		Class_name *object;
		std::string command;

	public:
		test( Class_name *obj = nullptr );
		int run();
		virtual void process() = 0;
};

template <class Class_name>
test<Class_name>::test( Class_name *obj ):
object( obj ) {
	//emtpy constructor
}

template <class Class_name>
int test<Class_name>::run() {
	//Read the flag which indicates the command to be test and
	//Stop if we have reached the end of the file

	ece250::allocation_table.stop_recording();

	const static std::string prompt = " % ";

	while ( true ) {
		//Terminate if there is an end-of-file or the user types 'exit'

		if ( std::cin.eof() ) {
			break;
		}

		++ece250::count;
		std::cout << ece250::count << prompt;

		std::cin >> command;

		//Remove any comments
		if ( command.substr( 0, 2 ) == "//" ) {
			char comment[1024];
			std::cin.getline( comment, 1024 );

			std::cout << command << comment << std::endl;
			continue;
		}

		//Terminate if there is an end-of-file or the user types 'exit'

		if ( std::cin.eof() ) {
			std::cout << "Exiting..." << std::endl;
			break;
		}

		//If user enters !!,
		//set the command to be the last command
		//If user enters !n where n is a number,  (1 <= n < count)
		//set the command ot be the nth command

		if ( command == "!!" ) {
			if ( ece250::count == 1 ) {
				std::cout << "Event not found" << std::endl;
				continue;
			}

			command = ece250::history[ece250::count - 1];
		} else if ( command[0] == '!' ) {
			int n;
			std::istringstream number( command.substr( 1, command.length() - 1 ) );
			number >> n;

			if ( n <= 0 || n >= ece250::count || n >= 1000 ) {
				std::cout << "Event not found" << std::endl;
				continue;
			}

			command = ece250::history[n];
		}

		//Only track the first 1001 commands
		if ( ece250::count < 1000 ) {
			ece250::history[ece250::count] = command;
		}

		//Start tracking any memory allocations made
		ece250::allocation_table.start_recording();

		//There are five key commands

		if ( command == "exit" ) {
			std::cout << "Okay" << std::endl;
			ece250::allocation_table.stop_recording();
			break;
		} else if ( command == "delete" ) {
			delete object;
			object = nullptr;
			std::cout << "Okay" << std::endl;
		} else if ( command == "summary" ) {
			ece250::allocation_table.summary();
		} else if ( command == "details" ) {
			ece250::allocation_table.details();
		} else if ( command == "memory" ) {
			int n;

			std::cin >> n;

			if ( n == ece250::allocation_table.memory_alloc() ) {
				std::cout << "Okay" << std::endl;
			} else {
				std::cout << "Failure in memory allocation: expecting "
				          << n << " bytes to be allocated, but "
				          << ece250::allocation_table.memory_alloc()
				          << " bytes were allocated" << std::endl;
			}
		} else if ( command == "memory_store" ) {
			ece250::allocation_table.memory_store();
			std::cout << "Okay" << std::endl;
		} else if ( command == "memory_change" ) {
			int n;

			std::cin >> n;

			ece250::allocation_table.memory_change( n );
		} else {
			process();
		}

		//Stop tracking any memory allocations made
		ece250::allocation_table.stop_recording();
	}

	return 0;
}

#endif
