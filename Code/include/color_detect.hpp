#ifndef COLOR_DETECT_HPP
#define COLOR_DETECT_HPP

#include "ap_int.h"
#include "hls_stream.h"

template<int ROWS, int COLS, int RESULT_SIZE>
void color_detect_hsv(hls::stream<ap_uint<10>> &src, hls::stream<ap_uint<16>> &thresholds, hls::stream<ap_uint<RESULT_SIZE>> &snk) {
	auto low_threshold = thresholds.read();
	auto high_threshold = thresholds.read();

	// Read image
	loop_rows: for (int row = 0; row < ROWS; row++) {
		loop_cols: for (int col = 0; col < COLS; col++) {
#pragma HLS PIPELINE II=3
			auto h = src.read();
			auto s = src.read();
			auto v = src.read();
			if (h >= low_threshold and h <= high_threshold) {
				snk.write((1 << RESULT_SIZE) - 1);
			} else {
				snk.write(0);
			}
		}
	}
}

template<int ROWS, int COLS, int RESULT_SIZE>
void color_detect_h(hls::stream<ap_uint<10>> &src, hls::stream<ap_uint<16>> &thresholds, hls::stream<ap_uint<RESULT_SIZE>> &snk) {
	auto low_threshold = thresholds.read();
	auto high_threshold = thresholds.read();

	// Read image
	loop_rows: for (int row = 0; row < ROWS; row++) {
		loop_cols: for (int col = 0; col < COLS; col++) {
#pragma HLS PIPELINE II=1
			auto h = src.read();
			if (h >= low_threshold and h <= high_threshold) {
				snk.write((1 << RESULT_SIZE) - 1);
			} else {
				snk.write(0);
			}
		}
	}
}

template<int ROWS, int COLS>
void color_detect_hsv_binary(hls::stream<ap_uint<10>> &src, hls::stream<ap_uint<16>> &thresholds, hls::stream<ap_uint<1>> &snk) {
	color_detect_hsv<ROWS, COLS, 1>(src, thresholds, snk);
}

template<int ROWS, int COLS>
void color_detect_hsv_8bits(hls::stream<ap_uint<10>> &src, hls::stream<ap_uint<16>> &thresholds, hls::stream<ap_uint<8>> &snk) {
	color_detect_hsv<ROWS, COLS, 8>(src, thresholds, snk);
}

template<int ROWS, int COLS>
void color_detect_h_binary(hls::stream<ap_uint<10>> &src, hls::stream<ap_uint<16>> &thresholds, hls::stream<ap_uint<1>> &snk) {
	color_detect_h<ROWS, COLS, 1>(src, thresholds, snk);
}

template<int ROWS, int COLS>
void color_detect_h_8bits(hls::stream<ap_uint<10>> &src, hls::stream<ap_uint<16>> &thresholds, hls::stream<ap_uint<8>> &snk) {
	color_detect_h<ROWS, COLS, 8>(src, thresholds, snk);
}

#endif //COLOR_DETECT_HPP
