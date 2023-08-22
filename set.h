#pragma once
/*
* 2023_05_22_csw_NOTE
* 2023_05_22_csw_NOTE 看到这里才知道为什么iterator需要接受三个参数。我现在懒得改了，直接表演开摆！
* 2023_05_23_csw_TODO 集合复制操作没有完成
*/

#include "simple_allocator.h"
#include "allocator.h"
#include "rbtree.h"

namespace MySTL {
	template <typename Key, typename Compare = less<Key>, typename Alloc = Simple_Allocator>
	class set {
	public:
		typedef input_iterator_tag iterator_category;
		typedef ptrdiff_t			difference_type;
		typedef Key					key_type;
		typedef Key					value_type;
		typedef RBtree<key_type, key_type, KeyIsValue<key_type>, Compare, Alloc> tree;
		typedef const key_type		const_key_type;
		typedef const value_type	const_value_type;
		typedef RBtree_iterator<key_type> iterator;

		tree rbtree; 

		iterator begin() { return rbtree.begin(); }
		iterator end() { return rbtree.end(); }
		pair<iterator, bool> insert(key_type Val) { return rbtree.uniqueInsert(Val); }
		iterator find(value_type Val) { return rbtree.find(Val); };
		bool erase(const iterator& It) { return rbtree.erase(It); }
		bool erase(const key_type &Val) { return rbtree.erase(Val); }
		const size_t size()const { return rbtree.size(); }
	};
}
