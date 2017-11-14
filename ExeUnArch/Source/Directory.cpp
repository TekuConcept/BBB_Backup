/**
* Created by TekuConcept on August 9, 2017
*/

#include "Directory.h"
#include <iostream>

#include <vector>
#include <sys/stat.h>
#include <stdio.h>
#include "SystemExceptions.h"

#ifdef _WIN32
#include <windows.h>
#include <strsafe.h>
#include <direct.h>
#undef GetCurrentDirectory
#else
#include <dirent.h>
#include <ftw.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#endif

#include <iostream>
#define DMSG(x) std::cerr << x << std::endl

using namespace System;
using namespace IO;

Directory::Directory() {}

void Directory::Create(String path) {
	if (path.length() == 0)
		throw ArgumentException("ArgumentException: path is too short!");
#ifdef _WINDOWS_
	if (path.length() > MAX_PATH)
		throw PathTooLongException();
#endif

	auto index = path.find_first_of("\\/");

	String root;
	while (index != std::string::npos) {
		root = path.substr(0, index + 1);
		if (!Exists(root)) {
#ifdef _WINDOWS_
			if (CreateDirectory(root.c_str(), NULL) == 0)
#else
			if (mkdir(root.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
#endif
			{
				throw IOException("IOException: Failed to create directory tree!");
			}
		}
		index = path.find_first_of("\\/", index + 1);
	}

#ifdef _WINDOWS_
	if (CreateDirectory(path.c_str(), NULL) == 0)
#else
	if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
#endif
		throw IOException("IOException: Failed to create directory!");
}

void Directory::Delete(String path) {
	try {
		Delete(path, false);
	}
	catch (std::runtime_error e) {
		throw e;
	}
}

String Directory::SplicePaths(String prefix, String suffix) {
	if (prefix.length() == 0) return suffix;

	std::string result(prefix);
	if (suffix.length() != 0) {
		if (prefix[prefix.length()-1] == '/') {
			if (suffix[0] == '/') result.append(suffix.substr(1));
			else result.append(suffix);
		}
		else {
			if (suffix[0] != '/') result.append("/");
			result.append(suffix);
		}
	}

	return result;
}

bool pathIsDots(std::string name) {
	if (name.length() <= 2) {
		if (name.length() == 0 || name == ".." || name == ".")
			return true;
	}
	return false;
}

void Directory::Delete(String path, Boolean recursive) {
	if (path.length() == 0 || path.find_first_not_of(' ') == std::string::npos)
		throw ArgumentException("ArgumentException: String must be a valid path!");
	if (Exists(path)) {
		if (recursive) {
#ifdef _WINDOWS_
			std::vector<String> directories;
			directories.push_back(path);

			TCHAR szDir[MAX_PATH];
			HANDLE hFind;
			WIN32_FIND_DATA ffd;
			size_t length_of_arg;

			while (directories.size() != 0) {
				String currentPath = directories.back();
				std::cerr << "Next: " << currentPath << std::endl;

				StringCchLength(currentPath.c_str(), MAX_PATH, &length_of_arg);
				if (length_of_arg > (MAX_PATH - 3)) {
					std::cerr << "Path too long: " << currentPath << std::endl;
					continue;
				}
				StringCchCopy(szDir, MAX_PATH, currentPath.c_str());
				StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

				hFind = FindFirstFile(szDir, &ffd);
				if (INVALID_HANDLE_VALUE == hFind) {
					std::cerr << "Cannot access file: " << currentPath << std::endl;
					continue;
				}

				do {
					std::string tempPath, name(ffd.cFileName);
					tempPath = SplicePaths(currentPath, name);
					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						if (!pathIsDots(name))
							directories.push_back(tempPath);
						else DeleteFile(tempPath.c_str()); // is file
				} while (FindNextFile(hFind, &ffd) != 0);

				if (GetLastError() != ERROR_NO_MORE_FILES)
					throw IOException("IOException: An error occured while recursively deleting!");
				FindClose(hFind);

				if (directories.back() == currentPath) {
					RemoveDirectory(currentPath.c_str());
					directories.pop_back();
				}
			}
#else
			if (nftw(path.c_str(), [](const char *pathname, const struct stat *sbuf,
				int type, struct FTW *ftwb) {
				if (remove(pathname) < 0) return -1;
			}, 10, FTW_DEPTH | FTW_MOUNT | FTW_PHYS) < 0) {
				throw IOException("IOException: Recursive delete failed!");
			}
#endif
		}
		else {
#ifdef _WINDOWS_
			if (!RemoveDirectory(path.c_str()))
#else
			if (remove(path.c_str()) != 0)
#endif
				throw IOException("IOException: Delete failed");
		}
	}
	else throw DirectoryNotFoundException();
}

bool Directory::Exists(String path) {
	if (path.length() == 0) return false;

	struct stat info;
	if (stat(path.c_str(), &info) != 0) return false;
	else if (info.st_mode & S_IFDIR)    return true;
	else                                return false;
}

String ReplaceString(String subject, const String& search, const String& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != String::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

String Directory::GetCurrentDirectory() {
	char buff[FILENAME_MAX];
#ifdef _WIN32
	_getcwd(buff, FILENAME_MAX);
#else
	getcwd(buff, FILENAME_MAX);
#endif
	String currentWorkingDir(buff);
#ifdef _WIN32
	currentWorkingDir = NormalizeDelimiters(currentWorkingDir);
#endif
	return currentWorkingDir;
}

std::vector<String> Directory::GetDirectories(String path) {
	try {
		return GetAllPaths(path).first;
	}
	catch (std::runtime_error e) {
		throw e;
	}
}

std::vector<String> Directory::GetFiles(String path) {
	try {
		return GetAllPaths(path).second;
	}
	catch (std::runtime_error e) {
		throw e;
	}
}

Directory::FolderFileCollectionPair Directory::GetAllPaths(String path) {
	FolderFileCollectionPair pair;
	std::vector<String> directories;
	std::vector<String> files;

#ifdef _WINDOWS_
	TCHAR szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;
	size_t length_of_arg;

	StringCchLength(path.c_str(), MAX_PATH, &length_of_arg);
	if (length_of_arg > (MAX_PATH - 3))
		throw PathTooLongException();

	StringCchCopy(szDir, MAX_PATH, path.c_str());
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(szDir, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
		throw IOException("IOException: Cannot access files!");
	do {
		std::string tempPath, name(ffd.cFileName);
		tempPath = SplicePaths(path, name);

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (!pathIsDots(name)) directories.push_back(tempPath);
		}
		else files.push_back(tempPath);
	} while (FindNextFile(hFind, &ffd) != 0);

	if (GetLastError() != ERROR_NO_MORE_FILES)
		throw IOException("IOException: An error occured while scanning!");

	FindClose(hFind);
#else
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string tempPath, name(ent->d_name);
			tempPath = SplicePaths(path, name);

			if (!pathIsDots(name)) {
				struct stat info;
				if (stat(tempPath.c_str(), &info) == 0) {
					if (info.st_mode & S_IFDIR)
						directories.push_back(tempPath);
					else if (info.st_mode & S_IFREG)
						files.push_back(tempPath);
				}
			}
		}
		closedir(dir);
	}
	else throw IOException("IOException: An error occured while scanning!");
#endif

	pair.first = directories;
	pair.second = files;
	return pair;
}

String Directory::GetDirectoryRoot(String path) {
	if (path.length() == 0) throw ArgumentException("ArgumentException: Path too short!");

#ifdef _WINDOWS_
	TCHAR  buffer[FILENAME_MAX] = TEXT("");
	TCHAR** lppPart = { NULL };
	if (GetFullPathName(path.c_str(), FILENAME_MAX, buffer, lppPart) == 0)
		return "C:\\";
	else {
		String res(buffer);
		auto index = res.find_first_of("\\");
		return res.substr(0, index + 1);
	}
#else
	(void)path;
	return "/";
#endif
}

String Directory::GetParent(String path) {
	String res = GetAbsolutePath(path);

	if (path.length() == 0) return res;
	else {
		auto index = res.find_last_of("/");
		return res.substr(0, index);
	}
}

String getAbsManualEval(String path, bool &success) {
	success = true;
	String sym = "", absolute;

#ifdef _WINDOWS_
	if (path[0] == ':') {
		char driveLetter = Directory::GetCurrentDirectory()[0];
		sym.append(&driveLetter, 1);
		sym.append(path);
	}
	else sym = Directory::SplicePaths(Directory::GetCurrentDirectory(), path);
#else
	if (path.find_first_of("/") != 0)
		sym = Directory::SplicePaths(Directory::GetCurrentDirectory(), path);
	else sym = path;
#endif

	unsigned int start = sym.length() - path.length();
	absolute.append(sym.c_str(), start);

	unsigned char dotCount = 0;
	for (unsigned int i = start; i < sym.length(); i++) {
		if (sym[i] == '.') {
			dotCount++;
			absolute.append(&sym[i], 1);
			// '.' - ignore
			// '..' - backup
			// '..+' - do nothing
		}
		else if (sym[i] == '/') {
			if (dotCount == 1) {
				absolute = absolute.substr(0, absolute.length() - 2);
			}
			else if (dotCount == 2) {
				auto idx = absolute.find_last_of("/"); // "/.."
				idx = absolute.find_last_of("/", idx - 1); // "/parent/.."
				absolute = absolute.substr(0, idx);
			}

			absolute.append(&sym[i], 1);
			dotCount = 0;
		}
		else absolute.append(&sym[i], 1);
	}

	return absolute;
}

String Directory::GetAbsolutePath(String path) {
	if (path.length() == 0) return GetCurrentDirectory();
	else {
		String res;
#ifdef _WINDOWS_
		TCHAR buffer[FILENAME_MAX] = TEXT("");
		TCHAR** lppPart = { NULL };
		if (GetFullPathName(path.c_str(), FILENAME_MAX, buffer, lppPart) == 0)
#else
		char buffer[4096];
		if (realpath(path.c_str(), buffer) == NULL)
#endif
		{
			// try evaluating non-existant path
			bool test;
#ifdef _WINDOWS_
			path = ReplaceString(path, "\\", "/");
#endif
			res.assign(getAbsManualEval(path, test));
			if (!test) throw std::runtime_error("Cannot resolve path name!");
		}
		else {
			res.assign(buffer);
#ifdef _WINDOWS_
			res = ReplaceString(res, "\\", "/");
#endif
		}
		return res;
	}
}

void Directory::Move(String sourceDirName, String destDirName) {
	if (sourceDirName.length() == 0 || destDirName.length() == 0)
		throw ArgumentException("ArgumentException: Paths cannot have 0 length.");
	if (!Exists(sourceDirName))
		throw IOException("IOException: Source directory doesn't exist.");
	if (sourceDirName == destDirName)
		throw IOException("IOException: Paths are the same.");

	if (Exists(destDirName)) {
		try { Delete(destDirName); }
		catch (std::runtime_error e) {
			throw IOException("IOException: Overwriting directory failed!");
		}
	}

	String sourceParent = GetParent(sourceDirName);
	String destParent = GetParent(destDirName);
	if (sourceParent != destParent) {
		if (!Exists(destParent)) {
			try { Create(destParent); }
			catch (std::runtime_error e) {
				throw IOException("IOException: Failed to create destination tree!");
			}
		}

#ifdef _WINDOWS_
		if (!MoveFileEx(sourceDirName.c_str(), destDirName.c_str(), MOVEFILE_WRITE_THROUGH)) {
			switch (GetLastError()) {
			case ERROR_ACCESS_DENIED: throw UnauthorizedAccessException("Access denied!");
			case ERROR_BAD_PATHNAME: throw IOException("IOException: Path contains invalid characters!");
			default: throw IOException("IOException: Unable to move directory!");
			}
		}
		return;
#endif
	}

	if (rename(sourceDirName.c_str(), destDirName.c_str()) != 0) {
		switch (errno) {
		case EACCES: throw UnauthorizedAccessException("Access denied!");
		case EINVAL: throw IOException("IOException: Path contains invalid characters!");
		default: throw IOException("IOException: Unable to move directory!");
		}
	}
}

String Directory::NormalizeDelimiters(String path) {
	return ReplaceString(path, "\\", "/");
}