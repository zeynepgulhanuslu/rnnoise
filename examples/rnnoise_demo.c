#include <stdio.h>
#include <stdlib.h>
#include "rnnoise.h"
#include <sndfile.h>

#define FRAME_SIZE 480

int main(int argc, char **argv) {
  int i;
  int first = 1;
  float x[FRAME_SIZE];
  SNDFILE *f1, *fout;
  SF_INFO info;
  DenoiseState *st;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <noisy speech WAV> <output denoised WAV>\n", argv[0]);
    return 1;
  }

  f1 = sf_open(argv[1], SFM_READ, &info);
  if (!f1) {
    fprintf(stderr, "Error opening input WAV file.\n");
    return 1;
  }

  fout = sf_open(argv[2], SFM_WRITE, &info);
  if (!fout) {
    fprintf(stderr, "Error opening output WAV file.\n");
    sf_close(f1);
    return 1;
  }

  st = rnnoise_create(NULL);

  while (1) {
    short tmp[FRAME_SIZE];
    sf_count_t count = sf_read_short(f1, tmp, FRAME_SIZE);
    if (count <= 0) break;

    for (i = 0; i < FRAME_SIZE; i++) {
      x[i] = tmp[i];
    }

    rnnoise_process_frame(st, x, x);

    for (i = 0; i < FRAME_SIZE; i++) {
      tmp[i] = x[i];
    }

    if (!first) {
      sf_write_short(fout, tmp, FRAME_SIZE);
    }
    first = 0;
  }

  rnnoise_destroy(st);
  sf_close(f1);
  sf_close(fout);

  return 0;
}
