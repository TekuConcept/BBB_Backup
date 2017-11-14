/**
* Created by TekuConcept on August 9, 2017
*/

#include "Archiver.h"
#include "File.h"
#include "Directory.h"
#include "SystemExceptions.h"
#include <sys/stat.h>

#include <iostream>
#define DMSG(x) std::cout << x << std::endl

#define BUF_SIZE 512

using namespace System;
using namespace IO;

Archiver::Archiver() {}

bool Archiver::readHeader(std::ifstream &tar) {
	String header(8, '\0');
	const char HEADER[] = "ARCHIVE\0";
	if (tar.read(&header[0], header.length())) {
		for (unsigned char i = 0; i < header.length(); i++)
			if (header[i] != HEADER[i]) return false;
		return true;
	}
	return false;
}

bool Archiver::readFooter(std::ifstream &tar) {
	String footer(12, '\0');
	const char FOOTER[] = "END ARCHIVE\0";
	if (tar.read(&footer[0], footer.length())) {
		for (unsigned char i = 0; i < footer.length(); i++)
			if (footer[i] != FOOTER[i]) return false;
		return true;
	}
	return false;
}

void Archiver::writeHeader(std::ofstream &tar) {
	tar.write("ARCHIVE\0", 8);
}

void Archiver::writeFooter(std::ofstream &tar) {
	tar.write("\0", 1); // pad footer from data
	tar.write("END ARCHIVE\0", 12);
}

bool Archiver::readDirectory(std::ifstream &tar, String dest) {
	short length = 0;
	if (!tar.read((char*)&length, sizeof(short))) return false;
	String name(length, '\0');
	if (!tar.read(&name[0], length)) return false;
	try {
		String dirName = Directory::SplicePaths(dest, name);
		Directory::Create(dirName);
		// DMSG("Directory: " << dirName);
		// DMSG("\t- Name Length: " << length);
	}
	catch (Exception e) {
		DMSG("[- abort -] " << e.what());
	}
	return true;
}

bool Archiver::readFile(std::ifstream &tar, String dest) {
	short length = 0;
	int permissions = 0;
	if (!tar.read((char*)&permissions, sizeof(int))) return false;
	if (!tar.read((char*)&length, sizeof(short))) return false;
	String name(length, '\0');
	if (!tar.read(&name[0], length)) return false;
	Long fileLen = 0;
	if (!tar.read((char*)&fileLen, sizeof(Long))) return false;
	// DMSG("File: " << name);
	// DMSG("\t- Name Length: " << std::hex << length);
	// DMSG("\t- Size: " << std::hex << fileLen);

	String fileName = Directory::SplicePaths(dest, name);
	std::ofstream file(fileName, std::ios::binary);
	if (!file.is_open()) {
		tar.seekg(fileLen, std::ios::cur);
		DMSG("[- abort -] IOException: Cannot write to file " << name);
	}
	else {
		char buffer[BUF_SIZE];
		Long processed = 0;
		while ((processed + BUF_SIZE < fileLen) &&
			tar.read(buffer, BUF_SIZE)) {
			file.write(buffer, BUF_SIZE);
			processed += BUF_SIZE;
		}
		Long rem = fileLen - processed;
		if (rem != 0) {
			tar.read(buffer, rem);
			file.write(buffer, rem);
		}

		file.close();
		try { File::SetFilePermissions(fileName, permissions); }
		catch (Exception e) { return false; }
	}
	return true;
}

void Archiver::writeDirectory(String path, std::ofstream &tar) {
	unsigned short pathLen = (unsigned short)path.length();

	tar.write("D", 1);
	tar.write((char*)(&pathLen), sizeof(short));
	tar.write(&path[0], path.length());
}

void Archiver::writeFile(String file, String name, std::ofstream &tar) {
	std::ifstream src(file, std::ios::binary);
	if (!src.is_open()) {
		DMSG("[- exclude -] Cannot open file for reading!");
		return;
	}

	Long size = 0;
	try { size = File::FileSize(file); }
	catch (IOException e) {
		DMSG("[- abort -] Cannot get file size: " << e.what());
	}

	unsigned short nameLen = (unsigned short)name.length();

	int permissions = 0;
	try { permissions = File::FilePermissions(file); }
	catch (IOException) {
#ifndef _WIN32
		permissions = S_IRWXU;
#endif
	}

	tar.write("F", 1);
	tar.write((const char*)&permissions, sizeof(int));
	tar.write((const char*)(&nameLen), sizeof(short));
	tar.write(&name[0], nameLen);
	tar.write((const char*)(&size), sizeof(Long));

	char buffer[BUF_SIZE];
	Long processed = 0;
	while (src.read(buffer, BUF_SIZE)) {
		tar.write(buffer, BUF_SIZE);
		processed += BUF_SIZE;
	}
	Long rem = size - processed;
	if (rem != 0) tar.write(buffer, rem);

	src.close();
}

void Archiver::archiveTree(String root, std::ofstream &tar) {
	String absPath = Directory::GetAbsolutePath(root);
	auto start = absPath.length();

	std::vector<String> dirQueue;
	dirQueue.push_back(absPath);
	while (dirQueue.size()) {
		String next = dirQueue.back();
		dirQueue.pop_back();

		auto collection = Directory::GetAllPaths(next);

		for (auto dir : collection.first) {
			writeDirectory(dir.substr(start), tar);
			dirQueue.push_back(dir);
		}

		for (auto file : collection.second) {
			writeFile(file, file.substr(start), tar);
		}
	}
}

void Archiver::archive(String target, String tarPath) {
	if (target.length() == 0 || tarPath.length() == 0)
		throw ArgumentException("ArgumentException: Paths too short!");

	std::ofstream tar(tarPath, std::ios::binary);
	if (!tar.is_open())
		throw IOException("IOException: Cannot open archive file.");
	writeHeader(tar);

	if (File::Exists(target)) { writeFile(target, File::FileName(target), tar); }
	else if (Directory::Exists(target)) { archiveTree(target, tar); }
	else {
		tar.close();
		throw IOException("IOException: Target does not exist!");
	}

	writeFooter(tar);
	tar.close();
}

void Archiver::unarchive(String tarPath, String destination) {
	if (tarPath.length() == 0)
		throw ArgumentException("ArgumentException: Paths too short!");
	if (!File::Exists(tarPath))
		throw IOException("IOException: Archive file does not exist!");
	if (destination.length() == 0)
		destination = Directory::GetCurrentDirectory();

	std::ifstream tar(tarPath, std::ios::binary);
	if (!tar.is_open())
		throw IOException("IOException: Cannot open archive file!");
	if (!readHeader(tar)) {
		tar.close();
		throw std::runtime_error("Archive Corrupted: Header");
	}

	if (!Directory::Exists(destination)) {
		try { Directory::Create(destination); }
		catch (std::runtime_error e) { throw e; }
	}

	char id;
	bool success = true;
	while (tar.read(&id, 1)) {
		if (id == '\0') break;
		else if (id == 'D') success = readDirectory(tar, destination);
		else if (id == 'F') success = readFile(tar, destination);
		else success = false;
		if (!success) {
			DMSG("ID: " << std::hex << (int)id);
			break;
		}
	}

	if (!success || !readFooter(tar)) {
		tar.close();
		if (!success)
			throw Exception("Archive Corrupted: Bad File Tree");
		else throw Exception("Archive Corrupted: Footer");
	}
	tar.close();
}