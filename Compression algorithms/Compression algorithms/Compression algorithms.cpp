#include "Compressor.h"
#include "BitWriter.h"

#include <fstream>
#include <sstream>
#include <cerrno>

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

int main()
{
	/*std::queue<compressor::lz77::triple> table = compressor::lz77::encode("hto_wo_vmie,_to_i_die", 9, 4);
	std::cout << compressor::lz77::decode(table);
	compressor::lz77::encode("badadadabaab", 100, 100);
	compressor::lz77::encode("krasnay_kraska", 8, 5);*/

	std::string result = get_file_contents("lolya.rar");

	////////////
	//
	// ENCODING
	//
	////////////
	
	std::ofstream o("encodedlolya.enc", std::ios_base::out | std::ios_base::binary);
	compressor::lz77::encode_to_stream(result, DICT_32KB, WORD_256C, o);
	o.close();

	////////////
	//
	// DECODING
	//
	////////////

	std::ifstream encoded_stream("encodedlolya.enc", std::ios_base::in | std::ios_base::binary);
	std::string decoded = compressor::lz77::decode_from_stream(encoded_stream);
	encoded_stream.close();

	//std::cout << "DECODED: " << std::endl << std::endl;
	//std::cout << decoded << std::endl;

	std::ofstream decoded_stream("decodedlolya.rar", std::ios_base::out | std::ios_base::binary);
	decoded_stream << decoded;
	decoded_stream.close();

	return 0;
}
