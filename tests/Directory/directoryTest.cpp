#include "yatta.hpp"
#include <cassert>
#include <iostream>

// Convenience Definitions
using yatta::Directory;

// Forward Declarations
void Directory_ConstructionTest();
void Directory_MethodTest();
void Directory_ManipulationTest();

int main() {
    Directory_ConstructionTest();
    Directory_MethodTest();
    Directory_ManipulationTest();
    exit(0);
}

void Directory_ConstructionTest() {
    // Ensure we can make an empty directory
    Directory directory;
    assert(directory.empty());

    // Ensure we can virtualize directories
    Directory dirA(Directory::GetRunningDirectory() + "/old");
    assert(dirA.hasFiles());

    // Dump to a package, and ensure we can virtualize the package
    const auto package = dirA.out_package("old");
    assert(package.has_value());
    Directory dirB(*package);
    assert(dirA.hasFiles());

    // Ensure move constructor works
    Directory moveDirectory(
        Directory(Directory::GetRunningDirectory() + "/old"));
    assert(moveDirectory.fileCount() == dirA.fileCount());

    // Ensure copy constructor works
    const Directory copyDir(moveDirectory);
    assert(copyDir.fileSize() == moveDirectory.fileSize());
}

void Directory_MethodTest() {
    // Ensure we can retrieve the running directory
    assert(!Directory::GetRunningDirectory().empty());

    // Verify empty directories
    Directory directory;
    assert(directory.empty());

    // Verify that we can input folders
    directory.in_folder(Directory::GetRunningDirectory() + "/old");
    assert(directory.hasFiles());

    // Ensure we have 4 files all together
    assert(directory.fileCount() == 4ULL);

    // Ensure the total size is as expected
    assert(directory.fileSize() == 147777ULL);

    // Ensure we can hash an actual directory
    assert(directory.hash() != yatta::ZeroHash);

    // Ensure we can clear a directory
    directory.clear();
    assert(directory.empty() && !directory.hasFiles());

    // Ensure we can hash an empty directory
    assert(directory.hash() == yatta::ZeroHash);
}

void Directory_ManipulationTest() {
    // Verify empty directories
    Directory directory;
    assert(directory.empty());

    // Ensure we have 8 files all together
    directory.in_folder(Directory::GetRunningDirectory() + "/old");
    directory.in_folder(Directory::GetRunningDirectory() + "/new");
    assert(directory.fileCount() == 8ULL);

    // Ensure the total size is as expected
    assert(directory.fileSize() == 189747ULL);

    // Reset the directory to just the 'old' folder, hash it
    directory = Directory(Directory::GetRunningDirectory() + "/old");
    const auto oldHash = directory.hash();
    (void)oldHash; // avoid GCC warning on old hash not being used
    assert(oldHash != yatta::ZeroHash);

    // Overwrite the /old folder, make sure hashes match
    directory.out_folder(Directory::GetRunningDirectory() + "/old");
    directory = Directory(Directory::GetRunningDirectory() + "/old");
    assert(directory.hash() == oldHash);

    // Ensure we can dump a directory as a package
    const auto package = directory.out_package("package");
    assert(package.has_value() && package->hasData());

    // Ensure we can import a package and that it matches the old one
    directory.clear();
    directory.in_package(*package);
    assert(directory.fileSize() == 147777ULL && directory.fileCount() == 4ULL);

    // Ensure new hash matches
    assert(directory.hash() == oldHash);

    // Try to diff the old and new directories
    Directory newDirectory(Directory::GetRunningDirectory() + "/new");
    const auto deltaBuffer = directory.out_delta(newDirectory);
    assert(deltaBuffer.has_value() && deltaBuffer->hasData());

    // Try to patch the old directory into the new directory
    assert(directory.in_delta(*deltaBuffer));

    // Ensure the hashes match
    assert(directory.hash() == newDirectory.hash());

    // Overwrite the /new folder, make sure the hashes match
    directory.out_folder(Directory::GetRunningDirectory() + "/new");
    directory = Directory(Directory::GetRunningDirectory() + "/new");
    assert(directory.hash() == newDirectory.hash());

    // Ensure we can't export an empty directory
    directory.clear();
    assert(!directory.out_folder(""));

    // Ensure we can't import an invalid directory
    assert(!directory.in_folder(""));

    // Ensure we can't export an empty package
    assert(!directory.out_package(""));

    // Ensure we can't import an empty package
    assert(!directory.in_package(yatta::Buffer()));
}