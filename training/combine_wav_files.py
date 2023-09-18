import os
import wave
from concurrent.futures import ThreadPoolExecutor

import numpy as np
from scipy.io import wavfile


def read_wav(wav_file):
    wav_file_path = os.path.join(input_dir, wav_file)
    _, audio_data = wavfile.read(wav_file_path)
    return audio_data


def combine_wav_files(input_dir, output_file, limit=500000):
    """Combines all WAV files in a directory into a single file until the shape reaches the limit.

    Args:
      input_dir: The directory containing the WAV audio files.
      output_file: The path to the combined WAV audio file.
      limit: The maximum shape of the combined WAV audio file.
    """

    # Get a list of all WAV files in the input directory
    wav_files = [f for f in os.listdir(input_dir) if f.endswith('.wav')]

    # Initialize an empty list to store audio data
    combined_data = np.zeros(limit, dtype=np.int16)
    current_size = 0

    # Use ThreadPoolExecutor for multi-threading
    with ThreadPoolExecutor() as executor:
        futures = []
        for wav_file in wav_files:
            future = executor.submit(read_wav, wav_file)
            futures.append(future)

        for future in futures:
            audio_data = future.result()

            # Calculate the remaining space in the combined_data
            remaining_space = limit - current_size

            # If the audio data is smaller than the remaining space, add it all to the combined_data
            if len(audio_data) <= remaining_space:
                combined_data[current_size:current_size + len(audio_data)] += audio_data
                current_size += len(audio_data)
            # Otherwise, add as much of the audio data as possible to the combined_data
            else:
                combined_data[current_size:] += audio_data[:remaining_space]
                current_size = limit

    print(f"output shape :{combined_data.shape}")
    # Save the combined audio data as a new WAV file
    wavfile.write(output_file, 16000, combined_data)
    print("WAV files combined successfully!")


def wav_to_pcm(input_wav_file, output_pcm_file):
    # Open the WAV file
    with wave.open(input_wav_file, 'rb') as wav_file:
        # Get the audio parameters
        sample_width = wav_file.getsampwidth()
        num_channels = wav_file.getnchannels()
        frame_rate = wav_file.getframerate()

        # Read all audio frames from the WAV file
        audio_frames = wav_file.readframes(wav_file.getnframes())

    # Write the audio data to the output PCM file
    with open(output_pcm_file, 'wb') as pcm_file:
        pcm_file.write(audio_frames)

    print("WAV to PCM conversion successful!")


def combine_audio_files(input_directory, output_filename):
    # Get a list of all audio files in the directory
    audio_files = [f for f in os.listdir(input_directory) if f.endswith('.wav')]

    # Check if there are any audio files in the directory
    if not audio_files:
        print("No audio files found in the directory.")
        return

    # Create a new output PCM audio file
    output_pcm = wave.open(output_filename, 'wb')

    # Set the audio file properties based on the first audio file in the directory
    with wave.open(os.path.join(input_directory, audio_files[0]), 'rb') as first_audio:
        num_channels = first_audio.getnchannels()
        sample_width = first_audio.getsampwidth()
        frame_rate = first_audio.getframerate()
        output_pcm.setnchannels(num_channels)
        output_pcm.setsampwidth(sample_width)
        output_pcm.setframerate(frame_rate)
    total_samples = 0
    for audio_file in audio_files:
        with wave.open(os.path.join(input_directory, audio_file), 'rb') as input_pcm:
            # Append PCM data from each audio file to the output PCM file
            input_frames = input_pcm.getnframes()
            print(f"audio file frame length: {input_frames}")
            output_pcm.writeframes(input_pcm.readframes(input_frames))
            total_samples += input_frames

    output_pcm.close()
    print(f"Combined audio files into '{output_filename}'.")
    print(f"Total samples in the combined audio: {total_samples}.")


if __name__ == '__main__':
    # Usage example:
    input_dir = 'D:/zeynep/data/noise-cancelling/valentini/noisy_testset_wav'
    output_file = 'D:/zeynep/data/noise-cancelling/valentini//all_noise.pcm'
    combine_audio_files(input_dir, output_file)

    input_wav_file = 'D:/zeynep/data/noise-cancelling/MS-SNSD/all_train_noise.wav'
    output_pcm_file = 'D:/zeynep/data/noise-cancelling/MS-SNSD/all_train_noise.pcm'
    #wav_to_pcm(input_wav_file, output_pcm_file)
