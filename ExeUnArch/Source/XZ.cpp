/**
 * Created by TekuConcept on August 8, 2017
 */

#include "XZ.h"
#include "File.h"
#include "SystemExceptions.h"

#include <iostream>
#define DMSG(x) std::cout << x << std::endl

using namespace System;
using namespace IO;

XZ::XZ() {}

String XZ::compress(String target) {
	if (target.length() == 0)
		throw ArgumentException("ArgumentException: Target path too short!");
	if (!File::Exists(target))
		throw IOException("IOException: Target does not exist!");

	std::ifstream file(target, std::ios::binary);
	if (!file.is_open())
		throw IOException("IOException: Cannot open file!");
	String xzFile = target.append(".xz");
	std::ofstream xz(xzFile, std::ios::binary);
	if (!xz.is_open()) {
		file.close();
		throw IOException("IOException: Cannot create xz file!");
	}

	const auto PRESET = 3;// | LZMA_PRESET_EXTREME;
	lzma_stream strm = LZMA_STREAM_INIT;

	try {
		// the heart of compression
		initEncoder(&strm, PRESET);
		compressionHelper(&strm, file, xz);
	}
	catch (Exception e) {
		DMSG("[- abort -] " << e.what());
	}

	lzma_end(&strm);
	xz.close();
	file.close();
	return xzFile;
}

void XZ::initEncoder(lzma_stream *strm, uint32_t preset) {
	lzma_ret ret = lzma_easy_encoder(strm, preset, LZMA_CHECK_CRC64);
	if (ret == LZMA_OK) return;

	switch (ret) {
	case LZMA_MEM_ERROR:         throw OutOfMemoryException("Memory allocation failed");
	case LZMA_OPTIONS_ERROR:     throw NotSupportedException("Preset is not supported");
	case LZMA_UNSUPPORTED_CHECK: throw NotSupportedException("Integrity check [CRC64] is not supported");
	default:                     throw Exception("Unknown error, possibly a bug");
	}
}

void XZ::compressionHelper(lzma_stream *strm,
	std::ifstream &infile, std::ofstream &outfile) {
	lzma_action action = LZMA_RUN;
	uint8_t inbuf[BUFSIZ];
	uint8_t outbuf[BUFSIZ];

	strm->next_in = NULL;
	strm->avail_in = 0;
	strm->next_out = outbuf;
	strm->avail_out = BUFSIZ;

	lzma_ret ret;
	do {
		if (strm->avail_in == 0 && !infile.eof()) {
			strm->next_in = inbuf;
			infile.read((char *)inbuf, BUFSIZ);
			strm->avail_in = (size_t)infile.gcount();

			if (infile.bad())
				throw IOException("IOException: Read error!");

			if (infile.eof()) action = LZMA_FINISH;
		}

		ret = lzma_code(strm, action);

		if (strm->avail_out == 0 || ret == LZMA_STREAM_END) {
			size_t write_size = BUFSIZ - strm->avail_out;

			outfile.write((char*)outbuf, write_size);
			if (outfile.bad())
				throw IOException("IOException: Write error!");

			strm->next_out = outbuf;
			strm->avail_out = BUFSIZ;
		}

		if (ret != LZMA_OK) {
			switch (ret) {
			case LZMA_STREAM_END: break;
			case LZMA_MEM_ERROR:  throw OutOfMemoryException("Memory allocation failed");
			case LZMA_DATA_ERROR: throw IOException("File size limits exceeded");
			default:              throw Exception("Unknown error, possibly a bug");
			}
		}
	} while(ret != LZMA_STREAM_END);
}

String XZ::decompress(String source) {
	if (source.length() == 0)
		throw ArgumentException("ArgumentException: Target path too short!");
	if (!File::Exists(source))
		throw IOException("IOException: Target does not exist!");

	String destination;
	auto last = source.find_last_of(".xz") - 2;
	if (last == source.length() - 3) {
		destination = source.substr(0, last);
	}
	else {
		destination.assign(source);
		destination.append(".uxz");
	}

	std::ifstream xzFile(source, std::ios::binary);
	if (!xzFile.is_open())
		throw IOException("IOException: Cannot open file!");
	std::ofstream file(destination, std::ios::binary);
	if (!file.is_open()) {
		xzFile.close();
		throw IOException("IOException: Cannot create uncompressed file!");
	}

	lzma_stream strm = LZMA_STREAM_INIT;

	try {
		// the heart of compression
		initDecoder(&strm);
		decompHelper(&strm, source, xzFile, file);
	}
	catch (Exception e) {
		DMSG("[- abort -] " << e.what());
	}

	lzma_end(&strm);
	file.close();
	xzFile.close();
	return destination;
}

void XZ::initDecoder(lzma_stream *strm) {
	lzma_ret ret = lzma_stream_decoder(strm, UINT64_MAX, LZMA_CONCATENATED);

	if (ret == LZMA_OK) return;

	switch (ret) {
	case LZMA_MEM_ERROR:     throw OutOfMemoryException("Memory allocation failed");
	case LZMA_OPTIONS_ERROR: throw NotSupportedException("Unsupported decompressor flags");
	default:                 throw Exception("Unknown error, possibly a bug");
	}
}

void XZ::decompHelper(lzma_stream *strm, String inname,
	std::ifstream &infile, std::ofstream &outfile) {
	lzma_action action = LZMA_RUN;

	uint8_t inbuf[BUFSIZ];
	uint8_t outbuf[BUFSIZ];

	strm->next_in = NULL;
	strm->avail_in = 0;
	strm->next_out = outbuf;
	strm->avail_out = BUFSIZ;

	lzma_ret ret;
	do {
		if (strm->avail_in == 0 && !infile.eof()) {
			strm->next_in = inbuf;
			infile.read((char*)inbuf, BUFSIZ);
			strm->avail_in = infile.gcount();

			if (infile.bad())
				throw IOException("IOException: Read error!");
			
			if (infile.eof()) action = LZMA_FINISH;
		}

		ret = lzma_code(strm, action);

		if (strm->avail_out == 0 || ret == LZMA_STREAM_END) {
			size_t write_size = BUFSIZ - strm->avail_out;

			outfile.write((char*)outbuf, write_size);
			if (outfile.bad())
				throw IOException("IOException: Write error!");

			strm->next_out = outbuf;
			strm->avail_out = BUFSIZ;
		}

		if (ret != LZMA_OK) {
			switch (ret) {
			case LZMA_STREAM_END:    break;
			case LZMA_MEM_ERROR:     throw OutOfMemoryException("Memory allocation failed");
			case LZMA_FORMAT_ERROR:  throw NotSupportedException("The input is not in the .xz format");
			case LZMA_OPTIONS_ERROR: throw NotSupportedException("Unsupported compression options");
			case LZMA_DATA_ERROR:    throw Exception("Compressed file is corrupt");
			case LZMA_BUF_ERROR:     throw Exception("Compressed file is truncated or otherwise corrupt");
			default:                 throw Exception("Unknown error, possibly a bug");
			}
		}
	} while (ret != LZMA_STREAM_END);
}