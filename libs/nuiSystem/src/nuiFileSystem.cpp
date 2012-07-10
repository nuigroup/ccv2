#include "nuiFileSystem.hpp"
#include "boost/regex.hpp"
#include "boost/filesystem.hpp"
#include <string.h>

using namespace boost::filesystem3;
using namespace nuiFileSystem;
using namespace boost;

list<nuiFile>* nuiFilesystem::read_directory(
    char* directory, 
    char* pattern)
{
    boost::regex expression (pattern);
    path dir (directory);

    list<nuiFile>* files = new list<nuiFile>();

    if(exists(dir))
    {
        for (directory_iterator d = directory_iterator(dir) ; d != directory_iterator() ; d++)
        {
            path p = d->path();
            string _path = p.string();
            if(regex_match(_path, expression) && is_regular_file(p))
            {
                if(p.is_relative())
                    p = complete(p);
                
                nuiFile* file = new nuiFile;

                string fullName = p.string();
                string shortName = p.filename().string();
                
                file->fullName = new char[fullName.length()];
                file->name = new char[shortName.length()];
                strcpy(file->fullName, fullName.c_str());
                strcpy(file->name, shortName.c_str());
                file->last_write = last_write_time(p);

                files->push_back(*file);
            }
        }
    }

    return files;
}