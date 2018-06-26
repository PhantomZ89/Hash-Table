#ifndef mem_alloc
#define mem_alloc

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cmath>
#include <ctime>
#include "Exceptions.h"

namespace mem_alloc {
	int memory_alloc_store;

	const size_t PAD = 16;

	class overflow {};
	class invalid_deletion {};
	class invalid_input {};

	//Each time a user calls either new or new[],
	//the information about the memory allocation is stored in an instance of this class

	class Stopwatch {
		private:
 			clock_t start_time;
 			float duration;

		public:
			Stopwatch() {
				duration = 0;
			}

			void start() {
 				start_time = std::clock();
			}

			void stop() {
 				clock_t end_time = std::clock();
 				//This supposed to be milisecond
 				duration = ((float)end_time - start_time)/CLOCKS_PER_SEC;
			}

 			float get_last_duration() const {
				return duration;
			}
	};

	class Allocation {
		public:
			void *address;
			size_t size;
			bool is_array;
			bool deleted;

			Allocation():
			address(0),
			size(0),
			is_array(false),
			deleted(false) {
				//Empty constructor
			}

			Allocation(void *a, size_t s, bool i):
			address(a),
			size(s),
			is_array(i),
			deleted(false) {
				//Empty constructor
			}
	};

	int to_int(int *ptr) {
		int result = *ptr;

		if (result < 0) {
			result = result + (1 << (sizeof(int) - 1));
		}

		return result >> 3;
	}

	//All instances of an allocation are stored in this chained hash table

	class HashTable {
		private:
			int array_size;
			Allocation *allocated;
			int total_memory_alloc;
			int total_memory_deleted;
			bool record;

		public:
			//Initialize all of the addresses to 0

			HashTable(int as):
			array_size(as),
			total_memory_alloc(0),
			total_memory_deleted(0),
			record(false) {
				allocated = new Allocation[array_size];
			}

			void reserve(int N) {
				//N must be a power of 2

				if ((N & ((~N) + 1)) != N) {
					throw illegal_argument();
				}

				delete [] allocated;
				array_size = N;
				allocated = new Allocation[array_size];
			}

			int memory_alloc() const {
				return total_memory_alloc - total_memory_deleted;
			}

			void memory_store() const {
				memory_alloc_store = total_memory_alloc - total_memory_deleted;
			}

			void memory_change(int n) const {
				int memory_alloc_diff = total_memory_alloc - total_memory_deleted - memory_alloc_store;

				if (memory_alloc_diff != n) {
					std::cout << "WARNING: expecting a change in memory allocation of "
					          << n << " bytes, but the change was " << memory_alloc_diff
					          << std::endl;
				}
			}

			//Insert uses the last log[2](array_size) bits of the address as the hash function
			//It finds an unallocated entry in the array and stores the information
			//about the memory allocation in that entry, including:
			//The amount of memory allocated,
			//New or new[] was used or not, and
			//The address of allocated memory.

			void insert(void *ptr, size_t size, bool is_array) {
				if (!record) {
					return;
				}

				//The hash function is the last log[2](array_size) bits
				int hash = to_int(reinterpret_cast<int *> (&ptr)) & (array_size - 1);

				for (int i = 0; i < array_size; ++i) {
					//It may be possible that we are allocated the same memory
					//location twice (if there are numerous allocations and deallocations of memory)
					//Thus, the second check is necessary,
					//otherwise it may introduce session dependant errors.

					if (allocated[hash].address == 0 || allocated[hash].address == ptr) {
						//Store address, amount of memory allocated,
						//whether or not new[] was used, and set 'deleted' to false.

						allocated[hash] = Allocation(ptr, size, is_array);

						//Add memory allocated to the total memory allocated.
						total_memory_alloc += size;

						return;
					}

					hash = (hash + 1) & (array_size - 1);
				}

				std::cout << "WARNING: allocating more memory than is allowed for this project" << std::endl;
				throw overflow();
			}

			//Remove checks:
			//If the given memory location was allocated in the first place, and
			//If the appropriate form of delete was used, i.e., delete versus delete[], and
			//If delete has already been called on this object

			size_t remove(void *ptr, bool is_array) {
				if (!record || ptr == 0) {
					return 0;
				}

				// the hash function is the last log[2](array_size) bits
				int hash = to_int(reinterpret_cast<int *> (&ptr)) & (array_size - 1);

				//Continue searching until all bins checked
				//or find empty bin.

				for (int i = 0; i < array_size && allocated[hash].address != 0; ++i) {
					if (allocated[hash].address == ptr) {
						//Check if wrong delete was called (e.g., delete[] when new was used or delete when new[] was used)
						//and if memory has already been deallocated previously.
						
						//If the deletion is successful, then set the 'deleted' flag to 'true'
						//and add the memory deallocated ot the total memory deallocated.
						if (allocated[hash].is_array  != is_array) {
							if (allocated[hash].is_array) {
								std::cout << "WARNING: use 'delete [] ptr;' to free memory allocated with 'ptr = new Class[array_size];'" << std::endl;
							} else {
								std::cout << "WARNING: use 'delete ptr;' to free memory allocated with 'ptr = new Class(...);'" << std::endl;
							}

							throw invalid_deletion();
						} else if (allocated[hash].deleted) {
							std::cout << "WARNING: calling delete twice on the same memory location: " << ptr << std::endl;
							throw invalid_deletion();
						}

						allocated[hash].deleted = true;
						total_memory_deleted += allocated[hash].size;

						//Zero the memory before it is deallocated
						char *cptr = static_cast<char *> (ptr);

						for (size_t j = 0; j < allocated[hash].size; ++j) {
							cptr[j] = 0;
						}

						return allocated[hash].size;
					}

					hash = (hash + 1) & (array_size - 1);
				}

				//If we've gotten this far, this means that the address was
				//never allocated, and therefore we are calling delete on
				//something which should be deleted.

				std::cout << "WARNING: deleting a pointer to which memory was never allocated: " << ptr << std::endl;
				throw invalid_deletion();
			}

			//Print a difference between the memory allocated and the memory deallocated
			void summary() {
				std::cout << "Memory allocated minus memory deallocated: "
				     << total_memory_alloc - total_memory_deleted << std::endl;
			}

			//Print the difference between total memory allocated and total memory deallocated.
			void details() {
				std::cout << "SUMMARY OF MEMORY ALLOCATION:" << std::endl;

				std::cout << "  Memory allocated:   " << total_memory_alloc << std::endl;
				std::cout << "  Memory deallocated: " << total_memory_deleted << std::endl << std::endl;

				std::cout << "INDIVIDUAL REPORT OF MEMORY ALLOCATION:" << std::endl;
				std::cout << "  Address  Using  Deleted  Bytes   " << std::endl;

				for (int i = 0; i < array_size; ++i) {
					if (allocated[i].address != 0) {
						std::cout << "  " << allocated[i].address
						          << (allocated[i].is_array ? "  new[]     " : "  new       ")
						          << (allocated[i].deleted  ? "Y    " : "N    ")
						          << std::setw(6)
						          << allocated[i].size << std::endl;
					}
				}
			}

			//Start recording memory allocations
			void start_recording() {
				record = true;
			}

			//Stop recording memory allocations
			void stop_recording() {
				record = false;
			}

			bool is_recording() {
				return record;
			}
	};

	bool asymptotic_tester(double *array, int N, int k, bool ln) {
		double *ratios = new double[N];
		double *differences = new double[N- 1];

		int M = 2;

		for (int i = 0; i < N; ++i) {
			ratios[i] = array[i] / (M*(ln ? std::log(static_cast<double>(M)) : 1.0));

			M = M*(1 << k);

		}

		for (int i = 0; i < N - 1; ++i) {
			differences[i] = ratios[i + 1] - ratios[i];
			//std::cout << differences[i] << std::endl;
		}

		for (int i = 1; i < N - 1; ++i) {
			if (!(differences[i] < 0)) {
				if (differences[i] > differences[i - 1]) {
					delete [] ratios;
					delete [] differences;

					return false;
				}
			}
		}

		delete [] ratios;
		delete [] differences;

		return true;
	}

	HashTable allocation_table(8192);

	std::string history[1000];
	int count = 0;

	void check_array_bounds(char *ptr, size_t size) {
		for (size_t i = 0; i < PAD; ++i) {
			if (ptr[i] != 'U') {
				std::cout << "Memory before the array located at adderss "
				          << static_cast<void *> (ptr + PAD) << " was overwritten" << std::endl;
				throw out_of_range();
			}

			if (ptr[size - 1 - i] != 'U') {
				std::cout << "Memory after the array located at adderss "
				          << static_cast<void *> (ptr + PAD) << " was overwritten" << std::endl;

				throw out_of_range();
			}
		}

		bool used = false;

		for (size_t i = 0; i < size - 2*PAD; ++i) {
			if (ptr[PAD + i] != 'U') {
				used = true;
				break;
			}
		}

		if (!used) {
			std::cout << "The memory allocated at adderss "
			          << static_cast<void *> (ptr + PAD) << " was not used" << std::endl;
		}

		std::memset(ptr, 'U', size);
	}

}


void *operator new(size_t size) throw(std::bad_alloc) {
	void *ptr = malloc(size);
	mem_alloc::allocation_table.insert(ptr, size, false);
	return static_cast<void *> (ptr);
}

void operator delete(void *ptr) throw () {
	mem_alloc::allocation_table.remove(ptr, false);
	free(ptr);
}

void *operator new[](size_t size) throw(std::bad_alloc) {
	char *ptr = static_cast<char *> (malloc(size + 2*mem_alloc::PAD));
	mem_alloc::allocation_table.insert(static_cast<void *> (ptr + mem_alloc::PAD), size, true);
	mem_alloc::initialize_array_bounds(ptr, size + 2*mem_alloc::PAD);
	return static_cast<void *> (ptr + mem_alloc::PAD);
}

void operator delete[](void *ptr) throw () {
	size_t size = mem_alloc::allocation_table.remove(ptr, true);

	if(mem_alloc::allocation_table.is_recording()) {
		mem_alloc::check_array_bounds(static_cast<char *> (ptr) - mem_alloc::PAD, size + 2*mem_alloc::PAD);
	}

	free(static_cast<char *>(ptr) - mem_alloc::PAD);
}

#endif
