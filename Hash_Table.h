#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#ifndef nullptr
#define nullptr 0
#endif

#include "Exceptions.h"
#include "Mem_Allocation.h"

enum bin_state_t { UNOCCUPIED, OCCUPIED, ERASED };

template <typename Type>
class Hash_table {
	private:
		int count;
		int power;
		int array_size;
		int mask;
		Type *array;
		bin_state_t *occupied;
		int empty_bin;				//Keeps count of empty bins

		int hash( Type const & ) const;

	public:
		Hash_table( int = 5 );
		~Hash_table();
		int size() const;
		int capacity() const;
		double load_factor() const;
		bool empty() const;
		bool member( Type const & ) const;
		Type bin( int ) const;

		void print() const;

		void insert( Type const & );
		bool erase( Type const & );
		void clear();

	// Friends

	template <typename T>
	friend std::ostream &operator<<( std::ostream &, Hash_table<T> const & );
};

//Constructor
template <typename Type>
Hash_table<Type>::Hash_table( int m ):
count( 0 ), power( m ),
array_size( 1 << power ),
mask( array_size - 1 ),
array( new Type[array_size] ),
occupied( new bin_state_t[array_size] ) {
	this->empty_bin = 0;
	for ( int i = 0; i < array_size; i++ ) {
		occupied[i] = UNOCCUPIED;
	}
}

// Your implementation here

//Desctructor
//Free up mem allocated by constructor
template<typename Type>
Hash_table<Type>::~Hash_table() {
	delete[] occupied;					//Deallocates mem for state array of hash table
	delete[] array;						//Deallocates mem for key array of hash table
}

//Hash frunction: modified quadratic probing
template<typename Type>
int Hash_table<Type>::hash(Type const &obj) const {
	int i = static_cast<int>(obj) % this->array_size;
	if(i < 0)
	{
		i += this->array_size;
	}
	return i;
}

//Accessors
template<typename Type>
int Hash_table<Type>::size() const {
	return this->count;					//Returns the number of elements in the hash table
}

template<typename Type>
int Hash_table<Type>::capacity() const {
	return this->array_size;			//Returns the total size of the hash table
}

template<typename Type>
double Hash_table<Type>::load_factor() const {
	double ratio = static_cast<double>(count + empty_bin) / this->array_size;

	return ratio;
}

template<typename Type>
bool Hash_table<Type>::empty() const {
	return(this->count == 0);			//Returns true if hash table has no elements and returns false if it has
}

template<typename Type>
bool Hash_table<Type>::member(Type const &obj) const {
	//Hash obj to find initial bin
	int probe = this->hash(obj);
	int offset = 1;
	int counter = this->array_size;
	//Loop through array to find whether or not obj is an element
	while(this->occupied[probe] != UNOCCUPIED)
	{
		if(this->occupied[probe] == ERASED)
		{
			probe = (probe + offset) % this->array_size;
			counter -= 1;
			offset +=1;
			if(counter == 0)
			{
				break;
			}
		}
		else
		{
			//If obj is found, return true
			if(this->array[probe] == obj)
			{
				return true;
			}
			//Else, go to next offset and check again
			else
			{
				probe = (probe + offset) % this->array_size;
				counter -= 1;;
				offset += 1;
				//If counter goes down to 0, entire array has been searched
				if(counter == 0)
				{
					//Return false since element is not part of array
					break;
				}
			}
		}
	}
	return false;
}

template<typename Type>
Type Hash_table<Type>::bin(int n) const {
	return this->array[n];				//Returns element stored in location n
}

//Mutators
template<typename Type>
void Hash_table<Type>::insert(Type const &obj) {
	//Check if table is full, throw overflow if it is
	if(this->count >= this->array_size)
	{
		throw overflow();
	}
	//If obj is a member, don't do anything
	else if(this->member(obj))
	{
		return;
	}
	//If not, hash obj and go from there
	else
	{
		int probe = this->hash(obj);
		int offset = 1;
		//Loop through to find the next empty or unoccupied location
		while(this->occupied[probe] == OCCUPIED)
		{
			probe = (probe + offset) % this->array_size;
			offset += 1;
		}
		if(this->occupied[probe] == ERASED)
		{
			if(this->empty_bin == 0)
			{
				this->empty_bin = 0;
			}
			else
			{
				this->empty_bin--;
			}
		}
		//Insert new element at empty location, change state at location and increment count
		this->array[probe] = obj;
		this->occupied[probe] = OCCUPIED;
		this->count++;
		return;
	}
}

template<typename Type>
bool Hash_table<Type>::erase(Type const &obj) {
	//Check if obj is a member of the hash table
	if(this->member(obj))
	{
		int probe = this->hash(obj);
		int offset = 1;
		//If location is not unoccupied
		while(this->occupied[probe] != UNOCCUPIED)
		{
			if(this->occupied[probe] == ERASED)
			{
				probe = (probe + offset) % this->array_size;
				offset += 1;
			}
			else
			{
				//If obj is in location, break out of loop
				if(this->array[probe] == obj)
				{
					break;
				}
				//If obj is not in location, apply offset and check again
				else
				{
					probe = (probe + offset) % this->array_size;
					offset += 1;
				}
			}
		}
		//After obj is found, change state in state array, delete element at that location and decrement number of elements in hash table
		this->occupied[probe] = ERASED;
		//this->array[probe] = 0;
		this->count--;
		this->empty_bin++;
		return true;
	}
	else
	{
		return false;
	}
}

template<typename Type>
void Hash_table<Type>::clear() {
	//Loop through all locations in hash table
	//Reset all states
	//Delete all elements
	//Set count to 0
	for(int i = 0; i < this->array_size; i++)
	{
		this->occupied[i] = UNOCCUPIED;
		//this->array[i] = 0;
	}
	this->count = 0;
	this->empty_bin = 0;
	return;
}

template <typename T>
std::ostream &operator<<( std::ostream &out, Hash_table<T> const &hash ) {
	for ( int i = 0; i < hash.capacity(); ++i ) {
		if ( hash.occupied[i] == UNOCCUPIED ) {
			out << "- ";
		} else if ( hash.occupied[i] == ERASED ) {
			out << "x ";
		} else {
			out << hash.array[i] << ' ';
		}
	}

	return out;
}

#endif
