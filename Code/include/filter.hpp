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

template<int COLS, typename T>
static void shift_window(T new_pixel, int col, T line_buffer[2][COLS], T window[3][3]) {
#pragma HLS INLINE
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            window[i][j] = window[i][j + 1];
        }
    }
    window[0][2] = line_buffer[0][col];
    window[1][2] = line_buffer[0][col] = line_buffer[1][col];
    window[2][2] = line_buffer[1][col] = new_pixel;
}


// LightningSim corrctions : 
// static accesses to stream : no if() read() else read(). 
// perfect loops for accesses.
template<int ROWS, int COLS, typename T, typename f_t>
void filter_border3x3_static_accesses(hls::stream<T> &src, hls::stream<T> &snk, f_t filter) {
#pragma HLS INLINE

    T line_buffer[2][COLS];
    T window[3][3];
#pragma HLS ARRAY_PARTITION variable=window dim=1 complete
#pragma HLS ARRAY_PARTITION variable=window dim=2 complete

    // 1. Initial border (Row 0): Pre-fill line buffer, no writes
    for (int col = 0; col < COLS; col++) {
#pragma HLS PIPELINE II=1
        shift_window<COLS>(src.read(), col, line_buffer, window);
    }

	// 2. Main Processing (Rows 1 to ROWS-1)
 	for (int row = 1; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
#pragma HLS PIPELINE II=1
			T t = src.read();
			shift_window<COLS>(t, col, line_buffer, window);
			T data;

			if(col == 0 || col == COLS-1 || row==1) {
				// write 0 to top row and left/right columns
				data = 0;
			} else {
				data = filter(window);
			}
			snk.write(data);
		}
	}

    // 3. Final border (Last Row): Flush remaining writes, no reads
    for (int col = 0; col < COLS; col++) {
#pragma HLS PIPELINE II=1
        snk.write(0);
    }
}

#endif //FILTER_HPP
