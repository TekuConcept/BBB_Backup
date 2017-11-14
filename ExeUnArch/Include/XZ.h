/**
 * Created by TekuConcept on August 8, 2017
 */

#ifndef XZ_H
#define XZ_H

#include <lzma.h>
#include "cstypes.h"
#include <fstream>

class XZ {
public:
	static String compress(String target);
	static String decompress(String source);

private:
	XZ();

	static void initEncoder(lzma_stream *strm, uint32_t preset);
	static void compressionHelper(lzma_stream *strm,
		std::ifstream &infile, std::ofstream &outfile);

	static void initDecoder(lzma_stream *strm);
	static void decompHelper(lzma_stream *strm, String inname,
		std::ifstream &infile, std::ofstream &outfile);
};

#endif