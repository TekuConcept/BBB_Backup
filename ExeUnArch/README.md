My quick 2-3 day attempt at creating a program that could "pack" another program, and then "unpack" (or install) that same program on another system. In other words, this is my attempt at writting a cross-platform installer bootstrap. (Not exactly the most elegant solution in my opinion... for now.)

This uses the XZ library to compress and decompress the target folder. The program appends the folder to the end of the executable (itself) when "arming" along with a footer containing metadata. When the armed program is run, it looks for the footer. If the footer is present, the program then attemps to unarchive itself.

The motivation behind this project was to write a quick hack to temporarilly compensate against a possible design flaw in a network content provider's update setup. It has served its purpose well as a temporary hot-fix solution.

I want to keep this code as a reference - specifically for some of the features within. I needed to completey write a make-shift cross-platform filesystem from scratch. (I recognize C++14 has this built in, but not all systems are that up-to-date.) Then I needed to figure out a quick way to replicate TAR's functionality, again from scratch. Finally, during this project, I figured out how to utilize the XZ library's C compression/decompression functions while wrapping them behind C++ objects.