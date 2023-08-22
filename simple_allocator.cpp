#include "simple_allocator.h"

namespace MySTL {
	void* Simple_Allocator::allocate(const size_t n) {
		return malloc(n); 
	}

	void Simple_Allocator::deallocate(void* p, ptrdiff_t n) {
		if (p == nullptr) return;
		free(p); 
		return; 
	}
}
