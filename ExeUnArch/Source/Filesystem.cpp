/**
* Created by TekuConcept on August 8, 2017
*/

#include "Filesystem.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <exception>
#include <iostream>

#include <stdio.h>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <strsafe.h>
#include <direct.h>
#else
#include <dirent.h>
#endif

Filesystem::Filesystem() {}

bool Filesystem::directoryExists(std::string path) {
	if (path.length() == 0) return true;
	PathType type = determinePathType(path);
	return type == DIRECTORY;
}

bool Filesystem::isDirectory(std::string path) {
	return determinePathType(path) == PathType::DIRECTORY;
}

bool Filesystem::isFile(std::string path) {
	return determinePathType(path) == PathType::FILE;
}

Filesystem::PathType Filesystem::determinePathType(std::string path) {
	struct stat info;
	if (stat(path.c_str(), &info) != 0) return PathType::PERROR;
	else if (info.st_mode & S_IFDIR)    return PathType::DIRECTORY;
	else if (info.st_mode & S_IFREG)    return PathType::FILE;
	else                                return PathType::UNKNOWN;
}

vstring Filesystem::findFiles(std::string root, OpMode mode) {
	vstring collection;
	getFilesHelper(root, collection, mode);
	return collection;
}

bool Filesystem::shouldIgnoreName(std::string name) {
	if (name.length() <= 2) {
		if (name.length() == 0 || name == ".." || name == ".")
			return true;
	}
	return false;
}

std::string Filesystem::splicePaths(std::string prefix, std::string suffix) {
	if (prefix.length() == 0) return suffix;

	std::string result(prefix);
	if (suffix.length() != 0 && suffix[0] != '/') {
		if (prefix[prefix.length() - 1] != '/')
			result.append("/");
		result.append(suffix);
	}

	return result;
}

std::string Filesystem::pathPrefix(std::string path) {
	std::size_t found = path.find_last_of("/\\");
	if (found == std::string::npos) return "";
	return path.substr(0, found);
}

void Filesystem::getFilesHelper(std::string root, vstring& cache, OpMode mode) {
#ifdef _WIN32
	TCHAR szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;
	size_t length_of_arg;

	StringCchLength(root.c_str(), MAX_PATH, &length_of_arg);
	if (length_of_arg > (MAX_PATH - 3)) return; // path too long

	StringCchCopy(szDir, MAX_PATH, root.c_str());
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(szDir, &ffd);
	if (INVALID_HANDLE_VALUE == hFind) return; // cannot access files
	do {
		std::string path, name(ffd.cFileName);
		path = splicePaths(root, name);

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (!shouldIgnoreName(name)) {
				cache.push_back(path);

				if (mode == RECURSIVE && isDirectory(path))
					getFilesHelper(path, cache, mode);
			}
		}
		else cache.push_back(path); // is file
	} while (FindNextFile(hFind, &ffd) != 0);

	if (GetLastError() != ERROR_NO_MORE_FILES) {
		std::cout << "An error occured while scanning..." << std::endl;
	}

	FindClose(hFind);
#else
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(root.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string path, name(ent->d_name);
			path = splicePaths(root, name);

			if (!shouldIgnoreName(name)) {
				cache.push_back(path);

				if (mode == RECURSIVE && isDirectory(path))
					getFilesHelper(path, cache, mode);
			}
		}
		closedir(dir);
	}
	else {
#ifdef VERBOSE
		std::cerr << "getFiles(): Could Not Open Directory '";
		std::cerr << root << "'";
		std::cerr << std::endl;
#endif
		return;
	}
#endif
}

bool Filesystem::deletePath(std::string path) {
	PathType type = determinePathType(path);
#ifdef _WIN32
	if (type == DIRECTORY) return RemoveDirectory(path.c_str());
	else if (type == FILE) return DeleteFile(path.c_str());
#else
	if (type == DIRECTORY) return (remove(path.c_str()) == 0); // can only delete empty directories
	else if (type == FILE) return (remove(path.c_str()) == 0);
#endif
	else                   return false;
}

bool Filesystem::directoryHelper(std::string path) {
	std::string prefix = pathPrefix(path);

	if (!directoryExists(prefix)) {
		directoryHelper(prefix);
		return createDirectory(prefix, NORMAL);
	}
	else return true;
}

bool Filesystem::createDirectory(std::string path, OpMode mode) {
	if (mode == RECURSIVE)
		if (!directoryHelper(path))
			return false;
#ifdef _WIN32
	int ret = _mkdir(path.c_str());
#else
	mode_t dirmode = 0755;
	int ret = mkdir(path.c_str(), dirmode);
#endif
	return (ret == 0 || ret == EEXIST);
}