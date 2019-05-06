#pragma once
#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "Buffer.h"
#include <filesystem>
#include <string>
#include <vector>


/** Add directory-related classes to the nSuite namespace. */
namespace NST {
	/***/
	class Directory {
	public:
		// Public (de)Constructors
		/** Destroy this virtual directory. */
		~Directory();
		/** Constructs a virtual directory from a path on disk.
		If the path is to a folder, the folder is immediately virtualized.
		If the path is to a .npack file, the package is read and virtualized.
		If the path is to an application with an embedded .npack resource, the application is mounted then the package is read and virtualized.
		@param	path				the absolute path to a desired folder or a package file.
		@param	exclusions			(optional) list of filenames/types to skip. "string" matches relative path, ".ext" matches extension.
		@return						directory generated from the parameters chosen. */
		Directory(const std::string & path, const std::vector<std::string> & exclusions = std::vector<std::string>());
		/** Constructs a virtual directory directly from a package buffer.
		@param	package				the package buffer to unpack and virtualize.
		@param	path				the absolute path to a desired folder or a package file.
		@param	exclusions			(optional) list of filenames/types to skip. "string" matches relative path, ".ext" matches extension.
		@return						directory generated from the parameters chosen. */
		Directory(const Buffer & package, const std::string & path, const std::vector<std::string> & exclusions = std::vector<std::string>());
		/** Copy Constructor.
		@param	other				the directory to copy from. */
		Directory(const Directory & other);
		/** Move Constructor.
		@param	other				the directory to move from and invalidate. */
		Directory(Directory && other);


		// Public Assignment Operators
		/** Copy-assignment operator.
		@param	other				the buffer to copy from.
		@return						reference to this. */
		Directory & operator=(const Directory & other);
		/** Move-assignment operator.
		@param	other				the buffer to move from.
		@return						reference to this. */
		Directory & operator=(Directory && other);
				

		// Public Output Methods
		/** Writes this virtual directory's contents to disk. */
		bool make_folder() const;
		/** Compresses all data found within this directory into an .npack - package formatted buffer.
		@return						on success a pointer to a buffer containing the compressed directory contents, empty otherwise. 
		Buffer format:
		-------------------------------------------------------------------------------------------
		| header: identifier title, package name size, package name  | compressed directory data  |
		------------------------------------------------------------------------------------------- */
		std::optional<Buffer> make_package() const;
		/** Compares this directory against another, generating a .ndiff - delta formatted buffer, for transforming this into the new directory.		
		@param	newDirectory		the newer directory to compare against.
		@return						on success a pointer to a buffer containing the patch instructions. 
		Buffer format:
		-------------------------------------------------------------------------------
		| header: identifier title, modified file count  | compressed directory data  |
		------------------------------------------------------------------------------- */
		std::optional<Buffer> make_delta(const Directory & newDirectory) const;


		// Public Methods
		/** Executes the instructions contained within a .ndiff - delta formatted buffer upon this directory.
		@param	diffBuffer			the buffer containing the compressed diff instructions.
		@return						true on update success, false otherwise. */
		bool apply_delta(const Buffer & diffBuffer);
		/** Retrieve the number of files in this directory. 
		@return						the file-count for this directory. */
		size_t fileCount() const;
		/** Calculates the number of bytes this directory uses.
		@return						the number of bytes used by this directory. */
		size_t byteCount() const;			


		// Public Header Structs
		struct PackageHeader : NST::Buffer::Header {
			// Attributes
			static constexpr const char TITLE[] = "nSuite package";
			size_t m_charCount = 0ull;
			std::string m_folderName = "";


			// (de)Constructors
			PackageHeader() = default;
			PackageHeader(const size_t & folderNameSize, const char * folderName) : Header(TITLE), m_charCount(folderNameSize) {
				m_folderName = std::string(folderName, folderNameSize);
			}


			// Interface Implementation
			virtual size_t size() const override {
				return size_t(sizeof(size_t) + (sizeof(char) * m_charCount));
			}
			virtual std::byte * operator << (std::byte * ptr) override {
				ptr = Header::operator<<(ptr);
				std::copy(ptr, &ptr[size_t(sizeof(size_t))], reinterpret_cast<std::byte*>(&m_charCount));
				ptr = &ptr[size_t(sizeof(size_t))];
				char * folderArray = new char[m_charCount];
				std::copy(ptr, &ptr[size_t(sizeof(char) * m_charCount)], reinterpret_cast<std::byte*>(&folderArray[0]));
				m_folderName = std::string(folderArray, m_charCount);
				delete[] folderArray;
				return &ptr[size_t(sizeof(char) * m_charCount)];
			}
			virtual std::byte *operator >> (std::byte * ptr) const override {
				ptr = Header::operator>>(ptr);
				*reinterpret_cast<size_t*>(ptr) = m_charCount;
				ptr = &ptr[size_t(sizeof(size_t))];
				std::copy(m_folderName.begin(), m_folderName.end(), reinterpret_cast<char*>(ptr));
				return &ptr[size_t(sizeof(char) * m_charCount)];
			}
		};
		/** Holds and performs Patch I/O operations on buffers. */
		struct PatchHeader : NST::Buffer::Header {
			// Attributes
			static constexpr const char TITLE[] = "nSuite patch";
			size_t m_fileCount = 0ull;


			// (de)Constructors
			PatchHeader(const size_t size = 0ull) : Header(TITLE), m_fileCount(size) {}


			// Interface Implementation
			virtual size_t size() const override {
				return size_t(sizeof(size_t));
			}
			virtual std::byte * operator << (std::byte * ptr) override {
				ptr = Header::operator<<(ptr);
				std::copy(ptr, &ptr[size()], reinterpret_cast<std::byte*>(&m_fileCount));
				return &ptr[size()];
			}
			virtual std::byte *operator >> (std::byte * ptr) const override {
				ptr = Header::operator>>(ptr);
				*reinterpret_cast<size_t*>(ptr) = m_fileCount;
				return &ptr[size()];
			}
		};


	private:
		// Private Default Constructor
		Directory() = default;


		// Private Destruction Method
		void devirtualize();


		// Private Methods
		/** Populates this virtual directory with the contents found in the specified path.
		@param	path				the path to pull data from, on disk. */
		void virtualize_folder(const std::string & path);
		/** Populates this virtual directory with the contents found in the specified package.
		@param	buffer				the package buffer to pull data from. */
		void virtualize_package(const Buffer & buffer);


		// Private Attributes
		struct DirFile {
			std::string relativePath = "";
			size_t size = 0ull;
			std::byte * data = nullptr;
		};
		std::vector<DirFile> m_files;
		std::string m_directoryPath = "", m_directoryName = "";
		std::vector<std::string> m_exclusions;
	};
};

#endif // DIRECTORY_H