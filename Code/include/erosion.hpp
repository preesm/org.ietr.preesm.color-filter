#ifndef EROSION_HPP
#define EROSION_HPP

#include "hls_math.h"

#include "filter.hpp"

template<typename T> T erosion_f(T data[3][3]) {
	// TODO: Manual distribution of min operation, need to specify distributivity to compiler.
	T res[3];
	for (int i = 0; i < 3; i++) {
		res[i] = data[i][0];
		for (int j = 1; j < 3; j++) {
			res[i] = hls::min(res[i], data[i][j]);
		}
		if (i > 0) {
			res[i] = hls::min(res[i], res[i - 1]);
		}
	}
	return res[2];
}

template<int ROWS, int COLS, typename T>
void erosion(hls::stream<T> &src, hls::stream<T> &snk) {
	filter_border3x3<ROWS, COLS>(src, snk, *erosion_f<T>);
}

#endif //EROSION_HPP
