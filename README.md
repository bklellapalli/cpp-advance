# std::map implementation

Implement a container class template named Map similar to the std::map class from the C++ Standard Library. Such containers implement key-value pairs, where key and value may be any types, including class types. (In the following, the value will be referred to as the mapped type or mapped object, and the term value will used to designate the entire pair. This is so as to be consistent with the terminology in the standard library.) Note that C++ terminology uses object even for fundamental types such as ints. Your Map class template will have two type parameters, Key_T and Mapped_T, denoting the key type and the mapped type, respectively. As in std::map, the mapped type values themselves must be in your map, not pointers to the values.

You may assume that the Key_T and Mapped_T are copy constructible and destructible. You may assume that Key_T has a less-than operator (<), and an equality operator (==), as free-standing functions (not member functions). You may also assume that Mapped_T has an equality comparison (==). If operator< is invoked on Map objects, you may also assume that Mapped_T has operator<. You may not assume that either class has a default constructor or an assignment operator. You may only assume that a Mapped_T that is used with operator[] may be default initialized. 

Your Map class must expose three nested classes: Iterator, ConstIterator, and ReverseIterator. None of these classes should permit default construction.
An iterator is an object that points to an element in a sequence. The iterators must traverse the Map by walking through the keys in sorted order. Iterators must remain valid as long as the element they are pointing to has not been erased. Any function that results in the removal of an element from a map, such as erase, will invalidate any iterator that points to that element, but not any other iterator.


Additionally, your class must meet the following time complexity requirements: O(lg(N)) for key lookup, insertion, and deletion; and O(1) for all iterator increments and decrements. 


# shared pointer implementation

Implement a non-intrusive, thread-safe, exception-safe, reference-counting smart pointer named cs540::SharedPtr. Our model for this will be std::shared_ptr

SharedPtr must allow different smart pointers in different threads to be safely assigned and unassigned to the same shared objects. You may use either locks or atomic increment operations to ensure thread-safety. You do not need to (and in fact should not) ensure that the same SharedPtr can be used concurrently.

Note that when the smart pointer determines that the object should be deleted, it must delete the object via a pointer to the original type, even if the template argument of the final smart pointer is of a base type. This is to avoid object slicing for non-virtual destructors.
You may (and should) create any helper classes you feel necessary.

NOTE: We are using the term “object” as in the standard, where it refers to any region of storage except functions. Thus, these smart pointers should be able to work with fundamental types as well as objects of class type.


# interpolate
