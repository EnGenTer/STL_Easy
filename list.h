#pragma once
#include "simple_allocator.h"
#include "allocator.h"
#include "iterator.h"

namespace MySTL {
	template <typename T>
	class List_Node {
	public:
		typedef T				value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef List_Node* void_pointer;

		T data; 
		void_pointer pre; 
		void_pointer next; 
	};
	
	template <typename T> 
	class List_Iterator {
	public:
		typedef forward_iterator_tag	iterator_category;
		typedef T					value_type; 
		typedef T*					pointer;
		typedef T&					reference;
		typedef List_Iterator<value_type>	self; 
		typedef List_Node<value_type>*	Node_Pointer;
		typedef ptrdiff_t			difference_type;

		Node_Pointer node; 
		List_Iterator(Node_Pointer p) {
			node = p; 
		}
		List_Iterator(const self &it) {
			node = it.node;
		} // 2023_05_10_csw_NOTE 拷贝构造函数的参数必须是引用形式
		~List_Iterator(void) {}

		self& operator++() { 
			node = node->next;
			return *this; 
		}
		self operator++(int) { 
			self tempnode = *this; 
			++*this; 
			return tempnode;
		}
		self& operator--() {
			node = node->pre;
			return *this;
		}
		self operator--(int) {
			self tempnode = *this;
			--*this;
			return tempnode;
		}
		self operator+(int count) {
			self tempp = *this;
			for (; count > 0; --count) {
				tempp.node = tempp.node->next;
			}
			return tempp;
		}
		self operator-(int count) {
			self tempp = *this;
			for (; count > 0; --count) {
				tempp.node = tempp.node->pre;
			}
			return tempp;
		}
		value_type operator*(void) { return node->data; }
		bool operator==(self it) { return node == it.node; }
		bool operator!=(self it) { return node != it.node; }
		Node_Pointer next(void) { return node->next; }
		Node_Pointer pre(void) { return node->pre; }
	};

	template <typename T, typename alloc = Simple_Allocator>
	class List {
	public:
		typedef T					value_type; 
		typedef value_type*			pointer;
		typedef value_type&			reference;
		typedef List_Node<value_type>	Node;
		typedef	Allocator<Node, alloc>	List_Allocator;
		typedef List_Iterator<value_type>	Iterator; 
		typedef size_t				diff_t;
		typedef List_Node<value_type>*	Node_Pointer;
		
		Node_Pointer DummyHead; 

		List(void) {
			DummyHead = List_Allocator::allocate(); 
			DummyHead->next = DummyHead; 
			DummyHead->pre = DummyHead;
		}

		~List() {
			Node_Pointer CurP; 
			for (Node_Pointer P = DummyHead->next; P != DummyHead; ) {
				CurP = P;
				P = P->next; 
				List_Allocator::deallocate(CurP); 
			}
			List_Allocator::deallocate(DummyHead);
		}

		void clear(void) {
			this->~List();
			DummyHead = List_Allocator::allocate();
			DummyHead->next = DummyHead;
			DummyHead->pre = DummyHead;
		}

		Iterator begin(void) {
			return Iterator(DummyHead->next);
		}
		Iterator end(void) {
			return Iterator(DummyHead);
		}

		void push_back(value_type V) {
			Node_Pointer NewNode = List_Allocator::allocate(); 
			NewNode->data = V; 
			NewNode->next = DummyHead;
			NewNode->pre = DummyHead->pre;
			DummyHead->pre->next = NewNode; 
			DummyHead->pre = NewNode; 
			return;
		}

		value_type pop_back(void) {
			Node_Pointer PopNode = DummyHead->pre;
			PopNode->pre->next = DummyHead;
			DummyHead->pre = PopNode->pre;
			value_type Value = PopNode->data; 
			List_Allocator::deallocate(PopNode); 
			return Value; 
		}

		diff_t size(void) {
			return iteratorDistance(begin(), end(), Iterator_Traits<Iterator>::iterator_category()); 
		}

		bool insideCheck(Iterator BeginIt, Iterator EndIt, Iterator NewIt) {
			Iterator CheckIterator(BeginIt); 
			for (; CheckIterator != EndIt; ++CheckIterator) {
				if (CheckIterator == NewIt) return true; 
			}
			return false; 
		}

		bool transfer(Iterator BeginIt, Iterator EndIt, Iterator NewIt) {
			if (insideCheck(BeginIt, EndIt, NewIt) == true)
				return false; 
			Iterator TailIt(EndIt - 1); 
			BeginIt.node->pre->next = EndIt.node;
			EndIt.node->pre = BeginIt.node->pre;

			NewIt.node->pre->next = BeginIt.node;
			BeginIt.node->pre = NewIt.node->pre;

			NewIt.node->pre = TailIt.node;
			TailIt.node->next = NewIt.node;
			return true; 
		}

		bool empty(void) {
			return begin() == end(); 
		}

		bool splice(List &InsertList, Iterator NewIt) {
			// if (insideCheck(InsertList.begin(), InsertList.end(), NewIt) == true) return false;
			// else if (InsertList.empty() == true) return true; 
			if (InsertList.empty() == true) return true;
			return transfer(InsertList.begin(), InsertList.end(), NewIt);
		}
	};
}