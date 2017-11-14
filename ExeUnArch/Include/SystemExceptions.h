/**
 * Created by TekuConcept on August 10, 2017
 *
 * Exceptions' names were taken from Microsoft's C# exceptions.
 * This was because the file system was being closely modeled after
 * the file system used in C#.
 */

#ifndef SYSTEM_EXCEPTIONS_H
#define SYSTEM_EXCEPTIONS_H

#include <stdexcept>

namespace System {
	class Exception : public std::runtime_error {
	public:
		Exception() :
			runtime_error("General Exception") {}
		Exception(const char* msg) : runtime_error(msg) {}
	};

	class ArgumentException : public Exception {
	public:
		ArgumentException() : Exception("Argument Exception") {}
		ArgumentException(const char* msg) : Exception(msg) {}
	};

	class DirectoryNotFoundException : public Exception {
	public:
		DirectoryNotFoundException() :
			Exception("Directory Not Found Exception") {}
		DirectoryNotFoundException(const char* msg) :
			Exception(msg) {}
	};

	class IOException : public Exception {
	public:
		IOException() : Exception("IO Exception") {}
		IOException(const char* msg) : Exception(msg) {}
	};

	class PathTooLongException : public Exception {
	public:
		PathTooLongException() :
			Exception("Path Too Long Exception") {}
		PathTooLongException(const char* msg) : Exception(msg) {}
	};

	class UnauthorizedAccessException : public Exception {
	public:
		UnauthorizedAccessException() :
			Exception("Unauthorized Access Exception") {}
		UnauthorizedAccessException(const char* msg) : Exception(msg) {}
	};

	class OutOfMemoryException : public Exception {
	public:
		OutOfMemoryException() :
			Exception("Out Of Memory Exception") {}
		OutOfMemoryException(const char* msg) : Exception(msg) {}
	};

	class NotImplementedException : public Exception {
	public:
		NotImplementedException() :
			Exception("Not Implemented Exception") {}
		NotImplementedException(const char* msg) : Exception(msg) {}
	};

	class NotSupportedException : public Exception {
	public:
		NotSupportedException() :
			Exception("Not Supported Exception") {}
		NotSupportedException(const char* msg) : Exception(msg) {}
	};
}

#endif