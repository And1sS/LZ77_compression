#pragma once

#include "Tree.h"
#include "BitWriter.h"
#include "BitReader.h"
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <map>
#include <algorithm>
#include <utility>
#include <iostream>
#include <cmath>

namespace compressor
{
	#define DICT_32KB 32768
	#define DICT_16KB 16384
	#define DICT_8KB 8192
	#define DICT_4KB 4096
	#define DICT_2KB 2048
	#define DICT_1KB 1024
	#define DICT_512B 512
	#define DICT_256B 256
	#define DICT_128B 128

	#define WORD_32C 32
	#define WORD_64C 64
	#define WORD_128C 128
	#define WORD_192C 192
	#define WORD_256C 256

	namespace huffman
	{
		std::string encode(const std::string& input);
		Tree<int, char>* get_char_tree(const std::string& input);
	}

	namespace lz77
	{
		typedef struct
		{
			size_t distance;
			uint8_t length;
			char symbol;
		} triple;


		std::queue<triple> encode(const std::string& input, size_t buffer_size, size_t length);
		std::string decode(std::queue<triple> table);

		void encode_to_stream(const std::string& input, size_t buffer_size, size_t length, std::ostream& stream);
		std::string decode_from_stream(std::istream& stream);
	}
}