#pragma once
/*
* 2023_05_05_csw_
* 暂时的弄一个list方便实验
*/

#include "allocator.h"
#include "simple_allocator.h"
#include "iterator.h"

using namespace MySTL; 

namespace MySTL {
	template <typename T>
	class List_Iterator : public Iterator<forward_iterator_tag, T>
	{
	public:
		typedef			forward_iterator_tag				iterator_category;
		typedef			T									value_type;
		typedef			T* pointer;
		typedef			T& reference;
		typedef			ptrdiff_t							difference_type;
		typedef			List_Iterator						self;

		pointer curnode;
		List_Iterator() { curnode = nullptr; }
		List_Iterator(pointer ip) { curnode = ip; }
		List_Iterator(reference ip) { curnode = ip; }

		inline reference operator&() { return &curnode->data; }
		inline value_type operator*() { return *curnode->data; }
		inline reference operator->() { return &curnode->data; }
		inline bool operator==(const List_Iterator x) { return &x == &curnode; }
		inline bool operator!=(const List_Iterator x) { return &x != &curnode; }

		//迭代器向前移动一个位置(i++)
		self operator++(int) {
			self tmp = *this;
			++* this;
			return tmp;
		}

		//迭代器向前移动一个位置(++i)
		self& operator++() {
			curnode = curnode->next;
			return *this;
		}

		//对迭代器递减1 (--i)
		self& operator--() {
			curnode = curnode->prev;
			return *this;
		}

		//对迭代器递减1 (i--)
		self operator--(int) {
			self tmp = *this;
			--* this;
			return tmp;
		}
	};

	template <class T, class alloc = Simple_Allocator>
	class List
	{
	public:
		typedef		T							value_type;
		typedef		value_type*					pointer;
		typedef		value_type&					reference;
		typedef		size_t						size_type;
		typedef		ptrdiff_t					difference_type;
		typedef		Allocator<List, Simple_Allocator>	allocator;
		typedef		List_Iterator<List>			iterator;

		List(void) : pre(this), next(this) {}
		List(T Value) : pre(this), next(this), data(Value) {}
		// 2023_05_08_csw_QEUS 这个地方需要写成给整个链表的析构？
		~List(void) { ; }

		inline pointer front() { return next; }
		inline pointer back() { return this; }

		void push_back(T Value) {
			List* Node = allocator::allocate();
			Node->pre = pre; 
			Node->next = this;
			pre->next = Node; 
			this->pre = Node; 
		}

		void pop_back() {
			pointer BackNode = back()->pre;
			allocator::deallocate(BackNode->next);
			pre = BackNode;
			BackNode->next = this;
		}

		size_t size() {
			return iteratorDistance(next, this, Iterator_Traits<iterator>::iterator_category());
		}

		void transfer(difference_type begin_idx1, difference_type end_idx1, difference_type begin_idx2) {
			iterator(begin_it);
		}

	private:
		T data;
		List* next = nullptr;
		List* pre = nullptr;
	};
};