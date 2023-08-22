#pragma once
/*
* 2023_05_04_csw_
* 使用由cpp提供的new和delete封装得到的分配器
* 不进行额外修改
* 
*/

#include <cstdlib>

namespace MySTL {
	class Simple_Allocator {
	public: 
		// 2023_05_04_csw_NOTE 申请开辟空间
		static void* allocate(const size_t);
		// 2023_05_04_csw_NOTE 回收空间
		static void deallocate(void*, ptrdiff_t); 

	};
}
