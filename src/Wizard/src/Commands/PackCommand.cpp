#include "Commands/PackCommand.h"
#include "BufferTools.h"
#include "Common.h"
#include "DirectoryTools.h"
#include "Log.h"
#include <fstream>


void PackCommand::execute(const int & argc, char * argv[]) const
{
	// Supply command header to console
	Log::PushText(
		"                     ~\r\n"
		"       Packager     /\r\n"
		"  ~----------------~\r\n"
		" /\r\n"
		"~\r\n\r\n"
	);

	// Check command line arguments
	std::string srcDirectory(""), dstDirectory("");
	for (int x = 2; x < argc; ++x) {
		std::string command = string_to_lower(std::string(argv[x], 5));
		if (command == "-src=")
			srcDirectory = sanitize_path(std::string(&argv[x][5]));
		else if (command == "-dst=")
			dstDirectory = sanitize_path(std::string(&argv[x][5]));
		else
			exit_program(
				" Arguments Expected:\r\n"
				" -src=[path to the directory to package]\r\n"
				" -dst=[path to write the package] (can omit filename)\r\n"
				"\r\n"
			);
	}	

	// If user provides a directory only, append a filename
	if (std::filesystem::is_directory(dstDirectory)) 
		dstDirectory = sanitize_path(dstDirectory) + "\\" + std::filesystem::path(srcDirectory).stem().string() + ".npack";

	// Ensure a file-extension is chosen
	if (!std::filesystem::path(dstDirectory).has_extension())
		dstDirectory += ".npack";

	// Compress the directory specified
	char * packBuffer(nullptr);
	size_t packSize(0ull), maxSize(0ull), fileCount(0ull);
	if (!DRT::CompressDirectory(srcDirectory, &packBuffer, packSize, &maxSize, &fileCount))
		exit_program("Cannot create package from the directory specified, aborting...\r\n");
	
	// Write package to disk
	std::filesystem::create_directories(std::filesystem::path(dstDirectory).parent_path());
	std::ofstream file(dstDirectory, std::ios::binary | std::ios::out);
	if (!file.is_open())
		exit_program("Cannot write package to disk, aborting...\r\n");
	file.write(packBuffer, (std::streamsize)packSize);
	file.close();
	delete[] packBuffer;

	// Output results
	Log::PushText(
		"Files packaged:  " + std::to_string(fileCount) + "\r\n" +
		"Bytes packaged:  " + std::to_string(maxSize) + "\r\n" +
		"Compressed Size: " + std::to_string(packSize) + "\r\n"
	);
}