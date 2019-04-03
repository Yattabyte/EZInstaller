#include "UnpackCommand.h"
#include "BufferTools.h"
#include "Common.h"
#include "DirectoryTools.h"
#include "TaskLogger.h"
#include <fstream>


void UnpackCommand::execute(const int & argc, char * argv[]) const
{
	// Supply command header to console
	auto & logger = TaskLogger::GetInstance();
	logger << 
		"                      ~\r\n"
		"    Unpacker         /\r\n"
		"  ~-----------------~\r\n"
		" /\r\n"
		"~\r\n\r\n";

	// Check command line arguments
	std::string srcDirectory(""), dstDirectory("");
	for (int x = 2; x < argc; ++x) {
		std::string command(argv[x], 5);
		std::transform(command.begin(), command.end(), command.begin(), ::tolower);
		if (command == "-src=")
			srcDirectory = std::string(&argv[x][5]);
		else if (command == "-dst=")
			dstDirectory = std::string(&argv[x][5]);
		else
			exit_program(
				" Arguments Expected:\r\n"
				" -src=[path to the package file]\r\n"
				" -dst=[directory to write package contents]\r\n"
				"\r\n"
			);
	}

	// Open pack file
	std::ifstream packFile(srcDirectory, std::ios::binary | std::ios::beg);
	const size_t packSize = std::filesystem::file_size(srcDirectory);
	if (!packFile.is_open())
		exit_program("Cannot read diff file, aborting...\r\n");

	// Copy contents into a buffer
	char * packBuffer = new char[packSize];
	packFile.read(packBuffer, std::streamsize(packSize));
	packFile.close();

	// Unpackage using the resource file
	size_t fileCount(0ull), byteCount(0ull);
	if (!DRT::DecompressDirectory(dstDirectory, packBuffer, packSize, byteCount, fileCount))
		exit_program("Cannot decompress package file, aborting...\r\n");

	// Output results
	logger
		<< "Files written:  " << std::to_string(fileCount) << "\r\n"
		<< "Bytes written:  " << std::to_string(byteCount) << "\r\n";
}