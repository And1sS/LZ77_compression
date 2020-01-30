#pragma once

#include "Compressor.h"

std::vector<Tree<int, char>*> get_freq_table(const std::string& input)
{
	std::map<char, int> char_counts;
	for (auto c : input)
		char_counts[c]++;

	std::vector<Tree<int, char>*> result;
	for (auto it = char_counts.begin(); it != char_counts.end(); ++it)
		result.push_back(new Tree<int, char>(it->second, it->first));

	return result;
}

Tree<int, char>* compressor::huffman::get_char_tree(const std::string& input)
{
	auto creation_list = get_freq_table(input);

	while (creation_list.size() > 1)
	{
		std::sort(creation_list.begin(), creation_list.end(),
			Tree_comparator<int, char>());
		const auto left = creation_list[0];
		const auto right = creation_list[1];

		creation_list.erase(creation_list.begin(), creation_list.begin() + 2);
		creation_list.push_back(Tree<int, char>::connect(left, right));
	}

	return creation_list[0];
}

std::map<char, std::string> get_encoding_table(Tree<int, char>* char_tree)
{
	std::stack<std::pair<Tree<int, char>*, std::string>> sk;
	std::map<char, std::string> encoding_table;

	sk.push(std::pair<Tree<int, char>*, std::string>(char_tree, ""));

	while (!sk.empty())
	{
		auto p = sk.top();
		sk.pop();

		if ((p.first->get_left() == nullptr) &&
			(p.first->get_right() == nullptr))
		{
			encoding_table[p.first->get_value()] = p.second;
			continue;
		}

		if (p.first->get_left() != nullptr)
			sk.push(std::pair<Tree<int, char>*, std::string>
			(p.first->get_left(), p.second + "0"));

		if (p.first->get_right() != nullptr)
			sk.push(std::pair<Tree<int, char>*, std::string>
			(p.first->get_right(), p.second + "1"));
	}

	return encoding_table;
}

std::string compressor::huffman::encode(const std::string& input)
{
	const auto char_tree = get_char_tree(input);
	auto encoding_table = get_encoding_table(char_tree);

	std::string result;
	for (auto c : input)
		result += encoding_table[c];

	return result;

}

std::pair<size_t, size_t> longest_substr(const std::string& a, const std::string& b)
{
	int max_length = 0;
	int max_start = 0;
	for (int i = 0; i < a.size(); i++)
	{
		int start = i;
		int index = 0;
		int length = 0;
		while ((a[i] == b[index]) && (i < a.size()) && (index < b.size()))
		{
			i++;
			index++;
			length++;

			if (i >= a.size() || index >= b.size())
				break;
		}

		if (length > max_length)
		{
			max_length = length;
			max_start = start;
		}
	}
	return std::pair<size_t, size_t>(max_start, max_length);
}

std::string to_binary(int a, size_t n)
{
	std::string result = "";
	for (size_t i = 0; i < n; i++)
	{
		if (a != 0)
		{
			if (a % 2 == 1)
				result.insert(result.begin(), '1');
			else
				result.insert(result.begin(), '0');
			a >>= 1;
		}
		else
			result.insert(result.begin(), '0');
	}

	return result;
}

std::queue<compressor::lz77::triple> compressor::lz77::encode(const std::string& input, size_t buffer_size, size_t length)
{
	size_t bstart = 0;
	size_t blength = 0;

	std::queue<triple> encoded_sybmols;

	triple tmp;
	for (size_t i = 0; i < input.size();)
	{
		if (i >= SIZE_MAX)
			std::cout << "MAX";
		if (i < buffer_size)
		{
			bstart = 0;
			blength = i;
		}
		else
		{
			bstart = i - buffer_size;
			blength = buffer_size;
		}
		//std::cout << "buffer after: " << input.substr(bstart, blength) << std::endl;
		int match_index = -1;


		for (size_t j = 0; j < blength; j++)
		{
			if (input[i] == input[bstart + j])
				match_index = j;
		}

		if (match_index == -1)
		{
			tmp.distance = 0;
			tmp.length = 0;
			tmp.symbol = input[i];
			encoded_sybmols.push(tmp);
			//std::cout << "<" << tmp.distance << ", " << tmp.length << ", " << tmp.symbol << ">" << std::endl;
			if (++i == input.size())
				break;
		}
		else
		{
			if (i + length >= input.size())
				length = input.size() - i;

			std::pair<size_t, size_t> substr_info = longest_substr(
				//input.substr(bstart, length + 1), input.substr(i, length));
				input.substr(bstart, blength), input.substr(i, length));


			i += substr_info.second + 1;
			tmp.distance = blength - substr_info.first; // distance from right edge of the buffer
			tmp.length = substr_info.second;
			tmp.symbol = input[i - 1];

			encoded_sybmols.push(tmp);
			//std::cout << "<" << tmp.distance << ", " << tmp.length << ", " << tmp.symbol << ">" << std::endl;
			if (i >= input.size())
				break;
		}
	}
	return encoded_sybmols;
}

void compressor::lz77::encode_to_stream(const std::string& input, size_t buffer_size, size_t length, std::ostream& stream)
{
	std::queue<triple> table = encode(input, buffer_size, length);
	BitWriter writer(stream);
	triple tmp;
	
	uint8_t dist_bits = std::ceil(std::log2(buffer_size)) + 1;
	uint8_t length_bits = std::ceil(std::log2(length)) + 1;
	uint8_t char_bits = 8;

	uint32_t table_size = table.size();

	uint8_t first_byte = (table_size << 24) >> 24;
	uint8_t second_byte = (table_size << 16) >> 24;
	uint8_t third_byte = (table_size << 8) >> 24;
	uint8_t fourth_byte = table_size >> 24;

	writer.write_byte(first_byte);
	writer.write_byte(second_byte);
	writer.write_byte(third_byte);
	writer.write_byte(fourth_byte);

	writer.write_bits(dist_bits, 8);
	writer.write_bits(length_bits, 8);

	while (!table.empty())
	{
		tmp = table.front();
		table.pop();

		writer.write_bits(tmp.distance, dist_bits);
		writer.write_bits(tmp.length, length_bits);
		writer.write_bits(tmp.symbol, char_bits);
	}
	writer.finish();
}

std::string compressor::lz77::decode(std::queue<triple> table)
{
	std::string result = "";
	while (!table.empty())
	{
		triple input = table.front();
		table.pop();
		//std::cout << "<" << input.distance << ", " << input.length << ", " << input.symbol << ">" << std::endl;
		if (input.length != 0)
			result += result.substr(result.length() - input.distance, input.length);
		result += input.symbol;
	}

	return result;
}

std::string compressor::lz77::decode_from_stream(std::istream& stream)
{
	std::queue<triple> table;
	BitReader reader(stream);
	triple tmp;

	
	uint32_t first_byte = (uint32_t) reader.read_bits(8);
	uint32_t second_byte = (uint32_t) reader.read_bits(8) << 8;
	uint32_t third_byte = (uint32_t) reader.read_bits(8) << 16;
	uint32_t fourth_byte = (uint32_t) reader.read_bits(8) << 24;
	uint32_t triads = first_byte + second_byte + third_byte + fourth_byte;

	//std::cout << "triads = " << triads << std::endl;
	uint8_t dist_bits = reader.read_bits(8);
	uint8_t length_bits = reader.read_bits(8);
	//std::cout << "dist_bits = " << dist_bits << std::endl;
	//std::cout << "length_bits = " << length_bits << std::endl;

	for(size_t i = 0; i < triads; i++)
	{
		tmp.distance = reader.read_bits(dist_bits);
		tmp.length = reader.read_bits(length_bits);
		tmp.symbol = reader.read_bits(8);
		table.push(tmp);
		//std::cout << "<" << tmp.distance << ", " << tmp.length << ", " << tmp.symbol << ">" << std::endl;
	}
	return decode(table);
}
