#pragma once
/*
* 2023_05_04_csw_
* ʹ����cpp�ṩ��new��delete��װ�õ��ķ�����
* �����ж����޸�
* 
*/

#include <cstdlib>

namespace MySTL {
	class Simple_Allocator {
	public: 
		// 2023_05_04_csw_NOTE ���뿪�ٿռ�
		static void* allocate(const size_t);
		// 2023_05_04_csw_NOTE ���տռ�
		static void deallocate(void*, ptrdiff_t); 

	};
}
