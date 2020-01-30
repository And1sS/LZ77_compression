#pragma once

#include <istream>
#include <iostream>

class BitReader
{
private:
	std::istream& reader;

	uint16_t current_byte = 0;
	uint8_t current_bit = 0;

public:
	BitReader(std::istream& stream);

	bool read_bit();
	bool eof() { return reader.eof(); }

	uint16_t read_bits(uint8_t quantity);
};

