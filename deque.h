#pragma once
/*
* 2023_05_13_csw_
* 2023_05_16_csw_TODO ȱ��deque���γ�ʼ������ fill_initialize()
* 2023_05_16_csw_TODO ��Ԫ�ط���buffer��ʹ�õ��Ǹ�ֵ��䡣����Ҫ�ĳ�placement new�ķ�����������
* 2023_05_16_csw_TODO ��deque��Ƶ�������в���ʱ������ʹ���ƶ�����realloc
*/

#include "simple_allocator.h"
#include "allocator.h"
#include "iterator.h"

#ifndef CMATH
#define CMATH
#include <cmath>
#endif // !CMATH


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

	// 2023_05_15_csw_TODO ���ø�
	// private: 
		pointer* Node; 
		pointer First; 
		pointer Last; 
		pointer Cur; 

	public:
		DequeIterator(void) {
			; 
		}

		DequeIterator(pointer* Node, pointer Begin, pointer End, pointer Cur) :
			Node(Node), First(Begin), Last(End), Cur(Cur) {
			;
		}

		DequeIterator(const self &CopyIt) :
			Node(CopyIt.Node), First(CopyIt.First), Last(CopyIt.Last), Cur(CopyIt.Cur) {
			;
		}

		value_type operator*(void) {
			return *Cur; 
		}

		reference operator->(void) {
			return &(*Cur); 
		}

		self& operator--(void) {
			if (First == Cur) {
				Node = Node - 1;
				Last = *Node + (Last - First);
				First = *Node;
				Cur = Last;
			}
			Cur--;
			return *this; 
		}

		self operator--(int) {
			self TempIt(*this);
			return --TempIt;
		}

		self& operator++(void) {
			Cur += 1;
			if (Last == Cur) {
				Node = Node + 1;
				Last = *Node + (Last - First);
				First = *Node;
				Cur = First;
			}
			return *this; 
		}

		self operator++(int) {
			self TempIt(*this);
			return ++TempIt;
		}

		self operator+(difference_type Idx) {
			self TempIt(*this); 
			// 2023_05_16_csw_NOTE �ȱ�cpp�ĳ�������0ȡ����py������ȡ����floor��Ī������û����
			Idx += Cur - First; 
			difference_type quo = Idx / (Last - First), rem = Idx % (Last - First); 
			if (rem < 0)
				quo -= 1, rem += (Last - First); 
			// difference_type ShiftingIdx = std::floor(Idx / (Last - First));
			TempIt.Node += quo;
			TempIt.Last = *TempIt.Node + (TempIt.Last - TempIt.First);
			TempIt.First = *TempIt.Node;
			TempIt.Cur = TempIt.First + rem;
			return TempIt;
		}

		self operator-(difference_type Idx) {
			return *this + (-Idx); 
		}

		bool operator==(self It) {
			return It.Cur == Cur; 
		}

		bool operator!=(self It) {
			return !(*this == It); 
		}

		value_type operator=(value_type V) {
			*Cur = V; 
			return *Cur;
		}

		self operator=(self It) {
			First = It.First;
			Last = It.Last;
			Cur = It.Cur;
			Node = It.Node;
			return *this; 
		}
	};

	template <typename T, typename alloc = Simple_Allocator, size_t BuffSize = 512>
	class Deque {
	public:
		enum { _Min_BuffSize = BuffSize };
		enum { _Default_Node_Num = 5 };
		enum { _Enlarge_Times = 2 };
		typedef random_access_iterator_tag iterator_category;
		typedef ptrdiff_t			difference_type;
		typedef T					value_type;
		typedef value_type*			pointer;
		typedef value_type&			reference;
		typedef	pointer*			Map;
		typedef DequeIterator<value_type>	Iterator; 
		typedef Allocator<value_type, alloc> Map_Node_Allocator;
		typedef Allocator<pointer, alloc> Map_Allocator;
		enum { NodeNum = (_Min_BuffSize / sizeof(value_type)) + 1 }; // 2023_05_15_csw_TODO ���ø��ˣ�д��ʵ����ֵ + 1�������Ԫ��

	private:
		Map map = nullptr; 
		Iterator start, finish; 
		size_t BufferNum = _Default_Node_Num; 

	public:
		Deque(void) {
			map = Map_Allocator::allocate(BufferNum);
			for (unsigned int i = 0; i < BufferNum; ++i)
				*(map + i) = nullptr; 
			size_t MidIdx = BufferNum >> 1;
			pointer MidPtr = getNode();
			*(map + MidIdx) = MidPtr;
			start = Iterator((map + MidIdx), MidPtr, MidPtr + NodeNum, MidPtr);
			finish = Iterator((map + MidIdx), MidPtr, MidPtr + NodeNum, MidPtr);
		}

		pointer getNode(size_t n = NodeNum) {
			pointer ptr = Map_Node_Allocator::allocate(n); 
			return ptr; 
		}

		Iterator begin(void) { return start; }
		Iterator end(void) { return end; }

		// 2023_05_15_csw_NOTE map�������¿��ٺ͸���buffer
		void mapRealloc(void) {
			Map NewMap = Map_Allocator::allocate(BufferNum * _Enlarge_Times); 
			for (unsigned int i = 0; i < (BufferNum * _Enlarge_Times); ++i)
				*(NewMap + i) = nullptr;
			size_t NewStartIdx = (BufferNum * (_Enlarge_Times - 1)) >> 1;
			for (size_t Idx = 0; Idx < BufferNum; ++Idx) {
				// Map CurNode = (NewMap + NewStartIdx + Idx);
				*(NewMap + NewStartIdx + Idx) = *(map + Idx);
				*(map + Idx) = nullptr; 
			}
			start.Node = NewMap + (start.Node - map + NewStartIdx);
			finish.Node = NewMap + (finish.Node - map + NewStartIdx);
			Map_Allocator::deallocate(map, BufferNum);
			map = NewMap;
			BufferNum *= _Enlarge_Times; 
		}

		// 2023_05_15_csw_NOTE mapβ������buffer
		void NodeAllocPushBack(void) {
			Map NextNode = (finish.Node + 1); 
			if (*NextNode != nullptr) Map_Node_Allocator::deallocate(*NextNode, NodeNum);
			*NextNode = Map_Node_Allocator::allocate(NodeNum); 
		}

		bool push_back(value_type Value) {
			// 2023_05_15_csw_NOTE ��ǰ����������
			// 2023_05_15_csw_NOTE ��ΪԪ������+1
			*finish.Cur = Value;
			if (finish.Cur == finish.Last - 1) {
				// 2023_05_15_csw_NOTE ��ǰmap����
				if (finish.Node - map == BufferNum - 1) {
					mapRealloc();
				}
				// 2023_05_15_csw_NOTE β������buffer
				NodeAllocPushBack();
				/*
				finish.Node += 1; 
				finish.Last = *finish.Node + (finish.Last - finish.First); 
				finish.First = *finish.Node; 
				finish.Cur = *finish.Node; 
				*/
			}
			++finish;
			return true;
		}

		bool pop_back(void) {
			if (start == finish) return false;
			--finish;
			return true;
		}

		// 2023_05_15_csw_NOTE mapβ������buffer
		void NodeAllocPushFront(void) {
			Map PreNode = (start.Node - 1);
			if (*PreNode != nullptr) Map_Node_Allocator::deallocate(*PreNode, NodeNum);
			*PreNode = Map_Node_Allocator::allocate(NodeNum);
		}

		// 2023_05_16_csw_TODO û���޸ģ�������͵���ˣ�������push_back()���
		bool push_front(value_type Value) {
			// 2023_05_15_csw_NOTE ��ǰ����������
			if (start.Cur == start.Last) {
				// 2023_05_15_csw_NOTE ��ǰmap����
				if (start.Node == map) {
					mapRealloc();
				}
				// 2023_05_15_csw_NOTE β������buffer
				NodeAllocPushFront();
			}
			--start;
			*start.Cur = Value;
			return true;
		}

		bool pop_front(void) {
			if (start == finish) return false;
			++start;
			return true;
		}

		value_type operator[](size_t Idx) {
			return *(start + Idx); 
		}

		difference_type size(void) const {
			difference_type finish_difference_type, start_difference_type;
			finish_difference_type = (finish.Node - map) * NodeNum + finish.Cur - finish.First;
			start_difference_type = (start.Node - map) * NodeNum + start.Cur - start.First;
			return finish_difference_type - start_difference_type;
		}

		difference_type loc(Iterator It) const {
			difference_type It_difference_type, start_difference_type;
			It_difference_type = (It.Node - map) * NodeNum + It.Cur - It.First;
			start_difference_type = (start.Node - map) * NodeNum + start.Cur - start.First;
			return It_difference_type - start_difference_type;
		}

		// 2023_05_16_csw_TODO ��Ϊֻʵ����pushback������û���жϺ�ʹ��ǰ����뷽��
		bool insert(Iterator InsertIt, value_type Value) {
			push_back(Value);
			for (Iterator BackIt(finish - 1); BackIt != InsertIt; --BackIt) {
				BackIt = *(BackIt - 1); 
			}
			InsertIt = Value; 
			return true; 
		}

		// 2023_05_16_csw_TODO ��Ϊֻʵ����pushback������û���жϺ�ʹ��ǰ��ɾ������
		bool erase(Iterator EraseIt) {
			for (Iterator FrontIt(EraseIt); FrontIt != finish - 1; ++FrontIt) {
				FrontIt = *(FrontIt + 1); 
			}
			if (finish.Cur == finish.First) {
				Map_Node_Allocator::deallocate(*finish.Node); 
			}
			--finish; 
			return true; 
		}
	};
}

#endif // !DEQUEH
