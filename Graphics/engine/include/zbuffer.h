#pragma once
#include <limits.h>
#include <vector>
#undef SAFE_BUFFER

class ZBuffer {
	typedef float value_type;
#ifdef SAFE_BUFFER
	std::vector<value_type> buffer;
#else
	value_type *buffer;
#endif

	size_t W, H;
public:
	ZBuffer(size_t width, size_t height): W(width), H(height) {
#ifdef SAFE_BUFFER
		buffer.resize(width * height);
#else
		buffer = new value_type[width * height];
#endif	
		Reset();
	}

	~ZBuffer() {
#ifndef SAFE_BUFFER
		delete[] buffer;
#endif
	}

	value_type& operator()(size_t x, size_t y) {
		return buffer[y * W + x];
	}

	void Reset() {
		value_type minValue = std::numeric_limits<value_type>::lowest();
#ifdef SAFE_BUFFER
		std::fill(buffer.begin(), buffer.end(), minValue);
#else
		std::fill(buffer, buffer + W * H, minValue);
#endif
	}
};
