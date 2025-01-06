/**----------------------------------------------------------------------------

    @file       FileManager.h
    @defgroup   AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      File Manager Module
    @copyright  Neil Bereford 2024

-----------------------------------------------------------------------------*/
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

//-----------------------------------------------------------------------------
// Namesapce
//-----------------------------------------------------------------------------

namespace AmigaGfx
{

//-----------------------------------------------------------------------------
// Clsss Definitions
//-----------------------------------------------------------------------------

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      File state machine states
  --------------------------------------------------------------------------*/
typedef enum
{
    IDLE = 0,    //!< 0 - structure is idle and free for use
    OPENED,      //!< 1 - File has been opened
    READ,        //!< 2 - File has been read
    ERRORED,     //!< 3 - Errored state
    TOTAL_STATES //!< 4 - Total States

} TE_FILE_STATE;

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      FILE_DATA structure stores the file data
                and the file state machine

  --------------------------------------------------------------------------*/
typedef struct
{
    std::string          fileName;   //!< Full File Name
    std::ifstream        fileHandle; //!< File Handle, when opened
    std::vector<uint8_t> fileData;   //!< File Data, stored as bytes
    TE_FILE_STATE        fileState;  //!< File State Machine
    uint32_t             fileSize;   //!< File Size in bytes
    uint32_t             fileID;     //!< Unique File ID

} TS_FILE_DATA, *PTS_FILE_DATA;

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBFile AmigaGfx Library File Module
    @brief      File Manager Class manages the files with their data
  --------------------------------------------------------------------------*/
class FileManager
{
  public:
    // Public Methods -------------------------------------------------------
    // Constructor / Destructor ---------------------------------------------
    FileManager();
    ~FileManager();
    // File Handling --------------------------------------------------------
    bool        OpenFile( const std::string& fileName, std::vector<uint8_t>& fileData );
    bool        SaveFile( const std::string& fileName, std::vector<uint8_t>& fileData );
    uint32_t    listAllFiles( const std::string& pathName );
    std::string processFileList( uint32_t fileIndex );

  private:
    // Private Data -------------------------------------------------------
    std::vector<TS_FILE_DATA> fileList;   //!< List of files
    std::vector<std::string>  fileNames;  //!< File Data
    uint32_t                  nextFileID; //!< Unique File ID used for next file
    uint32_t                  fileCount;  //!< Total number of files handled
};

//-----------------------------------------------------------------------------

} // namespace AmigaGfx

//-----------------------------------------------------------------------------
// End of file: FileManager.h
//-----------------------------------------------------------------------------
