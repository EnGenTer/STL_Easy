#pragma once
/*
* 2023_05_13_csw_
* 用自己建的vector 作为map的底层结构
* Vector写的太爆炸了，积重难返
*/
#include "simple_allocator.h"
#include "allocator.h"
#include "vector.h"

#ifndef DEQUEH
#define DEQUEH
using namespace MySTL; 

namespace MySTL {
	template <typename T>
	class DequeIterator {
	public:
		typedef T					value_type;
		typedef DequeIterator<value_type> self;
		typedef value_type*			pointer;
		typedef value_type&			reference;
		typedef ptrdiff_t			difference_type;

	private:
		pointer begin, end, cur; 
		difference_type node_idx;

	public: 
		DequeIterator(void) {};
		DequeIterator(const self &InputIt) : 
			begin(InputIt.begin), end(InputIt.end), cur(InputIt.cur), node_idx(InputIt.node_idx) {};
		DequeIterator(size_t NodeIdx, pointer NodeBegin, pointer NodeEnd):
			begin(NodeBegin), end(NodeEnd), cur(NodeBegin), node_idx(NodeIdx) {};

		reference operator*(void) {
			return *cur; 
		}

		pointer operator&(void) {
			return &(*cur); 
		}
	};

	template <typename T, typename alloc = Simple_Allocator>
	class Deque {
	public:
		enum {Min_Node_Size = 512};
		enum { Default_Node_Num = 10 }; 
		typedef T					value_type;
		typedef value_type*			pointer;
		typedef value_type&			reference;
		typedef	Vector<pointer>		Map;
		typedef size_t				diff_t; 
		typedef Allocator<value_type, alloc> Map_Node_Allocator; 
		typedef DequeIterator<value_type>	Iterator; 

	private:
		Iterator begin, end; 

	protected:
		Map map;
		const int Node_Cap = 1 > (Min_Node_Size / sizeof(value_type)) ? 1 : (Min_Node_Size / sizeof(value_type));
		
		pointer allocNewMapNode(void) {
			return Map_Node_Allocator::allocate(Node_Cap); 
		}

	public:
		Deque(void) {
			Map map(nullptr, Default_Node_Num);
			size_t mid_idx = map.capacity() >> 1; 
			pointer mid_node = getNode(mid_idx); 
			begin = Iterator(mid_idx, mid_node, mid_node) ;
		}

		inline pointer getNode(const size_t NodeIdx) {
			if (map[NodeIdx] == nullptr) {
				auto a = allocNewMapNode();
			}
			return map[NodeIdx]; 
		}
	};
}

#endif
