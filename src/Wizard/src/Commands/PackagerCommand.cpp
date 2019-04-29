#include "Commands/PackagerCommand.h"
#include "nSuite.h"
#include "StringConversions.h"
#include "Log.h"
#include "Resource.h"
#include <fstream>


int PackagerCommand::execute(const int & argc, char * argv[]) const
{
	// Supply command header to console
	NST::Log::PushText(
		"                             ~\r\n"
		"    Portable Package Maker  /\r\n"
		"  ~------------------------~\r\n"
		" /\r\n"
		"~\r\n\r\n"
	);

	// Create common variables
	bool success = false;
	HANDLE handle(nullptr);
	std::ofstream file;
	char * packBuffer(nullptr);
	size_t packSize(0ull), maxSize(0ull), fileCount(0ull);
	std::string srcDirectory(""), dstDirectory("");
	const NST::Resource unpacker(IDR_UNPACKER, "UNPACKER");

	// Check command line arguments
	for (int x = 2; x < argc; ++x) {
		std::string command = string_to_lower(std::string(argv[x], 5));
		if (command == "-src=")
			srcDirectory = NST::SanitizePath(std::string(&argv[x][5]));
		else if (command == "-dst=")
			dstDirectory = NST::SanitizePath(std::string(&argv[x][5]));
		else {
			NST::Log::PushText(
				" Arguments Expected:\r\n"
				" -src=[path to the directory to package]\r\n"
				" -dst=[path to write the portable package] (can omit filename)\r\n"
				"\r\n"
			);
			return EXIT_FAILURE;
		}
	}

	// If user provides a directory only, append a filename
	if (std::filesystem::is_directory(dstDirectory))
		dstDirectory = NST::SanitizePath(dstDirectory) + "\\package.exe";

	// Ensure a file-extension is chosen
	if (!std::filesystem::path(dstDirectory).has_extension())
		dstDirectory += ".exe";
	
	// Try to compress the directory specified
	if (!NST::CompressDirectory(srcDirectory, &packBuffer, packSize, &maxSize, &fileCount))
		NST::Log::PushText("Cannot create package from the directory specified, aborting...\r\n");
	else {
		// Ensure resource exists
		if (!unpacker.exists())
			NST::Log::PushText("Cannot access unpacker resource, aborting...\r\n");
		else {
			// Try to create package file
			std::filesystem::create_directories(std::filesystem::path(dstDirectory).parent_path());
			file = std::ofstream(dstDirectory, std::ios::binary | std::ios::out);
			if (!file.is_open())
				NST::Log::PushText("Cannot write package to disk, aborting...\r\n");
			else {
				// Write packager to disk
				file.write(reinterpret_cast<char*>(unpacker.getPtr()), (std::streamsize)unpacker.getSize());
				file.close();

				// Try to update packager's resource
				handle = BeginUpdateResource(dstDirectory.c_str(), false);
				if (!(bool)UpdateResource(handle, "ARCHIVE", MAKEINTRESOURCE(IDR_ARCHIVE), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), packBuffer, (DWORD)packSize))
					NST::Log::PushText("Cannot write archive contents to the package, aborting...\r\n");				
				else {
					// Output results
					NST::Log::PushText(
						"Files packaged:  " + std::to_string(fileCount) + "\r\n" +
						"Bytes packaged:  " + std::to_string(maxSize) + "\r\n" +
						"Compressed Size: " + std::to_string(packSize) + "\r\n"
					);
					success = true;
				}
			}
		}
	}

	// Clean-up
	file.close();
	EndUpdateResource(handle, !success);
	delete[] packBuffer;
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}