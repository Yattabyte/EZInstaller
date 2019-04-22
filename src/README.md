# Library
The core of this library can be found between 2 namespaces:  
- BFT (BufferTools)
- DFT (DirectoryTools)

#### The BufferTools namespace provides the following 5 functions:
- [CompressBuffer](#CompressBuffer)
- [DecompressBuffer](#DecompressBuffer)
- [DiffBuffers](#DiffBuffers)
- [PatchBuffer](#PatchBuffer)
- [HashBuffer](#HashBuffer)

#### The DirectoryTools namespace exposes 4 similar helper functions:
- [CompressDirectory](#CompressDirectory)
- [DecompressDirectory](#DecompressDirectory)
- [DiffDirectories](#DiffDirectories)
- [PatchDirectory](#PatchDirectory)

# Functions
### CompressBuffer
Compresses a source buffer into an equal or smaller-sized destination buffer.  
After compression, it applies a small header describing how large the uncompressed buffer is.
```c++
bool BFT::CompressBuffer(
  char * sourceBuffer, 
  const size_t & sourceSize,
  char ** destinationBuffer,
  size_t & destinationSize
);
```
Parameter:  `sourceBuffer`       the original buffer to read from.  
Parameter:  `sourceSize`         the size of the source buffer in bytes.  
Parameter:  `destinationBuffer`  pointer to the destination buffer, which will hold compressed contents.  
Parameter:  `destinationSize`    reference updated with the size in bytes of the compressed destinationBuffer.  
Return:                          true if compression success, false otherwise.  

Example Usage:
```c++
// Create a buffer, fill it with data
char * buffer = new char[100];

// Compress
char * compressedBuffer(nullptr);
size_t compressedSize(0);
bool result = BFT::CompressBuffer(buffer, 100, &compressedBuffer, compressedSize);
delete[] buffer;
if (result) {
  // Do something with compressedBuffer (size of compressedSize)
  delete[] compressedBuffer;
}
```

### DecompressBuffer
Decompressess a source buffer into an equal or larger-sized destination buffer.  
Prior to decompression, it reads from a small header describing how large of a buffer it needs to allocate.
```c++
bool BFT::DecompressBuffer(
  char * sourceBuffer, 
  const size_t & sourceSize, 
  char ** destinationBuffer, 
  size_t & destinationSize
);
```
Parameter:  `sourceBuffer`       the original buffer to read from.  
Parameter:  `sourceSize`         the size of the source buffer in bytes.  
Parameter:  `destinationBuffer`  pointer to the destination buffer, which will hold decompressed contents.  
Parameter:  `destinationSize`    reference updated with the size in bytes of the decompressed destinationBuffer.  
Return:                          true if decompression success, false otherwise.  

Example Usage:
```c++
// Create a buffer, fill it with data
char * compressedBuffer = new char[size_of_compressed_buffer];

// Decompress
char * buffer(nullptr);
size_t bufferSize(0);
bool result = BFT::DecompressBuffer(compressedBuffer, size_of_compressed_buffer, &buffer, bufferSize);
delete[] compressedBuffer;
if (result) {
  // Do something with decompressedBuffer (size of bufferSize)
  delete[] buffer;
}
```

### DiffBuffers
Processes two input buffers, diffing them.  
Generates a compressed instruction set dictating how to get from the old buffer to the new buffer.
```c++
bool BFT::DiffBuffers(
  char * buffer_old, 
  const size_t & size_old, 
  char * buffer_new, 
  const size_t & size_new, 
  char ** buffer_diff, 
  size_t & size_diff, 
  size_t * instructionCount = nullptr
);
```
Parameter:  `buffer_old`        the older of the 2 buffers.  
Parameter:  `size_old`          the size of the old buffer.  
Parameter:  `buffer_new`        the newer of the 2 buffers.  
Parameter:  `size_new`          the size of the new buffer.  
Parameter:  `buffer_diff`       pointer to store the diff buffer at.  
Parameter:  `size_diff`         reference updated with the size of the compressed diff buffer.  
Parameter:  `instructionCount`  (optional) pointer to update with the number of instructions processed.  
Return:                         true if diff success, false otherwise.  

Example Usage:
```c++
// Buffers for two input files
char * fileA = new char[100];
char * fileB = new char[255];

// ...Fill the buffers with some content... //

// Diff Files (generate instruction set from fileA - fileB)
char * diffBuffer(nullptr);
size_t diffSize(0), instructionCount(0);
bool result = BFT::DiffBuffers(fileA, 100, fileB, 255, &diffBuffer, diffSize, &instructionCount);
delete[] fileA;
delete[] fileB;
if (result) {
  // Do something with diffBuffer (size of diffSize)
  delete[] diffBuffer;
}
```

### PatchBuffer
Reads from a compressed instruction set, uses it to patch the 'older' buffer into the 'newer' buffer
```c++
bool BFT::PatchBuffer(
  char * buffer_old, 
  const size_t & size_old, 
  char ** buffer_new,
  size_t & size_new, 
  char * buffer_diff, 
  const size_t & size_diff, 
  size_t * instructionCount = nullptr
);
```
Parameter:  `buffer_old`        the older of the 2 buffers.  
Parameter:  `size_old`          the size of the old buffer.  
Parameter:  `buffer_new`        pointer to store the newer of the 2 buffers.  
Parameter:  `size_new`          reference updated with the size of the new buffer.  
Parameter:  `buffer_diff`       the compressed diff buffer (instruction set).  
Parameter:  `size_diff`         the size of the compressed diff buffer.  
Parameter:  `instructionCount`  (optional) pointer to update with the number of instructions processed.  
Return:                         true if patch success, false otherwise.  

Example Usage:
```c++
// Buffers for two input files
char * fileA = new char[100];
char * diffBuffer = new char[size_of_diff];

// ...Fill the buffers with some content... //

// Patch File (generate fileB from fileA + diffBuffer instructions)
char * fileB(nullptr);
size_t fileBSize(0), instructionCount(0);
bool result = BFT::PatchBuffer(fileA, 100, &fileB, fileBSize, diffBuffer, size_of_diff, &instructionCount);
delete[] fileA;
delete[] diffBuffer;
if (result) {
  // Do something with fileB (size of fileBSize)
  delete[] fileB;
}
```

### HashBuffer
Generates a hash value for the buffer provided, using the buffers' contents.
```c++
size_t BFT::HashBuffer(
  char * buffer, 
  const size_t & size
);
```
Parameter:  `buffer`  the older of the 2 buffers.  
Parameter:  `size`    the size of the old buffer.  
Return:               hash value for the buffer.    

Example Usage:
```c++
// Buffers for two input files
char * fileA = new char[100];
char * fileB = new char[255];

// ...Fill the buffers with some content... //

size_t hashA = BFT::HashBuffer(fileA, 100);
size_t hashB = BFT::HashBuffer(fileB, 255);

if (hashA != hashB) {
  // Diff the buffers or do something with the knowledge that they differ
}
```

### CompressDirectory
Compresses all disk contents found within a source directory into an .npack - package formatted buffer.  
After compression, it applies a small header dictating packaged folders' name.  
```c++
bool DFT::CompressDirectory(
  const std::string & srcDirectory, 
  char ** packBuffer, 
  size_t & packSize, 
  size_t * byteCount = nullptr,
  size_t * fileCount = nullptr,
  const std::vector<std::string> & exclusions = std::vector<std::string>()
);
```
Parameter:  `srcDirectory`  the absolute path to the directory to compress.  
Parameter:  `packBuffer`    pointer to the destination buffer, which will hold compressed contents.  
Parameter:  `packSize`      reference updated with the size in bytes of the compressed packBuffer.  
Parameter:  `byteCount`     (optional) pointer updated with the number of bytes written into the package.  
Parameter:  `fileCount`     (optional) pointer updated with the number of files written into the package.  
Parameter:  `exclusions`    (optional) list of filenames/types to skip "\\string" match relative path, ".ext" match extension.  
Return:                     true if compression success, false otherwise.  

Example Usage:
```c++
std::string directory_to_compress = "C:\\some directory";

// Compress
char * packageBuffer(nullptr);
size_t packageSize(0ull), maxSize(0ull), fileCount(0ull);
bool result = DRT::CompressDirectory(directory_to_compress, &packageBuffer, packageSize, &maxSize, &fileCount, {"\\cache.txt", ".cache"});
if (result) {
  // Do something with packageBuffer (size of packageSize)
  delete[] packageBuffer;
}
```

### DecompressDirectory
Decompresses an .npack - package formatted buffer into its component files in the destination directory.
```c++
bool DFT::DecompressDirectory(
  const std::string & dstDirectory, 
  char * packBuffer, 
  const size_t & packSize, 
  size_t * byteCount = nullptr,
  size_t * fileCount = nullptr
);
```
Parameter:  `dstDirectory`  the absolute path to the directory to compress.  
Parameter:  `packBuffer`    the buffer containing the compressed package contents.  
Parameter:  `packSize`      the size of the buffer in bytes.  
Parameter:  `byteCount`     (optional) pointer updated with the number of bytes written to disk.   
Parameter:  `fileCount`     (optional) pointer updated with the number of files written to disk.  
Return:                     true if decompression success, false otherwise.  

Example Usage:
```c++
std::string directory_to_write = "C:\\some directory";
char * packageBuffer = get_package_buffer();
size_t packageSize = get_package_size();

// Compress
size_t byteCount(0ull), fileCount(0ull);
bool result = DRT::DecompressDirectory(directory_to_write, packageBuffer, packageSize, &byteCount, &fileCount);
delete[] packageBuffer;
if (result) {
  // Do something knowing that the package has extracted
}
```

### DiffDirectories
Processes two input directories and generates a compressed instruction set for transforming the old directory into the new directory.
```c++
bool DFT::DiffDirectories(
  const std::string & oldDirectory, 
  const std::string & newDirectory, 
  char ** diffBuffer, 
  size_t & diffSize, 
  size_t & instructionCount
);
```
Parameter:  `oldDirectory`      the older directory, or path to an .npack file.  
Parameter:  `newDirectory`      the newer directory, or path to an .npack file.  
Parameter:  `diffBuffer`        pointer to the diff buffer, which will hold compressed diff instructions.  
Parameter:  `diffSize`          reference updated with the size in bytes of the diff buffer.  
Parameter:  `instructionCount`  (optional) pointer updated with the number of instructions compressed into the diff buffer.  
Return:                         true if diff success, false otherwise.  

Example Usage:
```c++
std::string oldDirectory = "C:\\old software";
std::string newDirectory = "C:\\new software";

// Diff the 2 directories
char * diffBuffer(nullptr);
size_t diffSize(0ull), instructionCount(0ull);
bool result = DRT::DiffDirectories(oldDirectory, newDirectory, &diffBuffer, diffSize, &instructionCount);
if (result) {
  // Do something with the diffBuffer (size of diffSize)
  delete[] diffBuffer;
}
```