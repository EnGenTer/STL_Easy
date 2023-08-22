/*
* 2023_05_06_csw_
* 2023_05_08_csw_NOTE
* 1、vector构造的时候没有调用拷贝构造函数和定位new 参考construct
* 参考uninitialized_copy等，将空间分配和构造函数分离
* 2、同时删除元素时也没有使用T的析构函数 有点只管杀不管埋的意思
* 缺少使用is_POD_type判断是否需要调用析构函数
*/

#include "simple_allocator.h"
#include "allocator.h"
#include "iterator.h"

#ifndef VECTORH
#define VECTORH

namespace MySTL {
	template <typename T, typename alloc = Simple_Allocator>
	class Vector
	{
	public:
		typedef	T					value_type; 
		typedef	value_type*			pointer;
		typedef value_type&			reference;
		typedef value_type*			iterator;
		typedef size_t				diff_t; 
		typedef Allocator<T, alloc>	allocator; 

		enum {CAPACITY_TIMES = 2};

		Vector(void) : begin_it(nullptr), end_it(nullptr), cap_it(nullptr) {};
		Vector(value_type DefaultValue, size_t Size) {
			begin_it = allocator::allocate(Size);
			end_it = begin_it + Size; 
			cap_it = end_it; 
			for (iterator cur_it = begin_it; cur_it != end_it; ++cur_it) *cur_it = DefaultValue; 
		}
		~Vector(void) {
			allocator::deallocate(begin_it, cap_it - begin_it);
		};

		iterator begin(void) {
			return begin_it; 
		}

		iterator end(void) {
			return end_it; 
		}

		size_t size(void) {
			return end_it - begin_it; 
		}

		size_t capacity(void) {
			return cap_it - begin_it; 
		}

		bool insert(T Value, size_t Idx) {
			if (end_it == cap_it) {
				replace_alloc_fill(); 
			}
			replace_copy(begin_it + Idx, end_it, begin_it + Idx + 1); 
			++end_it; 
			*(begin_it + Idx) = Value; 
			return true; 
		}

		reference operator[](const size_t Idx) {
			return *(begin_it + Idx); 
		}

		void push_back(T Value) {
			if (end_it == cap_it) {
				replace_alloc_fill();
			}
			*end_it = Value;
			++end_it;
		}

		diff_t new_capacity(iterator t1, iterator t2) {
			return t1 == t2 ? 1 : (end_it - begin_it) * CAPACITY_TIMES; 
		}

		void replace_alloc_fill() {
			diff_t new_cap = new_capacity(begin_it, end_it);
			iterator new_begin_it = allocator::allocate(new_cap);
			replace_copy(begin_it, end_it, new_begin_it);
			end_it = new_begin_it + (end_it - begin_it);
			allocator::deallocate(begin_it, cap_it - begin_it);
			begin_it = new_begin_it; 
			cap_it = begin_it + new_cap;
		}

		reference pop_back(void) {
			if (end_it != begin_it) {
				--end_it; 
				return *end_it; 
			}
			return *end_it; 
		}

		bool erase(const value_type Value) {
			for (iterator CurBegin = begin_it; CurBegin != end_it; ++CurBegin) {
				if (*CurBegin == Value) {
					replace_copy(CurBegin + 1, end_it, CurBegin); 
					return true; 
				}
			}
			return false; 
		}

		bool remove(const diff_t n) {
			if (size() > n) {
				return false; 
			}
			replace_copy(begin_it + n + 1, end_it, begin_it + n);
			return true; 
		}

		void clear(void) {
			end_it = begin_it; 
		}

		bool empty(void) {
			return (begin_it == end_it); 
		}

		T back(void) {
			return *(end_it - 1); 
		}

	private:
		iterator begin_it;
		iterator end_it;
		iterator cap_it;
	};
}

#endif // !VECTORH
