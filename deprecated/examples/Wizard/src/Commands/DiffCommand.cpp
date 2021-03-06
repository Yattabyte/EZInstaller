#include "Commands/DiffCommand.h"
#include "Directory.h"
#include "StringConversions.h"
#include "Log.h"
#include <filesystem>
#include <fstream>


int DiffCommand::execute(const int& argc, char* argv[]) const
{
	// Supply command header to log
	yatta::Log::PushText(
		"                      ~\r\n"
		"      Patch Maker    /\r\n"
		"  ~-----------------~\r\n"
		" /\r\n"
		"~\r\n\r\n"
	);

	// Check command line arguments
	std::string oldDirectory;
	std::string newDirectory;
	std::string dstDirectory;
	for (int x = 2; x < argc; ++x) {
		std::string command = yatta::string_to_lower(std::string(argv[x], 5));
		if (command == "-old=")
			oldDirectory = yatta::Directory::SanitizePath(std::string(&argv[x][5]));
		else if (command == "-new=")
			newDirectory = yatta::Directory::SanitizePath(std::string(&argv[x][5]));
		else if (command == "-dst=")
			dstDirectory = yatta::Directory::SanitizePath(std::string(&argv[x][5]));
		else {
			yatta::Log::PushText(
				" Arguments Expected:\r\n"
				" -old=[path to the older directory]\r\n"
				" -new=[path to the newer directory]\r\n"
				" -dst=[path to write the diff file] (can omit filename)\r\n"
				"\r\n"
			);
			return EXIT_FAILURE;
		}
	}

	// If user provides a directory only, append a filename
	if (std::filesystem::is_directory(dstDirectory)) {
		const auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm bt{};
		localtime_s(&bt, &time);
		dstDirectory = yatta::Directory::SanitizePath(dstDirectory) + "\\" + std::to_string(bt.tm_year) + std::to_string(bt.tm_mon) + std::to_string(bt.tm_mday) + std::to_string(bt.tm_hour) + std::to_string(bt.tm_min) + std::to_string(bt.tm_sec);
	}

	// Ensure a file-extension is chosen
	if (!std::filesystem::path(dstDirectory).has_extension())
		dstDirectory += ".ndiff";

	// Try to diff the 2 directories specified
	const auto diffBuffer = yatta::Directory(oldDirectory).make_delta(yatta::Directory(newDirectory));
	if (!diffBuffer)
		yatta::Log::PushText("Cannot diff the two paths chosen, aborting...\r\n");
	else {
		// Try to create diff file
		std::filesystem::create_directories(std::filesystem::path(dstDirectory).parent_path());
		std::ofstream file(dstDirectory, std::ios::binary | std::ios::out);
		if (!file.is_open())
			yatta::Log::PushText("Cannot write diff file to disk, aborting...\r\n");
		else {
			// Write the diff file to disk
			file.write(diffBuffer->cArray(), std::streamsize(diffBuffer->size()));
			file.close();

			// Output results
			yatta::Log::PushText("Bytes written:  " + std::to_string(diffBuffer->size()) + "\r\n");
			return EXIT_SUCCESS;
		}
	}

	return EXIT_FAILURE;
}