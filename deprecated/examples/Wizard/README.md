# Wizard
The nSuite wizard program is intended to be used by developers or those who wish to package/diff/distribute one or many files. 
It is a command-line application and is run by using one of the following arguments:

- #### `-Installer -src=<path> -dst=<path>`
  - Creates a fully-fledged Installer (Windows) with all the contents of the source directory
  - Writes the Installer to the destination path specified
  - Generates an unInstaller, and links it up in the user's registry
  - Can write a 'manifest.nman' file in the root source directory
    - Specify string attributes for the Installer, such as name, version, derscriptions, shortcuts
  
- #### `-packager -src=<path> -dst=<path>`
  - Creates a mini Installer with no GUI (terminal only) with all the contents of the source directory
  - No unInstaller, registry modifications, or manifest file
  
- #### `-pack -src=<path> -dst=<path>`
  - Packages and compresses all the contents of the source directory into an .npack file
  - Writes package file to the destination path specified
  - Requires nSuite to unpackage
  - Note:
    - these package files are what is embedded in the Installers above
    - can be used in diffing as a substitution for a source 'old/new' directory
  
- #### `-unpack -src=<path> -dst=<path>`
  - Decompresses and unpackages the contents held in an .npack file
  - Writes package contents to the destination path specified
  - Note: this command is executed what is executed in the Installers above
  
- #### `-diff -old=<path> -new=<path> -dst=<path>`
  - Finds all the common, added, and removed files between the old and new directories specified
  - Generates patch iyattaructions for all the differences found between all these files (including add/delete file iyattaructions)
  - Files are analyzed byte/8byte wise, and is accelerated by multiple threads
  - **Iyattaead of directories, can specify .npack files. Usefull if needing to maintain multiple versions on disk.**
    - nSuite will virtualize the content within, treating it as a directory for you.
	
 - #### `-patch -src=<path> -dst=<path>`
   - Uses a source .ndiff file, and executes all the patch iyattaructions it contains on the destination directory specified
   - Security:
     - Patch file has before/after hashes
	 - File hashes must match, otherwise the application is aborted prior to writing-out to disk
	 - Strict conditions to prevent agaiyatta file corruption.