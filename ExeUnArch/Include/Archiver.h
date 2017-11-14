/**
 * Created by TekuConcept on August 9, 2017
 */

#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <string>
#include <fstream>
#include "cstypes.h"

class Archiver {
private:
	Archiver();

	static bool readHeader(std::ifstream &tar);
	static bool readFooter(std::ifstream &tar);

	static void writeHeader(std::ofstream &tar);
	static void writeFooter(std::ofstream &tar);

	static bool readDirectory(std::ifstream &tar, String dest);
	static bool readFile(std::ifstream &tar, String dest);

	static void writeDirectory(String path, std::ofstream &tar);
	static void writeFile(String file, String name, std::ofstream &tar);

	static void archiveTree(String root, std::ofstream &tar);

public:
	static void archive(String target, String tarPath);
	static void unarchive(String tarPath, String destination);
};

#endif