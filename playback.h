#pragma once
#include <fstream>
#include "parse.h"
#include <portaudio.h>
#include <iostream>
#include <vector>
#include <cstdint>
#include <utility>

unsigned NUM_CHANNELS;
unsigned SAMPLE_RATE;
unsigned SAMPLE_SIZE;
PaSampleFormat sample_format;
constexpr unsigned FRAMES_PER_BUFFER = 256;

struct AudioData{
	uint8_t *data;
	size_t data_size;
	size_t offset;
};

int callback(const void *input,
						void *output,
						unsigned long frames_per_buffer,
						const PaStreamCallbackTimeInfo *time_info,
						PaStreamCallbackFlags status_flags,
						void *user_data)
{
	AudioData *audio_data = (AudioData *)user_data;
	size_t bytes_to_play = frames_per_buffer * 	NUM_CHANNELS * SAMPLE_SIZE;
	size_t bytes_remaining = audio_data->data_size - audio_data->offset;

	if(bytes_remaining < bytes_to_play) {
		memcpy(output, audio_data->data + audio_data->offset, bytes_remaining);
		memcpy((uint8_t *)output + bytes_remaining, 0, bytes_to_play - bytes_remaining);
		audio_data->offset += bytes_remaining;
		return paComplete;
	} else {
		memcpy(output, audio_data->data + audio_data->offset, bytes_to_play);
		audio_data->offset += bytes_to_play;
		return paContinue;
	}
}

void set_audio_info(std::vector<uint8_t> *file_data) {
	RiffChunk riff_chunck(file_data);
	junk_size = get_junk_size(file_data);
	FmtChunk fmt_chunk(file_data);
	fact_size = get_fact_size(file_data);
	NUM_CHANNELS = fmt_chunk.get_num_channels();
	SAMPLE_RATE = fmt_chunk.get_sample_rate();
	SAMPLE_SIZE = fmt_chunk.get_bits_per_sample() / 8;
	if(fmt_chunk.get_audio_format() == 1) {
		if(fmt_chunk.get_bits_per_sample() == 32) {
			sample_format = paInt32;
		} else if (fmt_chunk.get_bits_per_sample() == 16) {
			sample_format = paInt16;
		} else if (fmt_chunk.get_bits_per_sample() == 24) {
			sample_format = paInt24;
		}
	} else if(fmt_chunk.get_audio_format() == 3) {
		sample_format = paFloat32;
	}
}

std::pair<uint8_t *, unsigned> get_raw_data(std::vector<uint8_t> *file_data) {
	DataChunk data_chunk(file_data);
	uint8_t *raw_data_ptr = file_data->data() + DATA_SAMPLED_DATA_OFFSET;
	unsigned raw_data_size = data_chunk.get_chunk_size() - DATA_CHUNK_ID_SIZE - DATA_CHUNK_SIZE_SIZE;
	return std::make_pair(raw_data_ptr, raw_data_size);
}

enum PLAY_RESULT{
	PLAY_SUCCESS, PLAY_FAILURE
};

PLAY_RESULT playback(std::vector<uint8_t> *file_data){
	set_audio_info(file_data);
	auto raw_data_info = get_raw_data(file_data);
	uint8_t *raw_data = raw_data_info.first;
	unsigned raw_data_size = raw_data_info.second;
	PaError err = Pa_Initialize();
	if(err != paNoError) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err);
		return PLAY_FAILURE;
	}
	AudioData audio_data = {raw_data, raw_data_size, 0};
	PaStream *stream;
	err = Pa_OpenDefaultStream(&stream, 0, NUM_CHANNELS, sample_format, SAMPLE_RATE,
								FRAMES_PER_BUFFER, callback, &audio_data);

	if(err != paNoError) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err);
		return PLAY_FAILURE;
	}
	err = Pa_StartStream(stream);
	if(err != paNoError) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err);
		return PLAY_FAILURE;
	}
	while(Pa_IsStreamActive(stream)) {
		Pa_Sleep(100);
	}
	err = Pa_CloseStream(stream);
	if(err != paNoError) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err);
		return PLAY_FAILURE;
	}
	Pa_Terminate();
	return PLAY_SUCCESS;
}