/**
* Created by TekuConcept on August 9, 2017
*/

#include "File.h"

#include <sys/stat.h>
#include <fstream>

#include "Directory.h"
#include "SystemExceptions.h"

#ifdef _WIN32
#include <windows.h>
#undef GetCurrentDirectory
#else
#include <dirent.h>
#endif

#include <iostream>
#define DMSG(x) std::cerr << x << std::endl

using namespace System;
using namespace IO;

File::File() {}

void File::AppendAllLines(String path, std::vector<String> lines) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	std::ofstream file(path, std::ios_base::app);
	if (!file.is_open())
		throw IOException("IOException: Cannot open or create file!");
	for (auto line : lines) file << line << std::endl;
	file.close();
}

void File::AppendAllLines(String path, String* lines, Int32 length) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	std::ofstream file(path, std::ios_base::app);
	if (!file.is_open())
		throw IOException("IOException: Cannot open or create file!");
	for (int i = 0; i < length; i++) file << lines[i] << std::endl;
	file.close();
}

void File::AppendAllText(String path, String contents) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	std::ofstream file(path, std::ios_base::app);
	if (!file.is_open())
		throw IOException("IOException: Cannot open or create file!");
	file << contents;
	file.close();
}

void File::Copy(String sourceFileName, String destFileName) {
	try {
		Copy(sourceFileName, destFileName, false);
	}
	catch (std::runtime_error e) {
		throw e;
	}
}

void File::Copy(String sourceFileName, String destFileName, Boolean overwrite) {
	if (sourceFileName.length() == 0 || destFileName.length() == 0)
		throw ArgumentException("ArgumentException: File paths too short!");

	if (!Exists(sourceFileName))
		throw IOException("IOException: Source file does not exist!");

	if (!overwrite && Exists(destFileName))
		throw IOException("IOException: Destination file already exists!");

#ifdef _WINDOWS_
	if (CopyFile(sourceFileName.c_str(), destFileName.c_str(), !overwrite) == 0)
		throw IOException("IOException: Could not copy file!");
#else
	std::ofstream dest(destFileName, std::ios::binary);
	if (!dest.is_open())
		throw IOException("IOException: Cannot write to destination!");
	std::ifstream src(sourceFileName, std::ios::binary);
	if (!src.is_open()) {
		dest.close();
		throw IOException("IOException: Cannot read from source!");
	}

	dest << src.rdbuf();

	src.close();
	dest.close();
#endif
}

void File::Delete(String path) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

#ifdef _WINDOWS_
	if (DeleteFile(path.c_str()) == 0)
#else
	if (remove(path.c_str()) != 0)
#endif
		throw IOException("IOException: Cannot delete file!");
}

bool File::Exists(String path) {
	if (path.length() == 0) return false;

	struct stat info;
	if (stat(path.c_str(), &info) != 0) {
		try {
			// try again by prepending current directory
			if (path[0] == '\\' || path[0] == '/') return false;
			String path2 = Directory::SplicePaths(Directory::GetCurrentDirectory(), path);
			if (stat(path2.c_str(), &info) != 0)
				return false;
		}
		catch (...) {
			return false;
		}
	}

	if (info.st_mode & S_IFREG)    return true;
	else                           return false;
}

void File::Move(String sourceFileName, String destFileName) {
	if (sourceFileName.length() == 0 || destFileName.length() == 0)
		throw ArgumentException("ArgumentException: File paths too short!");

	if (!Exists(sourceFileName))
		throw IOException("IOException: Source file does not exist!");

#ifdef _WINDOWS_
	if (MoveFile(sourceFileName.c_str(), destFileName.c_str()) == 0) {
		switch (GetLastError()) {
		case ERROR_ACCESS_DENIED: throw UnauthorizedAccessException("Access denied!");
		case ERROR_BAD_PATHNAME: throw IOException("IOException: Path contains invalid characters!");
		default: throw IOException("IOException: Unable to move file!");
		}
	}
#else
	if (rename(sourceFileName.c_str(), destFileName.c_str()) != 0) {
		switch (errno) {
		case EACCES: throw UnauthorizedAccessException("Access denied!");
		case EINVAL: throw IOException("IOException: Path contains invalid characters!");
		default: throw IOException("IOException: Unable to move file!");
		}
	}
#endif
}

Long File::FileSize(String path) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	struct stat st;
	if (stat(path.c_str(), &st) != 0) {
		throw IOException("IOException: Cannot read file size!");
	}
	return st.st_size;
}

Int32 File::FilePermissions(String path) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

#ifdef _WINDOWS_
	return 0;
#else
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		throw IOException("IOException: Cannot read file size!");
	return (st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
#endif
}

void File::SetFilePermissions(String path, Int32 permissions) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

#ifdef _WINDOWS_
	(void)permissions;
#else
	if (chmod(path.c_str(), permissions) != 0)
		throw IOException("IOException: Failed to set file permissions!");
#endif
}

String File::FileName(String path) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	auto idx = path.find_last_of("\\/");
	if (idx == std::string::npos) return path;
	else return path.substr(idx + 1, std::string::npos);
}

String File::ReadAllBytes(String path) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	String bytes;
	std::ifstream file(path, std::ios::ate | std::ios::binary);
	if (!file.is_open())
		throw IOException("IOException: Cannot read from file!");
	auto length = file.tellg();

	bytes.resize((unsigned int)length);
	file.seekg(0, std::ios::beg);
	file.read(&bytes[0], length);

	file.close();
	return bytes;
}

std::vector<String> File::ReadAllLines(String path) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	std::vector<String> lines;
	std::ifstream file(path);
	if (!file.is_open())
		throw IOException("IOException: Cannot read from file!");

	String line;
	while (getline(file, line)) {
		lines.push_back(line);
	}

	file.close();
	return lines;
}

String File::ReadAllText(String path) {
	try {
		return ReadAllBytes(path);
	}
	catch (std::runtime_error e) {
		throw e;
	}
}

void File::WriteAllBytes(String path, String bytes) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	std::ofstream file(path, std::ios::binary);
	if (!file.is_open())
		throw IOException("IOException: Cannot write to file!");
	file.write(&bytes[0], bytes.length());
	file.close();
}

void File::WriteAllLines(String path, String* lines, Int32 length) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	std::ofstream file(path, std::ios_base::app);
	if (!file.is_open())
		throw IOException("IOException: Cannot open or create file!");
	for (int i = 0; i < length; i++) file << lines[i] << std::endl;
	file.close();
}

void File::WriteAllLines(String path, std::vector<String> lines) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	std::ofstream file(path);
	if (!file.is_open())
		throw IOException("IOException: Cannot open or create file!");
	for (auto line : lines) file << line << std::endl;
	file.close();
}

void File::WriteAllText(String path, String contents) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: Path too short!");

	std::ofstream file(path);
	if (!file.is_open())
		throw IOException("IOException: Cannot open or create file!");
	file << contents;
	file.close();
}