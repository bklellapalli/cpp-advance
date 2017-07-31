#ifndef SharedPtr_HPP
#define SharedPtr_HPP
#include<iostream>
#include <thread>
#include <mutex>

namespace cs540 {

static std::mutex mtx;  

template<typename T> 
void DeleteReferencePointer(void *p) { 
	delete static_cast<T*>(p); 
}

class ReferenceCounter {
	int counter = 0;
	public:
		void *ptr;
		void (*deleter)(void *);
		const std::type_info& (*typeInfo)(void *);
		int getCount() { return counter; }
		void increment() { counter++; } 
		void decrement() { counter--; } 
};

template <typename T>
class SharedPtr {
	public:	
		ReferenceCounter* ref_count; // Reference count
		T* ptr; //Object of another class
	
	/*------------------------- Public Member Functions ------------------------- */
			
		//Constructs a smart pointer that points to null.
		SharedPtr() : ptr(nullptr), ref_count(nullptr) { }

		//Constructs a smart pointer that points to object. The reference count is one. 
		template <typename U> 
		explicit SharedPtr(U* obj) {
			mtx.lock();
			ref_count = new ReferenceCounter();
			ref_count->increment();
			ref_count->ptr = static_cast<void*>(obj);
			ref_count->deleter = &DeleteReferencePointer<U>;
			ptr = obj;
			mtx.unlock();
		}
		
		template <typename U> 
		SharedPtr(U* obj, ReferenceCounter* ref):  ptr(obj), ref_count(ref) {	}

		// reference count is incremented
		SharedPtr(const SharedPtr &p) : ptr(p.ptr), ref_count(p.ref_count)  {
			mtx.lock();
			if(p.ptr != nullptr) { ref_count->increment(); }
			mtx.unlock();
		}

		// reference 	ptr = obj;count is incremented
		template <typename U>
		SharedPtr(const SharedPtr<U> &p) : ptr(p.ptr), ref_count(p.ref_count)  {
			mtx.lock();
			if(p.ptr != nullptr) { ref_count->increment(); }
			mtx.unlock();
		}
		
		//Move the managed object
		SharedPtr(SharedPtr &&p) {
			mtx.lock();
			if(this != &p && p.ptr != nullptr) {
				ptr = std::move(p.ptr);
				ref_count = std::move(p.ref_count); 
				p.ptr = nullptr;
				p.ref_count = nullptr; 
			}
			mtx.unlock();
		}
		
		//Move the managed object
		template <typename U>
		SharedPtr(SharedPtr<U> &&p) {
			mtx.lock();
			if(this != &p && p.ptr != nullptr) {
				ptr = std::move(p.ptr);
				ref_count = std::move(p.ref_count);
				p.ptr = nullptr;
				p.ref_count = nullptr;
			}
			mtx.unlock();
		}
		
		//Copy assignment
		SharedPtr& operator= (const SharedPtr &p){ 
			mtx.lock();
			if(ptr != p.ptr) { // Check self assignment
				if(ptr != nullptr) {
					ref_count->decrement(); 
					if(ref_count->getCount() == 0) {
						(*ref_count->deleter)(ref_count->ptr); //delete ptr;
						delete ref_count;					
					}
					ptr = nullptr;
					ref_count = nullptr;
				}
				if (p.ptr != nullptr) { 
					ptr = p.ptr;
					ref_count = p.ref_count;
					ref_count->increment(); //  increment reference count
				}
			}
			mtx.unlock();
			return *this;
		}
    		
    		//Copy assignment
    		template <typename U>
    		SharedPtr<T> &operator=(const SharedPtr<U> &p) { 
			mtx.lock();
			if(ptr != p.ptr) { // Check self assignment
				if(ptr != nullptr) {
					ref_count->decrement(); 
					if(ref_count->getCount() == 0) {
						(*ref_count->deleter)(ref_count->ptr); //delete ptr;
						delete ref_count;					
					}
					ptr = nullptr;
					ref_count = nullptr;
				}
				if (p.ptr != nullptr) { 
					ptr = p.ptr;
					ref_count = p.ref_count;
					ref_count->increment(); //  increment reference count
				}
			}
			mtx.unlock();
			return *this;
		}
    		
		//Move assignment
		SharedPtr &operator=(SharedPtr &&p) {
			mtx.lock();
			if(this != &p  && p.ptr != nullptr) {
				// reference count must remain unchanged
				ptr = std::move(p.ptr);
				ref_count = std::move(p.ref_count);
				p.ptr = nullptr;
				p.ref_count = nullptr; 
			} 
			mtx.unlock();
			return *this;
		}
		
		// Move assignment
		template <typename U>
		SharedPtr &operator=(SharedPtr<U> &&p) {
			mtx.lock();
			if(this != &p  && p.ptr != nullptr) {
				// reference count must remain unchanged
				ptr = std::move(p.ptr);
				ref_count = std::move(p.ref_count);
				p.ptr = nullptr;
				p.ref_count = nullptr;
			}
			mtx.unlock();
			return *this;
		}
		
		// Decrement reference count. Delete the object (reference count is 0)
		~SharedPtr() {
			mtx.lock();
			if(ptr != nullptr) {
				ref_count->decrement();
				if(ref_count->getCount() == 0) {
					(*ref_count->deleter)(ref_count->ptr); //delete ptr;
					delete ref_count;
				}
			}
			mtx.unlock();
		}
		
		// smart pointer is set to point to the null pointer
		void reset() {
			mtx.lock();
			if(ptr != nullptr) {
				ref_count->decrement();
				if(ref_count->getCount() == 0) {
					(*ref_count->deleter)(ref_count->ptr); //delete ptr;
					delete ref_count;
					ptr = nullptr;
					ref_count = nullptr;
				}
				ptr = nullptr; // Smart pointer is set to null pointer. 
			}
			mtx.unlock();  
		}
		
		// smart pointer is set to point to the null pointer
		template <typename U> 
		void reset(U *p) {
			mtx.lock();
			if(ptr != nullptr) {
				ref_count->decrement();
				if(ref_count->getCount() == 0) {
					(*ref_count->deleter)(ref_count->ptr); //delete ptr;
					delete ref_count;
					ptr = nullptr;
					ref_count = nullptr;
				}
			} 			
			ref_count = new ReferenceCounter();
			ref_count->increment();
			ref_count->ptr = static_cast<void*>(p);
        		ref_count->deleter = &DeleteReferencePointer<U>;
        		ptr = p;
        		mtx.unlock();
		}
		
		// Returns a pointer to the owned object
		T *get() const { return this->ptr; }
		
		// pointer is returned. 
		T *operator->() { return ptr; }
		
		// reference to the pointed-to object is returned
		T &operator*() { return *ptr; } 
		
		// Returns true if the SharedPtr is not null. 
		explicit operator bool() const { return (ptr != nullptr) ? true : false; }
		
		/*------------------------- Non-member (Free) Functions ------------------------- */		
		template <typename T1, typename T2> friend bool operator==(const SharedPtr<T1> &, const SharedPtr<T2> &);
		template <typename T1> friend bool operator==(const SharedPtr<T1> &, std::nullptr_t);
		template <typename T1> friend bool operator==(std::nullptr_t, const SharedPtr<T1> &);
		template <typename T1, typename T2> friend bool operator!=(const SharedPtr<T1>&, const SharedPtr<T2> &);
		template <typename T1> friend bool operator!=(const SharedPtr<T1> &, std::nullptr_t);
		template <typename T1> friend bool operator!=(std::nullptr_t, const SharedPtr<T1> &);
		template <typename T1, typename U1> friend SharedPtr<T1> static_pointer_cast(const SharedPtr<U1> &);
		template <typename T1, typename U1> friend SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U1> &);
};

/*------------------------- Non-member (Free) Functions ------------------------- */	

// Returns true if the two smart pointers point to the same object or if they are both null
template <typename T1, typename T2> 
bool operator==(const SharedPtr<T1> &sp1, const SharedPtr<T2> &sp2) {
	return (sp1.ptr == sp2.ptr) ? true : false; 
}

//Compare the SharedPtr against nullptr.
template <typename T1> 
bool operator==(const SharedPtr<T1> &sp, std::nullptr_t nullp) {
	return (sp.ptr == nullp) ? true : false;
}

//Compare the SharedPtr against nullptr.
template <typename T1> 
bool operator==(std::nullptr_t nullp, const SharedPtr<T1> &sp) {
	return (sp.ptr == nullp) ? true : false;
}

// True if the SharedPtrs point to different objects, or one points to null while the other does not. 	
template <typename T1, typename T2> 
bool operator!=(const SharedPtr<T1>&sp1, const SharedPtr<T2> &sp2) {
	return !(sp1 == sp2);
}
	
// Compare the SharedPtr against nullptr. 		
template <typename T1> 
bool operator!=(const SharedPtr<T1> &sp, std::nullptr_t nullp) {
	return (sp.ptr == nullp) ? false : true;
}
	
// Compare the SharedPtr against nullptr. 		
template <typename T1> 
bool operator!=(std::nullptr_t nullp, const SharedPtr<T1> &sp) {
	return (sp.ptr == nullp) ? false : true;
}


// Convert sp by using static_cast to cast the contained pointer. 		
template <typename T1, typename U1> 
SharedPtr<T1> static_pointer_cast(const SharedPtr<U1> &sp) {
	if(static_cast<T1*>(sp.ptr) == nullptr) {
		return SharedPtr<T1>();
	}
	return SharedPtr<T1>(static_cast<T1*>(sp.ptr), sp.ref_count);
}
		
//Convert sp by using dynamic_cast to cast the contained pointer
template <typename T1, typename U1> 
SharedPtr<T1> dynamic_pointer_cast(const SharedPtr<U1> &sp) {
	if(dynamic_cast<T1*>(sp.ptr) == nullptr) {
		return SharedPtr<T1>();
	}
	return SharedPtr<T1>(dynamic_cast<T1*>(sp.ptr), sp.ref_count);
}		
}
#endif
