#pragma once
/*
* 2023_05_17_csw_
* 2023_05_17_csw_TODO 现在还不会写传入比较函数
* 2023_05_17_csw_TODO 设想中是使用vector调用push_back后，用户自己调用backHeapAdjust调整heap
* 2023_05_17_csw_TODO 包括取出最大元素，也是用户先调用heap_pop后，在调用vector::push_back
* 2023_05_25_csw_TODO 没有调整PODtype
*/

#include "vector.h"
#include "iterator.h"

namespace MySTL {
	// 2023_05_17_csw_TODO 这个地方不是原地算法，想想能不能用指针改改
	template <typename Tp>
	static void inplaceSwitch(Tp &a, Tp &b) {
		Tp temp = a; 
		a = b; 
		b = temp; 
	}

	template <typename Tp> 
	static bool comp(const Tp &a, const Tp &b) {
		return a > b; 
	}

	template <typename Iterator>
	static Iterator getParent(Iterator Begin, Iterator Cur) {
		typedef	Iterator_Traits<Iterator>::difference_type			difference_type;
		difference_type dis = (Cur - 1 - Begin) >> 1;
		return (Begin + dis);
	}

	// 2023_05_17_csw_NOTE 注意入参是back，不是end
	template <typename Iterator>
	static void backHeapAdjust(Iterator Begin, Iterator Back) {
		Iterator tempit = Back, parentit;
		while (tempit != Begin) {
			parentit = getParent(Begin, tempit);
			if (comp(*parentit, *tempit) == true) {
				return;
			}
			inplaceSwitch(*parentit, *tempit);
			tempit = parentit;
		}
		return;
	}

	template <typename Iterator> 
	void heapify(Iterator Begin, Iterator End) {
		if (End - Begin <= 1) return;
		// 2023_05_17_csw_NOTE 注意入参是back，不是end
		for (Iterator backit = Begin, tempit; backit != End; ++backit) {
			tempit = backit;
			backHeapAdjust(Begin, tempit); 
		}
		return; 
	}

	// 2023_05_17_csw_NOTE 仅仅是为了统一名称和操作
	template <typename Iterator>
	bool heapPush(Iterator Begin, Iterator End) {
		backHeapAdjust(Begin, End - 1); 
		return true; 
	}

	// 2023_05_17_csw_TODO 应该加入比较函数的
	template <typename Tp>
	static Tp nextValue(Tp a, Tp b) {
		if (comp(a, b) == true) {
			return a; 
		}
		else {
			return b; 
		}
	}

	template <typename Iterator>
	void heapAdjust(const Iterator Begin, const Iterator End, Iterator& CurIt = Begin) {
		if (CurIt - Begin >= End - Begin) {
			return;
		}
		typedef	Iterator_Traits<Iterator>::difference_type			difference_type;
		difference_type&& shiftidx = (CurIt - Begin + 1) << 1; 
		Iterator rightit = Begin + shiftidx;
		Iterator leftit = rightit - 1; 
		Iterator *nextitptr; 

		if (leftit - Begin >= End - Begin) return; 
		if (rightit - Begin >= End - Begin) {
			nextitptr = &leftit;
		}
		else{
			if (comp(*leftit, *rightit) == true) {
				nextitptr = &leftit; 
			}
			else {
				nextitptr = &rightit; 
			}
		}

		if (comp(*CurIt, **nextitptr) == true) {
			return;
		}
		else {
			inplaceSwitch(*CurIt, **nextitptr);
			heapAdjust(Begin, End, *nextitptr); 
		}
	}

	template <typename Iterator>
	void frontHeapAdjust(Iterator Begin, Iterator End) {
		if (End - Begin <= 1) return;
		Iterator curit = Begin; 
		heapAdjust(Begin, End, curit);
		return;
	}

	template <typename Iterator>
	bool heapPop(Iterator Begin, Iterator End) {
		if (Begin == End) return false; 
		frontHeapAdjust(Begin, End); 
		return true; 
	}

	template <typename ForwardIterator>
	ForwardIterator find(ForwardIterator begin, ForwardIterator end, 
		const typename Iterator_Traits<ForwardIterator>::value_type& value) {
		for (; begin != end; ++begin) {
			if (*begin == value)
				return begin; 
		}
		return begin; 
	}

	// 2023_05_24_csw_NOTE 很憨批的地方在于，需要现在先自己开辟空间。copy不负责构建原容器，只负责填充数据
	template <typename ForwardIterator, typename OutputIterator>
	OutputIterator copy(ForwardIterator begin, ForwardIterator end, OutputIterator result) {
		return _copy(begin, end, result, Iterator_Traits<ForwardIterator>::iterator_category); 
	}

	// 2023_05_25_csw_TODO 没有处理POD问题
	template <typename ForwardIterator, typename OutputIterator>
	OutputIterator _copy(ForwardIterator begin, ForwardIterator end, OutputIterator result, forward_iterator_tag) {
		while (begin != end) {
			*result = *begin; 
			++begin, ++result; 
		}
		return result; 
	}

	template <typename ForwardIterator> 
	ForwardIterator advance(ForwardIterator it, typename Iterator_Traits<ForwardIterator>::difference_type dis) {
		typedef typename Iterator_Traits<ForwardIterator>::iterator_category iterator_category;
		return _advance(it, dis, iterator_category()); 
	}

	template <typename ForwardIterator>
	ForwardIterator _advance(ForwardIterator it, typename Iterator_Traits<ForwardIterator>::difference_type dis, forward_iterator_tag) {
		for (; dis > 0; --dis) {
			++it;
		}
		return it;
	}

	template <typename ForwardIterator>
	ForwardIterator _advance(ForwardIterator it, typename Iterator_Traits<ForwardIterator>::difference_type dis, random_access_iterator_tag) {
		return it + dis;
	}
}