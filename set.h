#pragma once
/*
* 2023_05_22_csw_NOTE
* 2023_05_22_csw_NOTE ���������֪��Ϊʲôiterator��Ҫ�����������������������ø��ˣ�ֱ�ӱ��ݿ��ڣ�
* 2023_05_23_csw_TODO ���ϸ��Ʋ���û�����
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
