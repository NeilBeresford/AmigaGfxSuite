/**----------------------------------------------------------------------------

    @file       Tools.h
    @defgroup   AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Handles error messages for the USB tool.

    @copyright  Neil Bereford 2024

-----------------------------------------------------------------------------*/

#pragma once

//-----------------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------------

#include <string>
#include <chrono>
#include <ctime>
#include <vector>

#include "../Logging/Logger.h"
#include "../ErrorHandling/Errors.h"

//-----------------------------------------------------------------------------
// Namespace
// ----------------------------------------------------------------------------

namespace AmigaGfx
{

//-----------------------------------------------------------------------------
// Class definitions
// ----------------------------------------------------------------------------

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Tools class (Support functionality) for the AmigaGfx Library
  --------------------------------------------------------------------------*/
class Tools
{
  public:
    // Function to access the singleton -----------------------------------------
    static Tools& getInstance()
    {
        static Tools instance; // Created only once
        return instance;
    }

    uint16_t crc16( uint8_t* pData, uint32_t len ) const;

    // Image functions ---------------------------------------------------------
    void Read_PNG( const char* file_name, uint32_t sprWidth, uint32_t sprHeight );
    void Write_PNG( const char* file_name );
    bool Check_8bitIndexed_PNG( const char* filename );
    void Save_ApolloV4_Palette( std::vector<png_color>& palette, const std::string& filename );

    // Compression functions ---------------------------------------------------
    void CompressData( unsigned char* pData, uint32_t len, unsigned char* pOut, unsigned long* pLenout );
    void CompressSpriteData( std::vector<uint8_t>& data, uint32_t w, uint32_t h, uint32_t sprW, uint32_t sprH, std::string fileName );

    // Disk related functions --------------------------------------------------
    void Save_Vector_To_File( const std::vector<uint8_t>& vData, const std::string& filename );

    // palette functions -------------------------------------------------------
    bool MergePalettes( std::vector<uint8_t>& paletteTo, std::vector<uint8_t>& paletteFrom, uint32_t ToStart, uint32_t FromStart, uint32_t FromSize );

  private:
    // Singleton constructor and destructor ------------------------------------
    Tools();
    ~Tools();

    Tools( const Tools& )            = delete;
    Tools& operator=( const Tools& ) = delete;

    // Constants ---------------------------------------------------------------
    // crc16 constants ----------------------------------------------------------
    const uint16_t CRC_START = 0xFFFF; //<! Const values for the crc16 - CRC start value
    const uint16_t CRC_POLY  = 0xA001; //<! Const values for the crc16 - CRC polynomial
    const uint16_t CRC_MASK  = 0x0001; //<! Const values for the crc16 - CRC mask
    const uint16_t CRC_SHIFT = 1;      //<! Const values for the crc16 - CRC shift
    const uint16_t CRC_BITS  = 8;      //<! Const values for the crc16 - CRC bits

    png_infop      info_ptr; // <-- Global info_ptr (good)
    png_bytepp     row_pointers;

    // private functions -------------------------------------------------------

}; // end class Singleton Tools

//-----------------------------------------------------------------------------

} // end namespace AmigaGfx

//-----------------------------------------------------------------------------
// End of file: Tools.h
// ----------------------------------------------------------------------------
