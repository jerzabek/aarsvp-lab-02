#include <omp.h>
#include <stdio.h>

#define WIDTH 3840
#define HEIGHT 2160
#define TOTAL_PIXELS WIDTH *HEIGHT
#define NUM_FRAMES 60
#define SUBSAMP_PIXELS TOTAL_PIXELS / 4

unsigned char R[TOTAL_PIXELS], G[TOTAL_PIXELS], B[TOTAL_PIXELS];
unsigned char Y[TOTAL_PIXELS], U[TOTAL_PIXELS], V[TOTAL_PIXELS];

unsigned char U_sub[SUBSAMP_PIXELS], V_sub[SUBSAMP_PIXELS];

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <input_filename>\n", argv[0]);
    return 1;
  }

  char *input_filename = argv[1];

  printf("[AARSVP-LAB02] Starting - Reading from file: %s\n", input_filename);

  double start_time_global = omp_get_wtime();
  double start_time = omp_get_wtime();
  FILE *fp = fopen(input_filename, "rb");

  /** --------------------------- citanje --------------------------- */
  for (int frame = 0; frame < NUM_FRAMES; ++frame) {
    fread(R, 1, TOTAL_PIXELS, fp);
    fread(G, 1, TOTAL_PIXELS, fp);
    fread(B, 1, TOTAL_PIXELS, fp);

#pragma omp parallel for
    for (int i = 0; i < TOTAL_PIXELS; ++i) {
      Y[i] = 0.257 * R[i] + 0.504 * G[i] + 0.098 * B[i] + 16;
      U[i] = -0.148 * R[i] - 0.291 * G[i] + 0.439 * B[i] + 128;
      V[i] = 0.439 * R[i] - 0.368 * G[i] - 0.071 * B[i] + 128;
    }
  }

  fclose(fp);

  double end_time = omp_get_wtime();
  double time_taken = end_time - start_time;

  printf("[AARSVP-LAB02] Finished reading in %f seconds\n", time_taken);

  /** --------------------------- spremanje cistog YUV videa --------------------------- */

  start_time = omp_get_wtime();

  FILE *fp_out = fopen("output-raw.yuv", "wb");

  for (int frame = 0; frame < NUM_FRAMES; ++frame) {
    fwrite(Y, 1, TOTAL_PIXELS, fp_out);
    fwrite(U, 1, TOTAL_PIXELS, fp_out);
    fwrite(V, 1, TOTAL_PIXELS, fp_out);
  }

  fclose(fp_out);

  end_time = omp_get_wtime();
  time_taken = end_time - start_time;

  printf("[AARSVP-LAB02] Finished writing output-raw.yuv in %f seconds\n", time_taken);

  /** --------------------------- poduzorkovanje --------------------------- */
  start_time = omp_get_wtime();

#pragma omp parallel for collapse(2)
  for (int frame = 0; frame < NUM_FRAMES; ++frame) {
    for (int i = 0; i < HEIGHT; i += 2) {
      for (int j = 0; j < WIDTH; j += 2) {
        int index = i * WIDTH + j;

        int subsample_index = (i / 2) * (WIDTH / 2) + (j / 2);

        /**
         * Matrica:
         * U_UpperLeft U_UpperRight
         * U_LowerLeft U_LowerRight
         *
         * Ovo uzmemo i izračunamo prosjek, isto tako za V
         */
        unsigned char U_UpperLeft = U[index];
        unsigned char U_UpperRight = U[index + 1];
        unsigned char U_LowerLeft = U[index + WIDTH];
        unsigned char U_LowerRight = U[index + WIDTH + 1];

        unsigned char V_UpperLeft = V[index];
        unsigned char V_UpperRight = V[index + 1];
        unsigned char V_LowerLeft = V[index + WIDTH];
        unsigned char V_LowerRight = V[index + WIDTH + 1];

        unsigned char U_avg = (U_UpperLeft + U_UpperRight + U_LowerLeft + U_LowerRight) / 4.0;
        unsigned char V_avg = (V_UpperLeft + V_UpperRight + V_LowerLeft + V_LowerRight) / 4.0;

        U_sub[subsample_index] = U_avg;
        V_sub[subsample_index] = V_avg;
      }
    }
  }

  end_time = omp_get_wtime();
  time_taken = end_time - start_time;

  printf("[AARSVP-LAB02] Finished processing subsampling 4:2:0 in %f seconds\n", time_taken);

  start_time = omp_get_wtime();

  FILE *fp_out_subsample = fopen("output-subsample.yuv", "wb");

  for (int frame = 0; frame < NUM_FRAMES; ++frame) {
    fwrite(Y, 1, TOTAL_PIXELS, fp_out_subsample);
    fwrite(U_sub, 1, TOTAL_PIXELS / 4, fp_out_subsample);
    fwrite(V_sub, 1, TOTAL_PIXELS / 4, fp_out_subsample);
  }

  fclose(fp_out_subsample);

  end_time = omp_get_wtime();
  time_taken = end_time - start_time;

  printf("[AARSVP-LAB02] Finished writing to output-subsample.yuv in %f seconds\n", time_taken);

  /** --------------------------- naduzorkovanje --------------------------- */
  start_time = omp_get_wtime();

#pragma omp parallel for collapse(2)
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      int index = i * WIDTH + j;

      int subsample_index = (i / 2) * (WIDTH / 2) + (j / 2);

      U[index] = U_sub[subsample_index];
      V[index] = V_sub[subsample_index];
    }
  }

  end_time = omp_get_wtime();
  time_taken = end_time - start_time;

  printf("[AARSVP-LAB02] Finished processing upsampling in %f seconds\n", time_taken);

  start_time = omp_get_wtime();

  FILE *fp_out_upsample = fopen("output-upsample.yuv", "wb");

  for (int frame = 0; frame < NUM_FRAMES; ++frame) {
    fwrite(Y, 1, TOTAL_PIXELS, fp_out_upsample);
    fwrite(U, 1, TOTAL_PIXELS, fp_out_upsample);
    fwrite(V, 1, TOTAL_PIXELS, fp_out_upsample);
  }

  fclose(fp_out_upsample);

  end_time = omp_get_wtime();
  time_taken = end_time - start_time;
  double total_time_taken = omp_get_wtime() - start_time_global;

  printf("[AARSVP-LAB02] Finished writing to output-upsample.yuv in %f seconds\n", time_taken);
  printf("[AARSVP-LAB02] Completed in total of %f seconds\n", total_time_taken);

  return 0;
}
