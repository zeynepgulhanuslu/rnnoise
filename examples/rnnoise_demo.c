/* Copyright (c) 2018 Gregor Richards
 * Copyright (c) 2017 Mozilla */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <sndfile.h>
#include "rnnoise.h"

#define FRAME_SIZE 480

int main(int argc, char **argv) {
  int i;
  int first = 1;
  float x[FRAME_SIZE];
  SNDFILE *f1, *fout;
  SF_INFO info;
  DenoiseState *st;
  st = rnnoise_create(NULL);
  if (argc!=3) {
    fprintf(stderr, "usage: %s <noisy speech> <output denoised>\n", argv[0]);
    return 1;
  }
  f1 = sf_open(argv[1], SFM_READ, &info);
  fout = sf_open(argv[2], SFM_WRITE, &info);
  while (1) {
    short tmp[FRAME_SIZE];
    sf_count_t n = sf_read_short(f1, tmp, FRAME_SIZE);
    if (n == 0) break;
    for (i=0;i<FRAME_SIZE;i++) x[i] = tmp[i];
    rnnoise_process_frame(st, x, x);
    for (i=0;i<FRAME_SIZE;i++) tmp[i] = x[i];
    if (!first) sf_write_short(fout, tmp, FRAME_SIZE);
    first = 0;
  }
  rnnoise_destroy(st);
  sf_close(f1);
  sf_close(fout);
  return 0;
}

