#pragma once
#include <new>
#include <cstdio> // for printf()
#include <cstdlib>
#ifdef _MSC_VER
#include <malloc.h>
#endif // _MSC_VER

class TrackNew
{
private:
	static inline int numMalloc = 0;
	static inline size_t sumSize = 0;
	static inline bool doTrace = false;
	static inline bool inNew = false;
public:
	static void reset()
	{
		numMalloc = 0;
		sumSize = 0;
	}

	static void trace(bool b)
	{
		doTrace = b;
	}

	static void* allocate(std::size_t size, std::size_t align, const char* call)
	{
		++numMalloc;
		sumSize += size;
		void* p;
		if (align == 0)
		{
			p = std::malloc(size);
		}
		else
		{
#ifdef _MSC_VER
			p = _aligned_malloc(size, align);
#else
			p = std::aligned_alloc(align, size);
#endif // _MSC_VER

		}
		if (doTrace)
		{
			// DON’T use std::cout here because it might allocate memory
			// while we are allocating memory (core dump at best)
			printf("#%d %s ", numMalloc, call);
			printf("(%zu bytes, ", size);
			if (align > 0)
				printf("%zu-bytes aligned) ", align);
			else
				printf("default-aligned) ");
			printf("=> %p (total: %zu Bytes)\n", (void*)p, sumSize);
		}
		return p;
	}

	static void status()
	{
		printf("%d allocations for %zu bytes\n", numMalloc, sumSize);
	}
};

[[nodiscard]]
void* operator new(std::size_t size)
{
	return TrackNew::allocate(size, 0, "::new");
}

[[nodiscard]]
void* operator new(std::size_t size, std::align_val_t align)
{
	return TrackNew::allocate(size, static_cast<std::size_t>(align), "::new aligned");
}

[[nodiscard]]
void* operator new[](std::size_t size)
{
	return TrackNew::allocate(size, 0, "::new[]");
}

[[nodiscard]]
void* operator new[](std::size_t size, std::align_val_t align)
{
	return TrackNew::allocate(size, static_cast<std::size_t>(align), "::new[] aligned");
}

// ensure deallocations match:
void operator delete (void* p) noexcept
{
	std::free(p);
}

void operator delete (void* p, std::size_t sz) noexcept
{
	::operator delete(p);
}

void operator delete (void* p, std::align_val_t al) noexcept 
{
#ifdef _MSC_VER
	_aligned_free(p); // Windows API
#else
	std::free(p); // C++17 API
#endif
}

void operator delete (void* p, std::size_t, std::align_val_t align) noexcept 
{
	::operator delete(p, align);
}