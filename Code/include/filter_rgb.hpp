#ifndef FILTER_RGB_HPP
#define FILTER_RGB_HPP

#include "hls_stream.h"

template<typename T>
void filter_rgb(hls::stream<T> &src, hls::stream<ap_uint<8>> &condition,
		hls::stream<T> &snk) {
#pragma HLS PIPELINE II=3 style=flp
	auto r = src.read();
	auto g = src.read();
	auto b = src.read();
	if (condition.read() > 0) {
		snk.write(r);
		snk.write(g);
		snk.write(b);
	} else {
		snk.write(0);
		snk.write(0);
		snk.write(0);
	}
}

#endif //FILTER_RGB_HPP
