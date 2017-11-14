/**
* Created by TekuConcept on August 8, 2017
*/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

typedef std::vector<std::string> vstring;

class Filesystem {
public:
	typedef enum PathType {
		FILE,
		DIRECTORY,
		UNKNOWN,
		PERROR
	} PathType;

	typedef enum OpMode {
		RECURSIVE,
		NORMAL
	} OpMode;

	static bool directoryExists(std::string path);
	static bool isDirectory(std::string path);
	static bool isFile(std::string path);
	static PathType determinePathType(std::string path);

	static vstring findFiles(std::string root, OpMode mode = NORMAL);

	static std::string splicePaths(std::string prefix, std::string suffix);
	static std::string pathPrefix(std::string path);

	static bool deletePath(std::string path);
	static bool createDirectory(std::string path, OpMode mode = NORMAL);

private:
	Filesystem();

	static void getFilesHelper(std::string root, vstring& cache, OpMode mode);
	static bool shouldIgnoreName(std::string name);
	static bool directoryHelper(std::string name);
};

#endif