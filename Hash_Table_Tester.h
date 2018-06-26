#ifndef HASH_TABLE_TESTER_H
#define HASH_TABLE_TESTER_H

#ifndef nullptr
#define nullptr 0
#endif

#include "Exceptions.h"
#include "Test.h"
#include "Hash_Table.h"

#include <iostream>


template <typename Type>
class Hash_table_tester:public test< Hash_table<Type> > {
	using test< Hash_table<Type> >::object;
	using test< Hash_table<Type> >::command;

	public:
		Hash_table_tester(Hash_table<Type> *obj =
0 ):test< Hash_table<Type> >(obj){
			//empty
		}

		void process();
};

template <typename Type>
void Hash_table_tester<Type>::process() {
	if(command == "new"){
		object = new Hash_table<Type>();
		std::cout << "Okay" << std::endl;
	} else if(command == "new:"){
		int n;
		std::cin >> n;
		object = new Hash_table<Type>(n );
		std::cout << "Okay" << std::endl;
	} else if(command == "size"){
		//Check if the size equals the next integer read

		int expected_size;

		std::cin >> expected_size;

		int actual_size = object->size();

		if(actual_size == expected_size){
			std::cout << "Okay" << std::endl;
		} else {
			std::cout << ": Failed size(): expecting the value '" << expected_size << "' but got '" << actual_size << "'" << std::endl;
		}
	} else if(command == "capacity"){
		//Check if the capacity equals the next integer read

		int expected_capacity;

		std::cin >> expected_capacity;

		int actual_capacity = object->capacity();

		if(actual_capacity == expected_capacity){
			std::cout << "Okay" << std::endl;
		} else {
			std::cout << ": Failed capacity(): expecting the value '" << expected_capacity << "' but got '" << actual_capacity << "'" << std::endl;
		}
	} else if(command == "load_factor"){
		//Check if the load factor equals the next double read

		double expected_load_factor;

		std::cin >> expected_load_factor;

		double actual_load_factor = object->load_factor();

		if(actual_load_factor == expected_load_factor){
			std::cout << "Okay" << std::endl;
		} else {
			std::cout << ": Failed load_factor(): expecting the value '" << expected_load_factor << "' but got '" << actual_load_factor << "'" << std::endl;
		}
	} else if(command == "empty"){
		//Check if the empty status equals the next Boolean read

		bool expected_empty;

		std::cin >> expected_empty;

		bool actual_empty = object->empty();

		if(actual_empty == expected_empty){
			std::cout << "Okay" << std::endl;
		} else {
			std::cout << ": Failed empty(): expecting the value '" << expected_empty << "' but got '" << actual_empty << "'" << std::endl;
		}
	} else if(command == "member"){
		//Check if the element is in the object

		Type n;
		bool expected_member;

		std::cin >> n;
		std::cin >> expected_member;

		bool actual_member = object->member(n );

		if(actual_member == expected_member){
			std::cout << "Okay" << std::endl;
		} else {
			std::cout << ": Failed member(" << n << "): expecting the value '" << expected_member << "' but got '" << actual_member << "'" << std::endl;
		}
	} else if(command == "bin"){
		//Check the element in the specified bin

		int n;
		Type expected_value;

		std::cin >> n;
		std::cin >> expected_value;

		Type actual_value = object->bin(n );

		if(actual_value == expected_value){
			std::cout << "Okay" << std::endl;
		} else {
			std::cout << ": Failed bin(" << n << "): expecting the value '" << expected_value << "' but got '" << actual_value << "'" << std::endl;
		}
	} else if(command == "insert"){
		//Insert the next integer read onto the hash table

		Type n;

		std::cin >> n;

		object->insert(n );
		std::cout << "Okay" << std::endl;
        } else if(command == "insert!"){
                //Cannot insert due to the table being full

                Type n;

                std::cin >> n;

                try {
                        object->insert(n );
                        std::cout << "Failed insert(" << n << "): expecting to catch an exception but did not" << std::endl;
                } catch(overflow){
                        std::cout << "Okay" << std::endl;
                } catch (...) {
                        std::cout << "Failed insert(" << n << "): expecting an overflow exception but caught a different exception" << std::endl;
                }
	} else if(command == "erase"){
		//Check the element in the specified bin

		Type n;
		bool expected_value;

		std::cin >> n;
		std::cin >> expected_value;

		bool actual_value = object->erase(n );

		if(actual_value == expected_value){
			std::cout << "Okay" << std::endl;
		} else {
			std::cout << ": Failed erase(" << n << "): expecting the value '" << expected_value << "' but got '" << actual_value << "'" << std::endl;
		}
	} else if(command == "clear"){
		object->clear();

		std::cout << "Okay" << std::endl;
	} else if(command == "cout"){
		std::cout << *object << std::endl;
	} else {
		std::cout << command << ": Command not found." << std::endl;
	}
}
#endif
