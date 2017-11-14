/**
* Created by TekuConcept on August 9, 2017
*/

#ifndef FILE_H
#define FILE_H

#include <string>
#include <vector>
#include "cstypes.h"

namespace System {
	namespace IO {
		class File {
		private:
			File();
		public:
			static void AppendAllLines(String path, std::vector<String> lines); // Appends lines to a file, and then closes the file. If the specified file does not exist, this method creates a file, writes the specified lines to the file, and then closes the file.
			static void AppendAllLines(String path, String* lines, Int32 length); // Appends lines to a file, and then closes the file. If the specified file does not exist, this method creates a file, writes the specified lines to the file, and then closes the file.
			static void AppendAllText(String path, String contents); // Opens a file, appends the specified string to the file, and then closes the file. If the file does not exist, this method creates a file, writes the specified string to the file, then closes the file.
			static void Copy(String sourceFileName, String destFileName); // Copies an existing file to a new file. Overwriting a file of the same name is not allowed.
			static void Copy(String sourceFileName, String destFileName, Boolean overwrite); // Copies an existing file to a new file.Overwriting a file of the same name is allowed.
			static void Delete(String path); // Deletes the specified file.
			static bool Exists(String path); // Determines whether the specified file exists.
			static void Move(String sourceFileName, String destFileName); // Moves a specified file to a new location, providing the option to specify a new file name.
			static Long FileSize(String path); // Gets the size of the specified file.
			static Int32 FilePermissions(String path); // Gets the file permissions
			static void SetFilePermissions(String path, Int32); // Sets the file permissions
			static String FileName(String path); // Gets the file name from a specified path.
			static String ReadAllBytes(String path); // Opens a binary file, reads the contents of the file into a byte array, and then closes the file.
			static std::vector<String> ReadAllLines(String path); // Opens a text file, reads all lines of the file, and then closes the file.
			static String ReadAllText(String path); // Opens a text file, reads all lines of the file, and then closes the file.
			static void WriteAllBytes(String path, String bytes); // Creates a new file, writes the specified byte array to the file, and then closes the file.If the target file already exists, it is overwritten.
			static void WriteAllLines(String path, String* lines, Int32 length); // Creates a new file, write the specified string array to the file, and then closes the file.
			static void WriteAllLines(String path, std::vector<String> lines); // Creates a new file, writes a collection of strings to the file, and then closes the file.
			static void WriteAllText(String path, String contents); // Creates a new file, writes the specified string to the file, and then closes the file.If the target file already exists, it is overwritten.
		};
	}
}

#endif