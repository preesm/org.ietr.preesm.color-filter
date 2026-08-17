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

	// to satisfy lightningsim's VERY strict parser. Statically predictable accesses baby !
	T r_out;
	T g_out;
	T b_out;
	if (condition.read() > 0) {
		r_out = r;
		g_out = g;
		b_out = b;
	} else {
		r_out = 0;
		g_out = 0;
		b_out = 0;
	}
	snk.write(r_out);
	snk.write(g_out);
	snk.write(b_out);
}

#endif //FILTER_RGB_HPP
