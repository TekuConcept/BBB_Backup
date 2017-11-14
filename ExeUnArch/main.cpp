//
// Created by TekuConcept on August 8, 2017
//
// My quick 2-3 day attempt at writing a self-extracting archiver program.
// This program can be armed by sending it a target folder patch to archive.
// When the "armed" program is executed, it will extract itself within the
// current directory.
//
// The program/file format is |EXE|Archive|Footer|
// The archive format is XZ(myTAR(Folder|Files))
// The footer provides meta information such as archive length and date setup.
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/stat.h>

#define DMSG(x) std::cout << x << std::endl
#include "SystemExceptions.h"
#include "Directory.h"
#include "File.h"
#include "Archiver.h"
#include "XZ.h"

using namespace System;
using namespace IO;

// Archive Metadata Info
typedef struct LoadInfo {
	unsigned char major;
	unsigned char minor;
	unsigned short patch;
	size_t fileIndex;
	size_t footIndex;
	String targetName;
	bool hasApp;
	String appPath;
} LoadInfo;

//
// Get the complete absolute patch to the current executable
//
String getProgramName(const char* arg1) {
	String exeName(arg1);
	return Directory::GetAbsolutePath(exeName);
}

//
// Attempts to read in and process the footer meta data.
// If the footer fails to parse, this function will return false.
// The footer will fail to parse if a) the program is unarmed or b)
// the footer is corrupted.
//
bool readFooterHelper(std::ifstream &file, LoadInfo &info) {
	size_t startIdx = 0, namelen = 0, applen = 0;
	String tag(8, '\0');

	file.seekg(0, std::ios::end);
	auto fileLen = file.tellg();
	auto footTagIdx = 8 + sizeof(size_t);

	file.seekg(fileLen - (std::streamoff)footTagIdx, std::ios::beg);

	// read footer start index
	if (!file.read((char*)&info.footIndex, sizeof(size_t))) return false;
	// read footer tag
	if (!file.read(&tag[0], 8) || tag != "FOOT_TAG") return false;
	file.seekg(fileLen - (std::streamoff)(info.footIndex), std::ios::beg);

	// read version
	if (!file.read((char*)&info.major, sizeof(unsigned char)) ||
		!file.read((char*)&info.minor, sizeof(unsigned char)) ||
		!file.read((char*)&info.patch, sizeof(unsigned short))) return false;

	// read load file index and then target name length
	if (!file.read((char*)&info.fileIndex, sizeof(size_t)) ||
		!file.read((char*)&namelen, sizeof(size_t))) return false;
	info.targetName.resize(namelen);
	// read target name and then a flag for the app name
	if (!file.read(&info.targetName[0], namelen) ||
		!file.read((char*)&info.hasApp, sizeof(bool))) return false;

	if (info.hasApp) {
		// read app path size and app path name
		if (!file.read((char*)&applen, sizeof(size_t))) return false;
		info.appPath.resize(applen);
		if (!file.read(&info.appPath[0], applen)) return false;
	}
	return true;
}

//
// Wrapper around "readFooterHelper"
// First attempts to open the file before trying to parse.
//
LoadInfo readFooter(String app) {
	std::ifstream file(app, std::ios::binary | std::ios::in);
	if (!file.is_open())
		throw IOException("IOException: Cannot open self!");

	LoadInfo info;
	if (!readFooterHelper(file, info)) {
		file.close();
		throw Exception("Corrupted footer: Reading info!");
	}

	file.close();
	return info;
}

//
// Writes all archive metadata to a new footer.
// This function will only be called if the program is unarmed.
//
void writeFooter(std::ofstream &file, LoadInfo info) {
	size_t length =
		2 * sizeof(unsigned char) +
		sizeof(unsigned short) +
		3 * sizeof(size_t) +
		sizeof(bool) +
		info.targetName.length() +
		8;

	size_t tnlen = info.targetName.length();
	file.write((char*)&info.major, sizeof(unsigned char));
	file.write((char*)&info.minor, sizeof(unsigned char));
	file.write((char*)&info.patch, sizeof(unsigned short));
	file.write((char*)&info.fileIndex, sizeof(size_t));
	file.write((char*)&tnlen, sizeof(size_t));
	file.write(info.targetName.c_str(), info.targetName.length());
	file.write((char*)&info.hasApp, sizeof(bool));
	if (info.hasApp) {
		length += sizeof(size_t) + info.appPath.length();
		size_t aplen = info.appPath.length();
		file.write((char*)&aplen, sizeof(size_t));
		file.write(info.appPath.c_str(), info.appPath.length());
	}
	file.write((char*)&length, sizeof(size_t));
	file.write("FOOT_TAG", 8);
}

//
// Unlike Linux, Windows and some other systems will not allow
// modifying currently execting files. However, they will allow
// copying those files, so we'll just create a new copy of this
// program and arm that.
//
bool copyApp(String app, std::ofstream &dest) {
	std::ifstream source(app, std::ios::binary);
	if (!source.is_open()) return false;
	dest << source.rdbuf();
	source.close();
	return true;
}

//
// Arms the new executable with the target directory.
// The directory will be squished into a single tape-archive
// and then is compressed with the XZ library before being
// written to the new executable.
//
void armWrite(String app, LoadInfo info) {
	String newapp = File::FileName(info.targetName);
	if (newapp == app) newapp.append("_Copy");
#ifdef _WIN32
	newapp.append(".exe");
#else
	newapp.append(".run");
#endif

	DMSG("New App Name: " << newapp);
	std::ofstream dest(newapp, std::ios::binary);
	if (!dest.is_open())
		throw IOException("IOException: Cannot create new executable!");

	if (!copyApp(app, dest)) {
		dest.close();
		throw IOException("IOException: Failed to create new executable!");
	}

	try {
		String arcName = app;
		arcName.append(".arc");
		Archiver::archive(info.targetName, arcName);
		String xzFile = XZ::compress(arcName);
		File::Delete(arcName);

		std::ifstream xz(xzFile, std::ios::binary);
		if (!xz.is_open())
			throw IOException("IOException: Cannot open target cache!");
		dest << xz.rdbuf();
		xz.close();
		File::Delete(xzFile);

		info.targetName = File::FileName(info.targetName);
		writeFooter(dest, info);
	}
	catch (Exception e) {
		throw e;
	}

	dest.close();
#ifndef _WIN32
	try { File::SetFilePermissions(newapp, S_IRWXU); }
	catch (Exception e) { throw e; }
#endif
}

//
// Arming function wrapper. This interface gathers all the necessary
// information (or metadata) for writing, archiving, and unarchiving.
//
void arm(String app) {
	LoadInfo info;
	info.fileIndex = File::FileSize(app);

	unsigned int x;
	std::cout << "Major [0-255]: ";
	do {
		std::cin >> x;
		if (x > 255) std::cout << "> ";
	} while (x > 255);
	info.major = x;

	std::cout << "Minor [0-255]: ";
	do {
		std::cin >> x;
		if (x > 255) std::cout << "> ";
	} while (x > 255);
	info.minor = x;

	std::cout << "Patch ID [0-65535]: ";
	do {
		std::cin >> x;
		if (x > 65535) std::cout << "> ";
	} while (x > 255);
	info.patch = x;
	std::cout << std::endl;

	bool pass = false;
	std::cout << "Target directory to unpack: ";
	String test;
	do {
		std::getline(std::cin, test);
		if (test.length() == 0) continue;
		pass = Directory::Exists(test);
		if (!pass) {
			DMSG("Cannot find target directory!");
		}
	} while (!pass);
	info.targetName = Directory::NormalizeDelimiters(test);
	info.hasApp = false;

	/*char c;
	std::cout << "Run target program after unpack? [y/n] ";
	String answers = "ynYN";
	do {
	c = std::cin.get();
	if (c == 'y' || c == 'Y') info.hasApp = true;
	} while (answers.find(c) == std::string::npos);

	if (info.hasApp) {
	std::cout << "Target program name: ";
	pass = false;
	do {
	std::getline(std::cin, info.appPath);
	if (info.appPath.length() == 0) continue;
	String test = Directory::NormalizeDelimiters(
	Directory::SplicePaths(info.targetName, info.appPath));
	pass = File::Exists(test);
	if (!pass) {
	DMSG("Cannot find target program!");
	}
	} while (!pass);
	}*/

	DMSG("Arming with:");
	DMSG("\t- Version: " << (int)info.major << "." << (int)info.minor << "." << info.patch);
	DMSG("\t- Target: " << info.targetName);
	if (info.hasApp) DMSG("\t- Program: " << info.appPath);

	armWrite(app, info);
}

//
// Unload (or unarchive/extract) this executable into the current directory.
//
void unload(String app, LoadInfo info) {
	std::ifstream file(app, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		throw IOException("IOException: Cannot open self.");

	auto fileLen = (size_t)file.tellg();
	fileLen -= info.footIndex;
	fileLen -= info.fileIndex;

	String xzFile = app;
	xzFile.append(".arc.xz");
	std::ofstream xz(xzFile, std::ios::binary);
	if (!xz.is_open()) {
		file.close();
		throw IOException("IOException: Cannot create cache!");
	}

	file.seekg(info.fileIndex);
	char buffer[BUFSIZ];
	size_t processed = 0;
	while ((processed + BUFSIZ < fileLen) &&
		file.read(buffer, BUFSIZ)) {
		xz.write(buffer, BUFSIZ);
		processed += BUFSIZ;
	}
	Long rem = fileLen - processed;
	if (rem != 0) {
		file.read(buffer, rem);
		xz.write(buffer, rem);
	}

	xz.close();
	file.close();

	String archive = XZ::decompress(xzFile);
	Archiver::unarchive(archive, info.targetName);
	File::Delete(xzFile);
	File::Delete(archive);

	if (info.hasApp) {
		// TODO attemp to run program
	}
}

void printUsage(String app) {
	DMSG("Usage: ");
	DMSG("\tStandalone:\t\"" << app << "\"");
	DMSG("\tArming:\t\"" << app << " -arm\"");
	exit(1);
}

int main(int argc, char* argv[]) {
	String app = getProgramName(argv[0]);

	// first read in footer info
	LoadInfo info;
	bool armed = false;
	try {
		info = readFooter(app);
		armed = true;
	}
	catch (Exception e) {
		armed = false;
		DMSG(e.what());
		if (argc == 1) {
			DMSG("[- Program Unarmed -]");
			exit(0);
		}
	}

	if (argc > 2) printUsage(app);
	else if (argc == 2) {
		String arg2(argv[1]);
		if (arg2 != "-arm") printUsage(app);

		if (armed) {
			char c;
			DMSG("Armed: ");
			DMSG("\t- Version: " << (int)info.major << "." << (int)info.minor << "." << info.patch);
			DMSG("\t- Target: " << info.targetName);
			if (info.hasApp) DMSG("\t- Program: " << info.appPath);
			exit(0);
		}

		try { arm(app); }
		catch (Exception e) { DMSG(e.what()); }
	}
	else {
		try { unload(app, info); }
		catch (Exception e) { DMSG(e.what()); }
	}

	/*String read;
	do {
	std::getline(std::cin, read);
	} while (read.length() == 0);*/

	return 0;
}