#include <exception>
#include <stdexcept>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include <iostream>

// +++++++++++++++++++++++++++++++++++++++++++++++++++

#include <boost/filesystem.hpp>

// See the boost documentation for the filesystem
// Especially: http://www.boost.org/doc/libs/1_41_0/libs/filesystem/doc/reference.html#Path-decomposition-table
// Link against boost_filesystem-mt (for multithreaded) or boost_filesystem
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
 
// +++++++++++++++++++++++++++++++++++++++++++++++++++
// ===== DEFINITIONS =======
typedef std::list<boost::filesystem::path> path_list_type;
std::map<std::string,path_list_type> getFiles(boost::filesystem::path dir);

// ===== IMPLEMENTATIONS =======
std::map<std::string,path_list_type> getFiles(boost::filesystem::path dir)
{
	// Define my map keys
	const std::string regular_file("REGULAR");
	const std::string directory_file("DIRECTORY");
	const std::string other_file("OTHER");
	
	// This is a return object
	// REGULAR -> {file1r,file2r,...,fileNr}
	// DIRECTORY -> {file1d,file2d,...,fileNd}
	// ...
	std::map<std::string,path_list_type> directoryContents;
	
	// Change to the absolute system path, instead of relative
	//boost::filesystem::path dirPath(boost::filesystem::system_complete(dir));
	boost::filesystem::path dirPath(dir);
	
	// Verify existence and directory status
	if ( !boost::filesystem::exists( dirPath ) ) 
	{
		std::stringstream msg;
		msg << "Error: " << dirPath.file_string() << " does not exist " << std::endl;
		throw std::runtime_error(msg.str());
	}
	
	if ( !boost::filesystem::is_directory( dirPath ) ) 
	{
		std::stringstream msg;
		msg << "Error: " << dirPath.file_string() << " is not a directory " << std::endl;
		throw std::runtime_error(msg.str());
	}

#ifdef GJS_DEBUG_PRINT				
	std::cout << "Processing directory: " << dirPath.directory_string() << std::endl;
#endif	

	// A director iterator... is just that, 
	// an iterator through a directory... crazy!
	boost::filesystem::directory_iterator end_iter;
	for ( boost::filesystem::directory_iterator dir_itr( dirPath );
		dir_itr != end_iter;
		++dir_itr )
	{
		// Attempt to test file type and push into correct list
		try
		{
			if ( boost::filesystem::is_directory( dir_itr->status() ) )
			{
				// Note, for path the "/" operator is overloaded to append to the path
				directoryContents[directory_file].push_back(dir_itr->path());
#ifdef GJS_DEBUG_PRINT				
				std::cout << dir_itr->path().filename() << " [directory]" << std::endl;
#endif
			}
			else if ( boost::filesystem::is_regular_file( dir_itr->status() ) )
			{
				directoryContents[regular_file].push_back(dir_itr->path());
#ifdef GJS_DEBUG_PRINT				
				std::cout << "Found regular file: " << dir_itr->path().filename() << std::endl;
#endif
			}
			else
			{
				directoryContents[other_file].push_back(dir_itr->path());
#ifdef GJS_DEBUG_PRINT				
				std::cout << dir_itr->path().filename() << " [other]" << std::endl;
#endif
			}

		}
		catch ( const std::exception & ex )
		{
			std::cerr << dir_itr->path().filename() << " " << ex.what() << std::endl;
		}
	}
	
	return directoryContents;
}