#include <stdio.h>
#include <stdlib.h>
#include "rnnoise.h"

#define FRAME_SIZE 480

// Function to read a WAV file
int read_wav(const char *filename, short **data, int *num_samples) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open input file");
        return 1;
    }

    // Skip WAV header (assuming it's 44 bytes)
    fseek(file, 44, SEEK_SET);

    // Calculate the number of samples
    fseek(file, 0, SEEK_END);
    *num_samples = ftell(file) / sizeof(short);
    fseek(file, 44, SEEK_SET);

    *data = (short *)malloc(sizeof(short) * (*num_samples));
    if (!(*data)) {
        perror("Failed to allocate memory");
        fclose(file);
        return 1;
    }

    fread(*data, sizeof(short), *num_samples, file);
    fclose(file);
    return 0;
}

// Function to write a WAV file
int write_wav(const char *filename, short *data, int num_samples) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open output file");
        return 1;
    }

    // Write WAV header (assuming it's 44 bytes)
    char header[44] = {
        'R', 'I', 'F', 'F',  // Chunk ID
        0, 0, 0, 0,            // Chunk size (to be filled)
        'W', 'A', 'V', 'E',  // Format
        'f', 'm', 't', ' ',  // Subchunk1 ID
        16, 0, 0, 0,           // Subchunk1 size (PCM)
        1, 0,                  // Audio format (PCM)
        1, 0,                  // Number of channels (1 for mono, 2 for stereo)
        0x80, 0x3E, 0x00, 0x00,  // Sample rate (16,000 Hz)
        0x00, 0x7D, 0x00, 0x00,  // Byte rate (16,000 * 2)
        2, 0,                  // Block align (2 bytes per sample)
        16, 0,                 // Bits per sample (16 bits)
        'd', 'a', 't', 'a',  // Subchunk2 ID
        0, 0, 0, 0             // Subchunk2 size (to be filled)
    };

    int subchunk2_size = num_samples * sizeof(short);
    int chunk_size = subchunk2_size + 36;
    memcpy(header + 4, &chunk_size, 4);
    memcpy(header + 40, &subchunk2_size, 4);

    fwrite(header, 1, 44, file);
    fwrite(data, sizeof(short), num_samples, file);
    fclose(file);
    return 0;
}

int main(int argc, char **argv) {
    int i;
    float x[FRAME_SIZE];
    short *input_data;
    int num_samples;
    DenoiseState *st;
    st = rnnoise_create(NULL);

    if (argc != 3) {
        fprintf(stderr, "usage: %s <noisy speech WAV> <output denoised WAV>\n", argv[0]);
        return 1;
    }

    if (read_wav(argv[1], &input_data, &num_samples) != 0) {
        fprintf(stderr, "Failed to read input WAV file.\n");
        return 1;
    }

    short *output_data = (short *)malloc(sizeof(short) * num_samples);

    for (i = 0; i < num_samples; i += FRAME_SIZE) {
        int frame_samples = (i + FRAME_SIZE <= num_samples) ? FRAME_SIZE : (num_samples - i);

        for (int j = 0; j < frame_samples; j++) {
            x[j] = input_data[i + j];
        }

        rnnoise_process_frame(st, x, x);
        int j = 0;
        for (j = 0; j < frame_samples; j++) {
            output_data[i + j] = x[j];
        }
    }

    if (write_wav(argv[2], output_data, num_samples) != 0) {
        fprintf(stderr, "Failed to write output WAV file.\n");
        return 1;
    }

    rnnoise_destroy(st);
    free(input_data);
    free(output_data);
    return 0;
}
