#ifndef RGB_TO_HSV_HPP
#define RGB_TO_HSV_HPP

#include <array>

#include "ap_int.h"
#include "hls_math.h"
#include "hls_stream.h"

template <int N> constexpr std::array<ap_ufixed<17,6>, 1 << N> hdiv_lut_gen() {
	std::array<ap_ufixed<17,6>, 1 << N> lut;
	lut[0] = 0;
	for (int i = 1; i < 1 << N; i++) {
		lut[i] = 60.0 / i;
	}

	return lut;
}

template <int N> constexpr std::array<ap_ufixed<2*N,N>, 1 << N> sdiv_lut_gen() {
	std::array<ap_ufixed<2*N,N>, 1 << N> lut;
	lut[0] = 0;
	for (int i = 1; i < 1 << N; i++) {
		lut[i] = ((1 << N) - 1.0) / i;
	}

	return lut;
}

inline void rgb_to_hsv(ap_uint<8> r, ap_uint<8> g, ap_uint<8> b, ap_uint<10> & h, ap_uint<10> & s, ap_uint<10> & v) {
	// Compute the value as maximum of RGB
	auto max = hls::max(hls::max(r, g), b);
	auto min = hls::min(hls::min(r, g), b);
	v = max;

	// Compute chroma and saturation based on maximum
	ap_uint<8> chroma = max - min;
	if (max == 0) {
		s = 0;
	} else {
        // LUT based division ((1 << 8) - 1) / max;
		const auto sdiv_lut = sdiv_lut_gen<8>();
		s = chroma * sdiv_lut[max];
	}

	// Compute Hue based on maximum and chroma
	ap_int<12> hf;
	if (chroma == 0) {
		hf = 0;
	} else if (r == max) {
		hf = g - b;
	} else if (g == max) {
		hf = b - r + 2 * chroma;
	} else {
		hf = r - g + 4 * chroma;
	}
	// LUT based division (60 / chroma)
	const auto hdiv_lut = hdiv_lut_gen<8>();
	h = hf * hdiv_lut[chroma];
	if (h < 0) {
		h += 360;
	}
}

void rgb_to_hsv(hls::stream<ap_uint<8>> &source, hls::stream<ap_uint<10>> &sink) {
#pragma HLS PIPELINE II=3 style=flp
	// Read input data
	auto r = source.read();
	auto g = source.read();
	auto b = source.read();
	ap_uint<10> h, s, v;

	rgb_to_hsv(r, g, b, h, s, v);

	// Write output data
	sink.write(h);
	sink.write(s);
	sink.write(v);
}

void rgb_to_h(hls::stream<ap_uint<8>> &source, hls::stream<ap_uint<10>> &sink) {
#pragma HLS PIPELINE II=3 style=flp
	// Read input data
	auto r = source.read();
	auto g = source.read();
	auto b = source.read();
	ap_uint<10> h, s, v;

	rgb_to_hsv(r, g, b, h, s, v);

	// Write output data
	sink.write(h);
}

#endif //RGB_TO_HSV_HPP
