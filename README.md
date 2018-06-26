# Hash-Table
This is a project that is inspired by a given project in my Algorithm and Data Structure course.

Hash Function:

    object statically cast as an int, taking this integer module M (i % M), and adding M if the value is negative.

Functions:

    int size() const
        Returns the number of elements currently stored in the hash table.
    int capacity() const
        Returns the number of bins in the hash table.
    double load_factor() const
        Returns the load factor of hash table (see static_cast<double>(...)). This should be the ratio of occupied and erased bins over the total number of bins.
    bool empty() const
        Returns true if the hash table is empty, false otherwise.
    bool member( Type const & ) const
        Returns true if object obj is in the hash table and false otherwise.
    Type bin( int n ) const
        Return the entry in bin n. The behaviour of this function is undefined if the bin is not filled. It will only be used to test locations that are expected to be filled by specific values.
    void print() const
        A function which you can use to print the class in the testing environment. This function will not be tested.
    void insert( Type const & )
        Insert the argument into the hash table in the appropriate bin as determined by the aforementioned hash function and the rules of quadratic hashing. If the table is full, thrown an overflow exception. If the hash table is not full and the argument is already in the hash table, do nothing. An object can be placed either into an empty or deleted bin. Do not rehash the entries even if there are many erased bins.
    bool erase( Type const & )
      Remove the argument from the hash table if it is in the hash table (returning false if it is not) by setting the corresponding flag of the bin to deleted.
    void clear()
      Removes all the elements in the hash table by setting all entries to unoccupied.
