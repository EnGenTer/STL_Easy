#pragma once
/*
* 2023_05_04_csw_
* ��װallocator
*/

namespace MySTL {
	template <class T, class alloc> // �������ͺ�allocator
	class Allocator {
		typedef	T		_Ty;
		typedef	alloc	Alloc; 

	public:
		static T* allocate(size_t n = 1) {
			return (_Ty*)alloc::allocate(sizeof(_Ty) * n);
		}

		static void deallocate(T* p, size_t n = 1) {
			p -> ~T(); 
			alloc::deallocate(p, sizeof(_Ty) * n);
		}
	};
}
