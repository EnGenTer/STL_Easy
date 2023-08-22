#pragma once
/*
2023_05_05_csw_
������
*/

#ifndef CSTDDEF
#define CSTDDEF
#include <cstddef>
#endif // !CSTDDEF


#ifndef ITERATOR
#define ITERATOR

namespace MySTL {
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	// 2023_05_05_csw_NOTE �ƺ���״���ƶ����鷳 ����û����iterator��ʵ���ƶ��Ȳ���
	template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
	class Iterator {
	public:
		typedef Category	iterator_category;
		typedef T			value_type;
		typedef Distance	difference_type;
		typedef Pointer		pointer;
		typedef Reference	reference;
	};

	// 2023_05_05_csw_NOTE ʹ��traits ��װ��������ԭ��ָ�� ����ͳһ��������
	template <class I>
	class Iterator_Traits {
	public:
		typedef		typename I::iterator_category		iterator_category;
		typedef		typename I::value_type				value_type;
		typedef		typename I::difference_type			difference_type;
		typedef		typename I::pointer					pointer;
		typedef		typename I::reference				reference;
	};

	template <class P>
	class Iterator_Traits<P*> {
	public:
		typedef		random_access_iterator_tag		iterator_category; // 2023_05_05_csw_QUES ��ȷ��
		typedef		P						value_type;
		typedef		ptrdiff_t				difference_type;
		typedef		P* pointer;
		typedef		P& reference;
	};

	template <class CP>
	class Iterator_Traits<const CP*> {
	public:
		typedef		random_access_iterator_tag		iterator_category; // 2023_05_05_csw_QUES ��ȷ��
		typedef		CP						value_type;
		typedef		ptrdiff_t				difference_type;
		typedef		const CP* pointer;
		typedef		const CP& reference;
	};

	// 2023_05_05_csw_NOTE ��Ȼд����iter ���ǿ��ܻᴫ��ԭ��ָ�� ������Ҫʹ��traits��װһ��
	template <typename iter>
	inline typename Iterator_Traits<iter>::difference_type iteratorDistance(iter it1, iter it2, forward_iterator_tag) {
		typename Iterator_Traits<iter>::difference_type counter = 0;
		while (it2 != it1) {
			++counter;
			++it1;
		}
		return counter;
	}

	template <typename iter>
	inline typename Iterator_Traits<iter>::difference_type iteratorDistance(iter it1, iter it2, bidirectional_iterator_tag) {
		typename Iterator_Traits<iter>::difference_type counter = 0;
		while (it2 != it1) {
			++counter;
			++it1;
		}
		return counter;
	}

	template <typename iter>
	inline typename Iterator_Traits<iter>::difference_type iteratorDistance(iter it1, iter it2, random_access_iterator_tag) {
		return it2 - it1;
	}

	template <typename iter>
	inline void iteratorAdvance(iter& it, typename Iterator_Traits<iter>::difference_type n, forward_iterator_tag) {
		while (n > 0) {
			++it;
			--n;
		}
	}

	template <typename iter>
	inline void iteratorAdvance(iter& it, typename Iterator_Traits<iter>::difference_type n, bidirectional_iterator_tag) {
		if (n > 0) {
			while (n > 0) {
				++it;
				--n;
			}
		}
		else
		{
			while (n < 0) {
				--it;
				++n;
			}
		}
	}

	template <typename iter>
	inline void iteratorAdvance(iter& it, typename Iterator_Traits<iter>::difference_type n, random_access_iterator_tag) {
		it += n;
	}

	template <typename T1, typename T2>
	inline void construct(T1* AllocatorPointer, const T2& Value) {
		new (AllocatorPointer) T1(Value);
	}

	// 2023_05_06_csw_NOTE ע�������pointer
	template <typename T>
	inline void destory(T p) {
		p -> ~T();
	}

	// 2023_05_06_csw_NOTE ע�������pointer
	template <typename T>
	inline void destory(T p, size_t n) {
		free(p);
	}

	template <typename T1, typename T2>
	void replace_copy(T1 Begin, T1 End, T2 ReBegin) {// 2023_05_06_csw_TODO ��������֪��дʲô
		for (; Begin != End; ++Begin, ++ReBegin) {
			*ReBegin = *Begin; 
		}
	}
}

#endif // !ITERATOR