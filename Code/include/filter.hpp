#ifndef FILTER_HPP
#define FILTER_HPP

#include "hls_stream.h"

template<int ROWS, int COLS, typename T, typename f_t>
void filter3x3(hls::stream<T> &src, hls::stream<T> &snk, f_t filter) {
	T line_buffer[2][COLS];
	T window[3][3];

	// Read image
	loop_rows: for (int row = 0; row < ROWS; row++) {
		loop_cols: for (int col = 0; col < COLS; col++) {
			// Slide filter window
			loop_inner_rows: for (int i = 0; i < 3; i++) {
				loop_inner_cols: for (int j = 0; j < 2; j++) {
					window[i][j] = window[i][j + 1];
				}
			}
			window[0][2] = line_buffer[0][col];
			window[1][2] = line_buffer[0][col] = line_buffer[1][col];
			window[2][2] = line_buffer[1][col] = src.read();

			if (row >= 2 && col >= 2) {
				auto res = filter(window);
				snk.write(res);
			}
		}
	}
}

template<int ROWS, int COLS, typename T, typename f_t>
void filter_border3x3(hls::stream<T> &src, hls::stream<T> &snk, f_t filter) {
#pragma HLS INLINE
	T line_buffer[2][COLS];
	T window[3][3];
#pragma HLS ARRAY_PARTITION variable=window dim=1 complete
#pragma HLS ARRAY_PARTITION variable=window dim=2 complete

	// Read image
	loop_rows: for (int row = 0; row < ROWS; row++) {
		loop_cols: for (int col = 0; col < COLS + 1; col++) {
			if (col < COLS) {
				// Slide filter window
				loop_inner_rows: for (int i = 0; i < 3; i++) {
					loop_inner_cols: for (int j = 0; j < 2; j++) {
						window[i][j] = window[i][j + 1];
					}
				}
				window[0][2] = line_buffer[0][col];
				window[1][2] = line_buffer[0][col] = line_buffer[1][col];
				window[2][2] = line_buffer[1][col] = src.read();

				if (row >= 2 && col >= 2) {
					// Meaningful data computation
					auto res = filter(window);
					snk.write(res);
				} else if (row >= 1 && col >= 1) {
					// Write first column and first line
					snk.write(0);
				}
			} else if (row >= 1) {
				// Write last column
				snk.write(0);
			}
		}
	}
	loop_last: for (int col = 0; col < COLS; col++) {
		// Write last line
		snk.write(0);
	}
}

#endif //FILTER_HPP
