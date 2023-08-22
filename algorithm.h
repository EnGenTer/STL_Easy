#pragma once
/*
* 2023_05_17_csw_
* 2023_05_17_csw_TODO ���ڻ�����д����ȽϺ���
* 2023_05_17_csw_TODO ��������ʹ��vector����push_back���û��Լ�����backHeapAdjust����heap
* 2023_05_17_csw_TODO ����ȡ�����Ԫ�أ�Ҳ���û��ȵ���heap_pop���ڵ���vector::push_back
* 2023_05_25_csw_TODO û�е���PODtype
*/

#include "vector.h"
#include "iterator.h"

namespace MySTL {
	// 2023_05_17_csw_TODO ����ط�����ԭ���㷨�������ܲ�����ָ��ĸ�
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

	// 2023_05_17_csw_NOTE ע�������back������end
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
		// 2023_05_17_csw_NOTE ע�������back������end
		for (Iterator backit = Begin, tempit; backit != End; ++backit) {
			tempit = backit;
			backHeapAdjust(Begin, tempit); 
		}
		return; 
	}

	// 2023_05_17_csw_NOTE ������Ϊ��ͳһ���ƺͲ���
	template <typename Iterator>
	bool heapPush(Iterator Begin, Iterator End) {
		backHeapAdjust(Begin, End - 1); 
		return true; 
	}

	// 2023_05_17_csw_TODO Ӧ�ü���ȽϺ�����
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

	// 2023_05_24_csw_NOTE �ܺ����ĵط����ڣ���Ҫ�������Լ����ٿռ䡣copy�����𹹽�ԭ������ֻ�����������
	template <typename ForwardIterator, typename OutputIterator>
	OutputIterator copy(ForwardIterator begin, ForwardIterator end, OutputIterator result) {
		return _copy(begin, end, result, Iterator_Traits<ForwardIterator>::iterator_category); 
	}

	// 2023_05_25_csw_TODO û�д���POD����
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