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

template<typename T> 
struct ErosionFunctor {
    T operator()(T data[3][3]) const {
        #pragma HLS INLINE
	T min_row0 = hls::min(data[0][0], hls::min(data[0][1], data[0][2]));
    T min_row1 = hls::min(data[1][0], hls::min(data[1][1], data[1][2]));
    T min_row2 = hls::min(data[2][0], hls::min(data[2][1], data[2][2]));
    
    return hls::min(min_row0, hls::min(min_row1, min_row2));
    }
};

template<int ROWS, int COLS, typename T>
void erosion(hls::stream<T> &src, hls::stream<T> &snk) {
#pragma HLS INLINE off
	ErosionFunctor<T> op;
    filter_border3x3_static_accesses<ROWS, COLS>(src, snk, op);
}



#endif //EROSION_HPP
