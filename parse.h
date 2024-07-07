#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>

constexpr unsigned CHUNK_ID_SIZE = 4;
constexpr unsigned CHUNK_SIZE_SIZE = 4;
constexpr unsigned FORMAT_SIZE = 4;
constexpr unsigned CHUNK_ID_OFFSET = 0;
constexpr unsigned CHUNK_SIZE_OFFSET = CHUNK_ID_OFFSET + CHUNK_ID_SIZE;
constexpr unsigned FORMAT_OFFSET = CHUNK_SIZE_OFFSET + CHUNK_SIZE_SIZE;

class RiffChunk{
	std::vector<uint8_t> chunk_id;
	std::vector<uint8_t> chunk_size;
	std::vector<uint8_t> format;

public:
	RiffChunk(std::vector<uint8_t> *file_data) :
		chunk_id(CHUNK_ID_SIZE), chunk_size(CHUNK_SIZE_SIZE), format(FORMAT_SIZE) {
		memcpy(chunk_id.data(), file_data->data() + CHUNK_ID_OFFSET, CHUNK_ID_SIZE);
		memcpy(chunk_size.data(), file_data->data() + CHUNK_SIZE_OFFSET, CHUNK_SIZE_SIZE);	
		memcpy(format.data(), file_data->data() + FORMAT_OFFSET, FORMAT_SIZE);
	}		

	std::string get_chunk_id() {
		std::string id;
		for(int i = 0; i < chunk_id.size(); ++i) {
			id.push_back((char)chunk_id[i]);
		}
		return std::move(id);
	}

	unsigned get_chunk_size() {
		return *(unsigned *)chunk_size.data();
	}

	std::string get_format() {
		std::string ret_format;
		for(int i = 0; i < format.size(); ++i) {
			ret_format.push_back((char)format[i]);
		}
		return std::move(ret_format);
	}
};

constexpr unsigned UNKNOWN_CHUNK_ID_OFFSET = FORMAT_OFFSET + FORMAT_SIZE;
constexpr unsigned UNKNOWN_CHUNK_SIZE_OFFSET =UNKNOWN_CHUNK_ID_OFFSET + CHUNK_ID_SIZE;

unsigned get_junk_size(std::vector<uint8_t> *file_data) {
	std::string chunk_id(4, '\0');
	// Unknown chunk id takes up 4 bytes
	memcpy(chunk_id.data(), file_data->data() + UNKNOWN_CHUNK_ID_OFFSET, CHUNK_ID_SIZE);
	if(chunk_id == "JUNK") {
		unsigned size = 0;
		memcpy(&size, file_data->data() + UNKNOWN_CHUNK_SIZE_OFFSET, CHUNK_SIZE_SIZE);
		return size;
	} else return 0;
}
unsigned junk_size = 0;

constexpr unsigned FMT_CHUNK_ID_SIZE = 4;
constexpr unsigned FMT_CHUNK_SIZE_SIZE = 4;
constexpr unsigned FMT_AUDIO_FORMAT_SIZE = 2;
constexpr unsigned FMT_NUM_CHANNELS_SIZE = 2;
constexpr unsigned FMT_SAMPLE_RATE_SIZE = 4;
constexpr unsigned FMT_BYTE_RATE_SIZE = 4;
constexpr unsigned FMT_BLOCK_ALIGN_SIZE = 2;
constexpr unsigned FMT_BITS_PER_SAMPLE_SIZE = 2;

const unsigned FMT_CHUNK_ID_OFFSET = UNKNOWN_CHUNK_ID_OFFSET + junk_size;
const unsigned FMT_CHUNK_SIZE_OFFSET = FMT_CHUNK_ID_OFFSET + FMT_CHUNK_ID_SIZE;
const unsigned FMT_AUDIO_FORMAT_OFFSET = FMT_CHUNK_SIZE_OFFSET + FMT_CHUNK_SIZE_SIZE;
const unsigned FMT_NUM_CHANNELS_OFFSET = FMT_AUDIO_FORMAT_OFFSET + FMT_AUDIO_FORMAT_SIZE;
const unsigned FMT_SAMPLE_RATE_OFFSET = FMT_NUM_CHANNELS_OFFSET + FMT_NUM_CHANNELS_SIZE;
const unsigned FMT_BYTE_RATE_OFFSET = FMT_SAMPLE_RATE_OFFSET + FMT_SAMPLE_RATE_SIZE;
const unsigned FMT_BLOCK_ALIGN_OFFSET = FMT_BYTE_RATE_OFFSET + FMT_BYTE_RATE_SIZE;
const unsigned FMT_BITS_PER_SAMPLE_OFFSET = FMT_BLOCK_ALIGN_OFFSET + FMT_BLOCK_ALIGN_SIZE;

class FmtChunk{
	std::vector<uint8_t> chunk_id;
	std::vector<uint8_t> chunk_size;
	std::vector<uint8_t> audio_format;
	std::vector<uint8_t> num_channels;
	std::vector<uint8_t> sample_rate;
	std::vector<uint8_t> byte_rate;
	std::vector<uint8_t> block_align;
	std::vector<uint8_t> bits_per_sample;

public:
	FmtChunk(std::vector<uint8_t> *file_data) :
		chunk_id(FMT_CHUNK_ID_SIZE), chunk_size(FMT_CHUNK_SIZE_SIZE), audio_format(FMT_AUDIO_FORMAT_SIZE),
		num_channels(FMT_NUM_CHANNELS_SIZE), sample_rate(FMT_SAMPLE_RATE_SIZE), byte_rate(FMT_BYTE_RATE_SIZE),
		block_align(FMT_BLOCK_ALIGN_SIZE), bits_per_sample(FMT_BITS_PER_SAMPLE_SIZE)
	{
		memcpy(chunk_id.data(), file_data->data() + FMT_CHUNK_ID_OFFSET, FMT_CHUNK_ID_SIZE);
		memcpy(chunk_size.data(), file_data->data() + FMT_CHUNK_SIZE_OFFSET, FMT_CHUNK_SIZE_SIZE);
		memcpy(audio_format.data(), file_data->data() + FMT_AUDIO_FORMAT_OFFSET, FMT_AUDIO_FORMAT_SIZE);
		memcpy(num_channels.data(), file_data->data() + FMT_NUM_CHANNELS_OFFSET, FMT_NUM_CHANNELS_SIZE);
		memcpy(sample_rate.data(), file_data->data() + FMT_SAMPLE_RATE_OFFSET, FMT_SAMPLE_RATE_SIZE);
		memcpy(byte_rate.data(), file_data->data() + FMT_BYTE_RATE_OFFSET, FMT_BYTE_RATE_SIZE);
		memcpy(block_align.data(), file_data->data() + FMT_BLOCK_ALIGN_OFFSET, FMT_BLOCK_ALIGN_SIZE);
		memcpy(bits_per_sample.data(), file_data->data() + FMT_BITS_PER_SAMPLE_OFFSET, FMT_BITS_PER_SAMPLE_SIZE);
	}
	std::string get_chunk_id() {
		std::string id;
		for(int i = 0; i < chunk_id.size(); ++i) {
			id.push_back((char)chunk_id[i]);
		}
		return std::move(id);
	}

	unsigned get_chunk_size() {
		return *(unsigned *)chunk_size.data();
	}

	unsigned get_audio_format() {
		return *(uint16_t *)audio_format.data();
	}

	unsigned get_num_channels() {
		return *(uint16_t *)num_channels.data();
	}

	unsigned get_sample_rate() {
		return *(unsigned *)sample_rate.data();
	}

	unsigned get_byte_rate() {
		return *(unsigned *)byte_rate.data();
	}

	unsigned get_block_align() {
		return *(uint16_t *)block_align.data();
	}

	unsigned get_bits_per_sample() {
		return *(uint16_t *)bits_per_sample.data();
	}
	
};

const unsigned UNKNOWN_CHUNK_ID_OFFSET_2 = FMT_BITS_PER_SAMPLE_OFFSET + FMT_BITS_PER_SAMPLE_SIZE;
const unsigned UNKNOWN_CHUNK_SIZE_OFFSET_2 = UNKNOWN_CHUNK_ID_OFFSET_2 + CHUNK_ID_SIZE;

unsigned get_fact_size(std::vector<uint8_t> *file_date) {
	std::string id(4, '\0');
	std::memcpy(id.data(), file_date->data() + UNKNOWN_CHUNK_ID_OFFSET_2, CHUNK_ID_SIZE);
	if(id == "fact") {
		unsigned size = 0;
		memcpy(&size, file_date->data() + UNKNOWN_CHUNK_SIZE_OFFSET_2, CHUNK_SIZE_SIZE);
		return size;
	}
	else return 0;
}
unsigned fact_size = 0;

std::vector<uint8_t> *read_file(const char *dir) {
	std::ifstream file(dir, std::ios::binary | std::ios::ate);
	if(!file.is_open()) {
		std::cerr << "Error open file: " << dir << "\n";
		exit(1);
	}
	int file_length = file.tellg();	
	std::vector<uint8_t> *file_data = new std::vector<uint8_t>(file_length);
	file.seekg(0, std::ios::beg);
	file.read((char *)(file_data->data()), file_length);
	return file_data;
}

constexpr unsigned DATA_CHUNK_ID_SIZE = 4;
constexpr unsigned DATA_CHUNK_SIZE_SIZE = 4;

const unsigned DATA_CHUNK_ID_OFFSET = UNKNOWN_CHUNK_ID_OFFSET_2 + fact_size;
const unsigned DATA_CHUNK_SIZE_OFFSET = DATA_CHUNK_ID_OFFSET + DATA_CHUNK_ID_SIZE;
const unsigned DATA_SAMPLED_DATA_OFFSET = DATA_CHUNK_SIZE_OFFSET + DATA_CHUNK_SIZE_SIZE;

class DataChunk{
	std::vector<uint8_t> chunk_id;
	std::vector<uint8_t> chunk_size;

public:
	DataChunk(std::vector<uint8_t> *file_data) : chunk_id(4), chunk_size(4) {
		memcpy(chunk_id.data(), file_data->data() + DATA_CHUNK_ID_OFFSET, DATA_CHUNK_ID_SIZE);
		memcpy(chunk_size.data(), file_data->data() + DATA_CHUNK_SIZE_OFFSET, DATA_CHUNK_SIZE_SIZE);
	}

	std::string get_chunk_id() {
		std::string id;
		for(int i = 0; i < chunk_id.size(); ++i) {
			id.push_back((char)chunk_id[i]);
		}
		return std::move(id);
	}

	unsigned get_chunk_size() {
		return *(unsigned *)chunk_size.data();
	}
};