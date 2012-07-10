/*******************************************************************************
Author:		Anatoly Lushnikov
Copyright:	(c) 2012 NUI Group
*******************************************************************************/


#ifndef NUI_FILESYSTEM
#define NUI_FILESYSTEM

#include "boost/filesystem.hpp"

using namespace std;
using namespace boost::filesystem3;


namespace nuiFileSystem
{

struct nuiFile;
class nuiFilesystem;

//! structure to hold basic file info
struct nuiFile
{
    time_t last_write;  //! last write time
    char* fullName;     //! full file name
    char* name;         //! short file name
};

//! class to get file information from directory according to pattern
class nuiFilesystem
{
public:
    // returns list of files in directory according to boost regex
    static list<nuiFile>* read_directory(   char* directory, 
                                            char* pattern = "(.*)");
protected:
private:
};

}

#endif

