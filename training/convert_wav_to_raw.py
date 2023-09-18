from pydub import AudioSegment
import soundfile as sf


def wav_to_raw(input_wav_file, output_raw_file):
    # Read the WAV file
    audio = AudioSegment.from_file(input_wav_file)

    # Set the audio parameters for the raw file
    channels = audio.channels
    sample_width = 2  # 16-bit (2 bytes)
    frame_rate = audio.frame_rate

    # Convert audio to raw format data
    raw_data = audio.raw_data
    print(f"raw data {len(raw_data)}")
    # Save the raw data as a new raw file
    with open(output_raw_file, 'wb') as raw_file:
        raw_file.write(raw_data)

    print("WAV to RAW conversion successful!")


# Usage example:
input_wav_file = 'D:/zeynep/data/noise-cancelling/MS-SNSD/noise_train/NeighborSpeaking_13.wav'
output_raw_file = 'D:/zeynep/data/noise-cancelling/MS-SNSD/all_train_noise.pcm'
wav_to_raw(input_wav_file, output_raw_file)
