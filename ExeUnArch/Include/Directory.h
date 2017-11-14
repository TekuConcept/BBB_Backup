/**
* Created by TekuConcept on August 9, 2017
*/

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#include <vector>
#include "cstypes.h"

namespace System {
	namespace IO {
		class Directory {
		private:
			Directory();
		public:
			typedef std::pair<std::vector<String>, std::vector<String>> FolderFileCollectionPair;

			static void Create(String path); // Creates all directories and subdirectories in the specified path unless they already exist.
			static void Delete(String path); // Deletes an empty directory from a specified path.
			static void Delete(String path, Boolean recursive); // Deletes the specified directory and, if indicated, any subdirectories and files in the directory.
			static bool Exists(String path); // Determines whether the given path refers to an existing directory on disk.
			static String GetCurrentDirectory(); // Gets the current working directory of the application.
			static std::vector<String> GetDirectories(String path); // Returns the names of subdirectories(including their paths) in the specified directory.
			static std::vector<String> GetFiles(String path); // Returns the names of files(including their paths) in the specified directory.
			static FolderFileCollectionPair GetAllPaths(String path);
			static String GetDirectoryRoot(String); // Returns the volume information, root information, or both for the specified path.
			static String GetParent(String path); // Retrieves the parent directory of the specified path, including both absolute and relative paths.
			static String GetAbsolutePath(String path); // Retrieves the absolute path of the specified path.
			static void Move(String sourceDirName, String destDirName); // Moves a file or a directory and its contents to a new location.
			static String SplicePaths(String prefix, String suffix); // Splices two paths together.
			static String NormalizeDelimiters(String path); // replaces all '\\' with '/'
		};
	}
}

#endif