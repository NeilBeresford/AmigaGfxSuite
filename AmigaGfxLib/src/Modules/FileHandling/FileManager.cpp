/**----------------------------------------------------------------------------

    @file       FileManager.cpp
    @defgroup   AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      File Manager Module
    @copyright  Neil Bereford 2024

Notes:

-----------------------------------------------------------------------------*/
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "../../../inc/Modules/FileHandling/FileManager.h"

//-----------------------------------------------------------------------------
// Namesapce
//-----------------------------------------------------------------------------

namespace AmigaGfx
{

//-----------------------------------------------------------------------------
// Class Support Functions
//-----------------------------------------------------------------------------

// Constructors and Destructors -----------------------------------------------

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      Constructor for the FileManager class

  --------------------------------------------------------------------------*/
FileManager::FileManager()
{
    fileCount  = 0;
    nextFileID = 0;
    fileList.clear();
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      Destructor for the FileManager class

  --------------------------------------------------------------------------*/
FileManager::~FileManager()
{
}

// Public Functions ----------------------------------------------------------

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      Open a file and read the data into memory

    @param      fileName - Full path and file name to open
    @param      fileData - Vector to store the file data
    @return     bool - True if the file was opened and read successfully

  --------------------------------------------------------------------------*/
bool FileManager::OpenFile( const std::string& fileName, std::vector<uint8_t>& fileData )
{
    bool result = false;

    // Open the file
    std::ifstream file( fileName, std::ios::binary );

    if ( file.is_open() )
    {
        // Get the file size
        file.seekg( 0, std::ios::end );
        size_t fileSize = file.tellg();
        file.seekg( 0, std::ios::beg );

        // Read the file into memory
        fileData.resize( fileSize );
        file.read( reinterpret_cast<char*>( fileData.data() ), fileSize );

        // Close the file
        file.close();

        result = true;
    }

    return result;
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      Save a file from memory to disk

    @param      fileName - Full path and file name to save
    @param      fileData - Vector containing the file data
    @return     bool - True if the file was saved successfully

  --------------------------------------------------------------------------*/
bool FileManager::SaveFile( const std::string& fileName, std::vector<uint8_t>& fileData )
{
    bool result = false;

    // Open the file
    std::ofstream file( fileName, std::ios::binary );

    if ( file.is_open() )
    {
        // Write the file into memory
        file.write( reinterpret_cast<char*>( fileData.data() ), fileData.size() );

        // Close the file
        file.close();

        result = true;
    }

    return result;
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      List all files in a directory

    @param      pathName - Full path to the directory
    @return     uint32_t - Number of files listed

  --------------------------------------------------------------------------*/
uint32_t FileManager::listAllFiles( const std::string& pathName )
{
    // List all files in the directory
    std::string              path = pathName;
    std::vector<std::string> directorys;

    directorys.push_back( pathName );

    fileNames.clear();

    while ( directorys.size() > 0 )
    {
        path = directorys.back();
        directorys.pop_back();

        for ( const auto& entry : std::filesystem::directory_iterator( path ) )
        {
            if ( entry.is_directory() )
            {
                directorys.push_back( entry.path().string() );
            }
            else
            {
                fileNames.push_back( entry.path().string() );
            }
        }
    }

    return fileNames.size();
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      Process the file list

    @param      fileIndex - Index of the file to process
    @return     std::string - File name

  --------------------------------------------------------------------------*/
std::string FileManager::processFileList( uint32_t fileIndex )
{
    if ( fileIndex >= 0 && fileIndex < fileNames.size() )
        return fileNames[ fileIndex ];
    else
        return "ERROR";
}

//-----------------------------------------------------------------------------

} // namespace AmigaGfx

//-----------------------------------------------------------------------------
// End of file: FileManager.cpp
//-----------------------------------------------------------------------------
