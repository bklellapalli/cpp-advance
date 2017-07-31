#ifndef MAP_HPP
#define MAP_HPP

#include<stdlib.h>
#include<time.h>
#include <utility>
#include<iostream>
#include <stdexcept>
#include <initializer_list>

namespace cs540 {

#define MAX_LEVEL 32
#define PROBABILITY 0.2
#define DEFAULT_LEVEL 1
#define DEFAULT_HEIGHT 0
#define CACHE_SIZE 10

template <class Key_T, class Mapped_T> class Map;
template <class Key_T, class Mapped_T> bool operator==(const Map<Key_T, Mapped_T> &, const Map<Key_T, Mapped_T> &);
template <class Key_T, class Mapped_T>  bool operator!=(const Map<Key_T, Mapped_T> & , const Map<Key_T, Mapped_T> &);
template <class Key_T, class Mapped_T>  bool operator<(const Map<Key_T, Mapped_T> &, const Map<Key_T, Mapped_T> &);

/* -------------------------- Node Structure -------------------------- */
template <class Key_T, class Mapped_T> 
struct Node {
	struct Node *next, *prev, *up, *down;
	std::pair<Key_T, Mapped_T> p;
	Node(std::pair<Key_T, Mapped_T> _p) : next(NULL), prev(NULL), up(NULL), down(NULL), p(_p) { }
};

/*-------------------------- Cache ---------------------------------*/
template <class Key_T, class Mapped_T> 
struct Cache {
	struct Cache *next, *prev;
	Node<Key_T, Mapped_T>* node;
};

/* -------------------------- Skiplist Class -------------------------- */
template <class Key_T, class Mapped_T> 
class Skiplist {
	int height, size;
	Node<Key_T, Mapped_T>* head = NULL;
	Node<Key_T, Mapped_T>* tail = NULL;		
	int getLevel();
	void initialize(std::pair<const Key_T, Mapped_T>, Node<Key_T, Mapped_T>**, Node<Key_T, Mapped_T>**, int, int);
	void setUPDownLink(Node<Key_T, Mapped_T>**, Node<Key_T, Mapped_T>**);
	void setNextPrevLink(Node<Key_T, Mapped_T>**, Node<Key_T, Mapped_T>**);
	void freeNode(Node<Key_T, Mapped_T>**, Node<Key_T, Mapped_T>**);
	Node<Key_T, Mapped_T>* searchKey(const Key_T) const;
	Node<Key_T, Mapped_T>* insertPair(std::pair<const Key_T, Mapped_T>);
	void removeKey(Key_T);
	friend class Map<Key_T, Mapped_T>;
	friend bool operator== <>(const Map<Key_T, Mapped_T> &, const Map<Key_T, Mapped_T> &);
	friend bool operator!= <>(const Map<Key_T, Mapped_T> & , const Map<Key_T, Mapped_T> &);
	friend bool operator< <>(const Map<Key_T, Mapped_T> &, const Map<Key_T, Mapped_T> &);
};


/* -------------------------- Map Class -------------------------- */
template<class Key_T, class Mapped_T>
class Map {
	typedef std::pair<Key_T, Mapped_T> ValueType;
	private:
		int cacheCount = 0;
		Cache<Key_T, Mapped_T>* cache = NULL;
		Skiplist<Key_T, Mapped_T> skiplist;
		Node<Key_T, Mapped_T>* findFirstNode() const;
		Node<Key_T, Mapped_T>* findLastNode() const;
		Node<Key_T, Mapped_T>* retrieveCache(Key_T) const;
		void insertCache(Node<Key_T, Mapped_T>*);
		void removeCache(Key_T);
		void clearCache();
	public:	
		/* ----------------------- Iterator Class ---------------------- */
		class Iterator {
			public:
			Node<Key_T, Mapped_T>* current;
			Iterator & operator++() { current = current->next; return *this; }
			Iterator & operator--() { current = current->prev; return *this; }
			Iterator operator++(int) { 
				Iterator it; 
				it.current = current; 
				current = current->next; 
				return it; 
			}
			Iterator operator--(int) { 
				Iterator it; 
				it.current = current; 
				current = current->prev; 
				return it; 
			}
			ValueType & operator*() const { return this->current->p; }
			ValueType * operator->() const {return &(current->p); }
			friend class Map<Key_T, Mapped_T>;
		};
		/* ----------------------- Const Iterator Class ----------------------- */
		class ConstIterator {
			public:
			Node<Key_T, Mapped_T>* current;
			ConstIterator & operator++() { current = current->next;	return *this; } 
			ConstIterator & operator--() { current = current->prev; return *this; } 
			ConstIterator operator++(int) { 
				ConstIterator it; 
				it.current = current; 
				current = current->next; 
				return it; 
			}	
			ConstIterator operator--(int) { 
				ConstIterator it; 
				it.current = current; 
				current = current->prev; 
				return it; 
			}
			const ValueType & operator*() const { return current->p; }
			const ValueType * operator->() const { return &(current->p); }
			friend class Map<Key_T, Mapped_T>;
		};
		/* -------------------------- Reverse Iterator Class -------------------------- */
		class ReverseIterator {
			public:
			Node<Key_T, Mapped_T>* current;
			ReverseIterator & operator++() { current = current->prev; return *this; }
			ReverseIterator & operator--() { current = current->next; return *this; } 	
			ReverseIterator operator++(int) { 
				ReverseIterator it; 
				it.current = current; 
				current = current->prev; 
				return it; 
			}
			ReverseIterator operator--(int) { 
				ReverseIterator it; 
				it.current = current; 
				current = current->next; 
				return it; 
			}
			ValueType & operator*() const { return current->p; }
			ValueType * operator->() const { return &(current->p); }
			friend class Map<Key_T, Mapped_T>;
		};	
	public:
		Map() { }
		Map(const Map<Key_T, Mapped_T> &);
		Map& operator= (const Map<Key_T, Mapped_T> &);
		Map(std::initializer_list<std::pair<const Key_T, Mapped_T>>);
		~Map() { clear(); }
		
		int size() const { return skiplist.size; }
		bool empty() const { return (skiplist.size == DEFAULT_HEIGHT); }
		Iterator begin() { 
			Iterator it; 
			it.current = findFirstNode(); 
			return it;
		}
		Iterator end() { 
			Iterator it; 
			it.current = findLastNode(); 
			return it; 
		}
		ConstIterator  begin() const {
			ConstIterator it; 
			it.current = findFirstNode(); 
			return it; 
		}
		ConstIterator  end() const { 
			ConstIterator it; 
			it.current = findLastNode(); 
			return it; 
		}
		ReverseIterator rbegin() { 
			ReverseIterator it; 
			it.current = findLastNode(); 
			return it; 
		}
		ReverseIterator rend() { 
			ReverseIterator it; 
			it.current = findFirstNode(); 
			return it; 
		}
		Iterator find(const Key_T & key) { 
			Iterator it; 
			it.current = retrieveCache(key); // Look up in cache
			if(it.current == NULL) {
				it.current = skiplist.searchKey(key);
				if (it.current->next != NULL)
					insertCache(it.current); // insert in cache
			} 
			return it; 
		}
		ConstIterator find(const Key_T & key) const { 
			ConstIterator it; 
			it.current = retrieveCache(key); // Loop up in cache
			if(it.current == NULL) {
				it.current = skiplist.searchKey(key);
			} 
			return it; 
		}
		Mapped_T &at(const Key_T & key) { 
			Node<Key_T, Mapped_T>* temp = retrieveCache(key); // look up in cache
			if(temp == NULL) {
				temp = skiplist.searchKey(key);
			}  
			if(temp == NULL || (temp != NULL && temp->next == NULL)) {
				throw std::out_of_range("Not Found!"); 
			}
			insertCache(temp); // insert in cache
			return temp->p.second; 
		}
		const Mapped_T &at(const Key_T & key) const { 
			Node<Key_T, Mapped_T>* temp = retrieveCache(key); // retrieve cache
			if(temp == NULL) {
				temp = skiplist.searchKey(key);
			} 
			if(temp == NULL || (temp != NULL && temp->next == NULL)) {
				throw std::out_of_range("Not Found!"); 
			}
			return temp->p.second; 
		}
		Mapped_T &operator[](const Key_T &);  
		std::pair<Iterator, bool> insert(const ValueType &);
		void erase(const Key_T & key) { 	
			removeCache(key); // Remove node from cache
			skiplist.removeKey(key); 
		}
		void erase(Iterator pos) { 
			removeCache(pos.current->p.first); // Remove node from cache
			skiplist.removeKey(pos.current->p.first); 
		}
		void clear();
		
		/* ------------------------ Operator Overloading (Friend function)---------------------------  */
		
		friend bool operator== <>(const Map<Key_T, Mapped_T> &, const Map<Key_T, Mapped_T> &);
		friend bool operator!= <>(const Map<Key_T, Mapped_T> & , const Map<Key_T, Mapped_T> &);
		friend bool operator< <>(const Map<Key_T, Mapped_T> &, const Map<Key_T, Mapped_T> &);
	
		friend bool operator==(const Iterator & it1, const Iterator & it2) {  return it1.current == it2.current; }
		friend bool operator==(const ConstIterator & it1, const ConstIterator & it2) { return it1.current == it2.current; }
		friend bool operator==(const Iterator & it1, const ConstIterator & it2) { return it1.current == it2.current; }
		friend bool operator==(const ConstIterator & it1, const Iterator & it2) { return it1.current == it2.current; }
		friend bool operator!=(const Iterator & it1, const Iterator & it2) { return it1.current != it2.current; }
		friend bool operator!=(const ConstIterator & it1, const ConstIterator & it2){ return it1.current != it2.current; }
		friend bool operator!=(const Iterator & it1, const ConstIterator & it2) { return it1.current != it2.current; }
		friend bool operator!=(const ConstIterator & it1, const Iterator & it2) { return it1.current != it2.current; }	
		friend bool operator==(const ReverseIterator & it1, const ReverseIterator & it2) { return it1.current != it2.current; }
		friend bool operator!=(const ReverseIterator & it1, const ReverseIterator & it2) { return it1.current != it2.current; }
};

/*------------------------ Skiplist class method(s) ---------------------------------*/
/* Generates ramdon number that determine height at which node needs to be inserted .*/
template <class Key_T, class Mapped_T> 
int Skiplist<Key_T, Mapped_T>  :: getLevel() {
	int level = DEFAULT_LEVEL;
   	srand(time(NULL));
   	for(;level <= height && level < MAX_LEVEL && drand48() < PROBABILITY; level++);
    	return level;
}

/* Initialize Head, Tail and height and size of skip list */
template <class Key_T, class Mapped_T> 
void Skiplist<Key_T, Mapped_T> :: initialize( std::pair<const Key_T, Mapped_T> p, Node<Key_T, Mapped_T>** h_node, 
					Node<Key_T, Mapped_T>** t_node, int lvl, int count) {	
	if(*h_node != NULL) return;
	 *h_node = new Node<Key_T, Mapped_T>(p);
	 *t_node = new Node<Key_T, Mapped_T>(p);
	 (*h_node)->next = *t_node;
	 (*t_node)->prev = *h_node;
	 height = lvl;
	 size = count;
}


/* Set "up" and "down" links for node */
template <class Key_T, class Mapped_T> 
void Skiplist<Key_T, Mapped_T> :: setUPDownLink(Node<Key_T, Mapped_T>** up_node, 
					Node<Key_T, Mapped_T>** down_node) {
	(*up_node)->down = *down_node;
	(*down_node)->up = *up_node;
}

/* Set "next" and "prev" links for node */
template <class Key_T, class Mapped_T> 
void Skiplist<Key_T, Mapped_T> :: setNextPrevLink(Node<Key_T, Mapped_T>** prev_node, 
					Node<Key_T, Mapped_T>** new_node) {
	(*new_node)->next = (*prev_node)->next;
	(*new_node)->prev = *prev_node;
	(*prev_node)->next->prev = *new_node;
	(*prev_node)->next = *new_node;
}

/* Delete node from the skiplist */
template <class Key_T, class Mapped_T> 
void Skiplist<Key_T, Mapped_T> :: freeNode(Node<Key_T, Mapped_T>** prev_temp, 
					Node<Key_T, Mapped_T>** temp) {
	(*prev_temp)->next = (*temp)->next;
	(*temp)->next->prev = *prev_temp;
	delete(*temp);
	if(head->next != tail) return;
	// Free top level and move head and tail one level down
	if(height > 1) {
		head = head->down;
		tail = tail->down;
		delete(head->up);
		delete(tail->up);
		height--;	
	} 
}

/* Search node */
template < class Key_T, class Mapped_T > 
Node<Key_T, Mapped_T>* Skiplist<Key_T, Mapped_T> :: searchKey(const Key_T key) const {
	bool isFound = false;
	Node<Key_T, Mapped_T>* tempHead = head;	
	Node<Key_T, Mapped_T>* tempTail = tail;	
	
	for(; tempHead != NULL; tempHead = tempHead->down) {	
		Node<Key_T, Mapped_T>* temp = tempHead;
		for(; temp != tempTail; temp = temp->next) {
			if(temp->prev != NULL && temp->p.first == key) {
				tempHead = temp;
				isFound = true;
				break;
			}
			else if(temp->prev != NULL && !(temp->p.first < key) && !(temp->p.first == key)) {
				tempHead = temp->prev;
				break;
			}
		}
		if(tempTail->down != NULL) 
			tempTail = tempTail->down;
		else
			break; 
	}
	if(isFound) return tempHead;
	return tempTail;
}

/* Insert node */
template <class Key_T, class Mapped_T> 
Node<Key_T, Mapped_T>* Skiplist<Key_T, Mapped_T> :: insertPair(std::pair<const Key_T, Mapped_T> p) {
	initialize(p, &head, &tail, DEFAULT_HEIGHT, DEFAULT_HEIGHT);
	Node<Key_T, Mapped_T>* tempHead = NULL;
	Node<Key_T, Mapped_T>* tempTail = NULL;
	Node<Key_T, Mapped_T>* upNode = NULL;
	// Generate level
	int lvl = getLevel(); 
	if(lvl > height && lvl != DEFAULT_LEVEL) {	
		// Add new level	
		initialize(p, &tempHead, &tempTail, lvl, size);
		setUPDownLink(&tempHead, &head);
		setUPDownLink(&tempTail, &tail);
		head = tempHead;
		tail = tempTail;
	}
	tempHead = head;
	tempTail = tail;
	height = (lvl > height) ? lvl : height; // Update height
	size++; // Increment size of the skiplist
	// Traverse tempHead and tempTail to level where we have to insert node
	for(int i = height; i > lvl; i--) {
		tempHead = tempHead->down;
		tempTail = tempTail->down;	
	}
	// Insert element in each level starting from the lvl
	for(; tempHead != NULL; tempHead = tempHead->down) {	
		Node<Key_T, Mapped_T>* temp = tempHead;
		for(; temp != tempTail; temp = temp->next) {
			if(temp->next == tempTail || (!(temp->next->p.first < p.first) && !(temp->next->p.first == p.first))) {				
				// Create new node to be added in list
				Node<Key_T, Mapped_T>* newNode = new Node<Key_T, Mapped_T>(p);
				if(upNode != NULL) {
					setUPDownLink(&upNode, &newNode);
				}
				upNode = newNode;
				setNextPrevLink(&temp, &newNode);
				break;
			}
		}
		tempTail = tempTail->down;
		if(tempHead->down == NULL) return temp->next;
	}
}

/* Remove node */
template <class Key_T, class Mapped_T> 
void Skiplist<Key_T, Mapped_T> :: removeKey(Key_T key) {
	bool isRemoved = false;	
	Node<Key_T, Mapped_T>* temp = NULL;
	Node<Key_T, Mapped_T>* tempHead = head;
	for(; tempHead != NULL; tempHead = tempHead->down) {
		temp = tempHead;
		while(temp != NULL) {
			if(temp->prev != NULL && temp->p.first == key) {
				Node<Key_T, Mapped_T>* prev_temp = temp->prev;
				freeNode(&prev_temp, &temp);
				tempHead = prev_temp;
				isRemoved = true;
				break;
			}
			else if(temp->prev != NULL && !(temp->p.first < key) && !(temp->p.first == key)) {
				tempHead = temp->prev;
				break;
			}
			temp = temp->next;	
		}
		if(tempHead->down == NULL && isRemoved) {
			size--;
		}	
	}		
}

/*------------------------ Map class method(s) ---------------------------------*/

/* Insert cache element at the rear */
template <class Key_T, class Mapped_T> 
void Map<Key_T, Mapped_T> :: insertCache(Node<Key_T, Mapped_T>* n) {
	if(cache == NULL) {
		cache = new Cache<Key_T, Mapped_T>();
		cache->prev = NULL;
		cache->next = NULL;
	} 
	if(skiplist.height == cacheCount) {
		clearCache();
	} 
	Cache<Key_T, Mapped_T>* temp = new Cache<Key_T, Mapped_T>();
	temp->node = n;
		
	temp->next = NULL;
	cache->next = temp;
	temp->prev = cache;
	cache = temp;
	cacheCount++;
}

/* Remove least recent used element from the cahce */
template <class Key_T, class Mapped_T> 
void Map<Key_T, Mapped_T> :: clearCache() {
	Cache<Key_T, Mapped_T>* temp = cache;
	while(temp->prev->prev != NULL) {
		temp = temp->prev;	
	}
	if(temp->next != NULL) {
		temp->next->prev = temp->prev;
		temp->prev->next = temp->next;
		delete(temp);
	}
	else {
		cache = temp->prev;
		delete(temp);
		cache->next = NULL;
	}
	cacheCount--;
}

/* Loop through Cache */
template <class Key_T, class Mapped_T> 
Node<Key_T, Mapped_T>* Map<Key_T, Mapped_T> :: retrieveCache(Key_T key) const {
	if(cache == NULL) {
		 return NULL;
	} 
	Cache<Key_T, Mapped_T>* temp = cache;	
	while(temp->prev != NULL) {
		if(temp->node->p.first == key) {
			return temp->node;
		}
		temp = temp->prev;
	}
	return NULL;	
}

/* Remove element from cache */
template <class Key_T, class Mapped_T> 
void Map<Key_T, Mapped_T> :: removeCache(Key_T key) {
	Cache<Key_T, Mapped_T>* temp = cache;	
	while(temp->prev != NULL) {
		if(temp->node->p.first == key) {
			if(temp->next != NULL) {
				temp->next->prev = temp->prev;
				temp->prev->next = temp->next;
				delete(temp);
				cacheCount--;
			}
			else {
				cache = temp->prev;
				delete(temp);
				cache->next = NULL;
			}
		}
		temp = temp->prev;
	}
}

/* Find first node in skiplist */
template <class Key_T, class Mapped_T> 
Node<Key_T, Mapped_T>* Map<Key_T, Mapped_T> :: findFirstNode() const {
	Node<Key_T, Mapped_T>* tempHead = skiplist.head;
	if(tempHead == NULL) {
		return NULL;
	}
	for( ; tempHead->down != NULL; tempHead = tempHead->down);
	return tempHead->next;
}

/* Find last element in skiplist */
template <class Key_T, class Mapped_T> 
Node<Key_T, Mapped_T>* Map<Key_T, Mapped_T> :: findLastNode() const {
	Node<Key_T, Mapped_T>* tempTail = skiplist.tail;
	if(tempTail == NULL) {
		return NULL;
	}
	for( ; tempTail->down != NULL; tempTail = tempTail->down);
	return tempTail;
}

/* Copy Constructor */
template <class Key_T, class Mapped_T> 
Map<Key_T, Mapped_T> :: Map(const Map<Key_T, Mapped_T> &obj) {
	*this = obj;
} 

/* Assignment operator */
template <class Key_T, class Mapped_T> 
Map<Key_T, Mapped_T>& Map<Key_T, Mapped_T> :: operator=(const Map<Key_T, Mapped_T>& obj) {
	Node<Key_T, Mapped_T>* tempHead = obj.skiplist.head;
	Node<Key_T, Mapped_T>* tempTail = obj.skiplist.tail;
	while(tempHead->down != NULL) {
		tempHead = tempHead->down;
		tempTail = tempTail->down;
	}
	while(tempHead->next != tempTail) {
		skiplist.insertPair(tempHead->next->p);
		tempHead = tempHead->next;
	}
	return *this;
}

/* Constructor accepting initializer list*/
template <class Key_T, class Mapped_T> 
Map<Key_T, Mapped_T> :: Map(std::initializer_list<std::pair<const Key_T, Mapped_T>> obj) {
	for (auto li : obj) {
		skiplist.insertPair(li);
	}
}

/* Returns value */
template <class Key_T, class Mapped_T>
Mapped_T & Map<Key_T, Mapped_T> :: operator[](const Key_T & key) {
	Node<Key_T, Mapped_T>* temp = retrieveCache(key); // look up in cache
	if(temp == NULL) {
		temp = skiplist.searchKey(key);
	}  
	if(temp != NULL && temp->next != NULL) {
		insertCache(temp); // insert in cache
		return temp->p.second;
	}
	auto p = std::make_pair(key, NULL);
	temp = skiplist.insertPair(p);
	return temp->p.second;
}  

/* Insert function */
template <class Key_T, class Mapped_T> 
std::pair<typename Map<Key_T, Mapped_T> :: Iterator, bool> Map<Key_T, Mapped_T> :: insert(const ValueType & p) {
	Iterator it;
	std::pair<Iterator, bool> result;
	Node<Key_T, Mapped_T>* temp = retrieveCache(p.first); // look up in cache
	if(temp == NULL) {
		temp = skiplist.searchKey(p.first);
	} 
	if(temp == NULL || (temp!= NULL && temp->next == NULL)) {
		it.current = skiplist.insertPair(p);
		result = std::make_pair(it, true);
		return result;
	}
	insertCache(temp); // Insert in cache
	it.current = temp;
	result = std::make_pair(it, false);
	return result;
}

/* Clear all nodes in skiplist */
template <class Key_T, class Mapped_T>
void Map<Key_T, Mapped_T> :: clear() {
	if(skiplist.size == DEFAULT_HEIGHT) return; // Call free only once

	Node<Key_T, Mapped_T>* tempHead = skiplist.head;
	while(tempHead != NULL) {
		Node<Key_T, Mapped_T>* temp = tempHead;
		tempHead = tempHead->down;
		while(temp != NULL) {
			Node<Key_T, Mapped_T>* tempNext = temp->next;
			delete(temp);
			temp = tempNext;	
		}
	}
	Cache<Key_T, Mapped_T>* tempCache = cache;
	while(tempCache != NULL) {
		Cache<Key_T, Mapped_T>* tempC = tempCache->prev;
		delete(tempCache);
		tempCache = tempC;
		cacheCount--;	
	}
	
	skiplist.size = DEFAULT_HEIGHT;
	skiplist.height = DEFAULT_HEIGHT;
}

/* ------------------------ Operator Overloading (Friend function)---------------------------  */

template <class Key_T, class Mapped_T> 
bool operator==(const Map<Key_T, Mapped_T> & m1, const Map<Key_T, Mapped_T> & m2) { 
	if(m1.skiplist.size != m2.skiplist.size) return false;	
	int count = 0;
	Node<Key_T, Mapped_T>* m1_tempHead = m1.skiplist.head;
	Node<Key_T, Mapped_T>* m2_tempHead = m2.skiplist.head;	
	
	for( ; m1_tempHead->down != NULL; m1_tempHead = m1_tempHead->down);
	for( ; m2_tempHead->down != NULL; m2_tempHead = m2_tempHead->down);

	while(m1_tempHead->next != NULL && m1_tempHead->next->p == m2_tempHead->next->p) {
		count++;
		m1_tempHead =  m1_tempHead -> next;
		m2_tempHead =  m2_tempHead -> next;
	}
	return count == m1.skiplist.size;
}

template <class Key_T, class Mapped_T> 
bool operator!=(const Map<Key_T, Mapped_T> & m1, const Map<Key_T, Mapped_T> & m2) {
	return !(m1 == m2);
}

template <class Key_T, class Mapped_T> 
bool operator<(const Map<Key_T, Mapped_T> & m1, const Map<Key_T, Mapped_T> & m2) {
	if(m1.size < m2.size)  return true;

	Node<Key_T, Mapped_T>* m1_tempHead = m1.skiplist.head;
	Node<Key_T, Mapped_T>* m2_tempHead = m2.skiplist.head;		
	for( ; m1_tempHead->down != NULL; m1_tempHead = m1_tempHead->down);
	for( ; m2_tempHead->down != NULL; m2_tempHead = m2_tempHead->down);
	
	while(m1_tempHead->next != NULL) {
		if(m1_tempHead->next->p < m2_tempHead->next->p) {
			return true;
		}
		m1_tempHead =  m1_tempHead -> next;
		m2_tempHead =  m2_tempHead -> next;
	}	
	return false;
}	

}
#endif
