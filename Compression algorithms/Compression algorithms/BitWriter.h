#pragma once

#include <ostream>
#include <iostream>

class BitWriter
{
private:
	uint8_t current_byte = 0;
	uint8_t current_bit = 0; 

	std::ostream& writer;

	void _check_last();

public:
	void write_bits(uint16_t num, uint8_t quantity);
	void write_byte(uint8_t num);
	void finish();
	BitWriter(std::ostream& stream) : writer(stream)
	{}
};

