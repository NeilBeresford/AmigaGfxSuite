/**----------------------------------------------------------------------------

    @file       Tools.cpp
    @defgroup   AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Handles error messages for the USB tool.

    @copyright  Neil Bereford 2024

Notes:


-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------------

#include <png.h>
#include <zlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "../../../inc/Modules/Utilities/Tools.h"

//-----------------------------------------------------------------------------
// Namespace
//-----------------------------------------------------------------------------

namespace AmigaGfx
{

//-----------------------------------------------------------------------------
// Class support functions
// ----------------------------------------------------------------------------

// Constructor and destructor  -------------------------------------------------

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Constructor for the Tools class

  --------------------------------------------------------------------------*/
Tools::Tools()
{
}
//-----------------------------------------------------------------------------
/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Destructor for the Tools class

  --------------------------------------------------------------------------*/
Tools::~Tools()
{
}

//-----------------------------------------------------------------------------

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Calculates the CRC for the data
    @param      pData - Pointer to the data
    @param      len - Length of the data to calculate the CRC for
    @return     uint16_t - CRC16 value
  --------------------------------------------------------------------------*/
uint16_t Tools::crc16( uint8_t* pData, uint32_t len ) const
{
    uint16_t    crc = CRC_START;
    uint8_t     i;

    png_uint_32 version = png_access_version_number();

    while ( len-- )
    {
        crc ^= *pData++;
        for ( i = 0; i < CRC_BITS; i++ )
        {
            if ( crc & CRC_MASK )
            {
                crc >>= CRC_SHIFT;
                crc ^= CRC_POLY;
            }
            else
            {
                crc >>= CRC_SHIFT;
            }
        }
    }
    return crc;
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Reads PNG file - PLEASE note this will only read 8bit indexed
                PNG files. The image data is saved to disk in RAW format and
                the sprite data is compressed and saved to disk. The palette
                is saved in a format used by the Apollo V4.
    @param      file_name - Pointer to the file name
  --------------------------------------------------------------------------*/
void Tools::Read_PNG( const char* file_name, uint32_t sprWidth, uint32_t sprHeight )
{
    FILE*       fp      = fopen( file_name, "rb" );
    png_structp png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );

    // Read in the PNG file
    if ( !fp )
    {
        throw std::runtime_error( "Failed to open file for reading" );
    }

    info_ptr = png_create_info_struct( png_ptr ); // <-- creating a new, local info_ptr
    png_init_io( png_ptr, fp );
    png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL );
    row_pointers = png_get_rows( png_ptr, info_ptr );

    // Get the image information
    uint32_t picWidth   = png_get_image_width( png_ptr, info_ptr );
    uint32_t picHeight  = png_get_image_height( png_ptr, info_ptr );
    uint32_t picColours = png_get_palette_max( png_ptr, info_ptr );

    //-------------------------------------------------------------------------
    // PArt one - get the palette and save it in a format used by the Apollo V4
    //-------------------------------------------------------------------------
    png_colorp palette;
    int        num_palette;

    png_get_PLTE( png_ptr, info_ptr, &palette, &num_palette );
    std::vector<png_color> color_palette( palette, palette + num_palette );

    // Save the palette in a format used for the Apollo V4
    Save_ApolloV4_Palette( color_palette, "palette.bin" );

    //-------------------------------------------------------------------------
    // Part two - get the image data and save it in RAW format
    // -------------------------------------------------------------------------
    // At this point row_pointers contain all the lines of the image in a raw format
    // get the image raw data for saving
    std::vector<uint8_t> rawData( picWidth * picHeight );
    for ( uint32_t y = 0; y < picHeight; y++ )
    {
        for ( uint32_t x = 0; x < picWidth; x++ )
        {
            uint8_t* pLine              = ( (uint8_t**)row_pointers )[ y ];
            rawData[ y * picWidth + x ] = pLine[ x ];
        }
    }

    // create file name and save data to disk
    std::string rawName2( file_name );

    std::string rawName( file_name );
    rawName += std::format( "-{0}-{1}.RAW", picWidth, picHeight );

    // Crude check for the sprite height
    uint32_t sprH = picHeight;
    if ( picWidth == 60 && picHeight > picWidth * 2 )
        sprH = picWidth;

    Save_Vector_To_File( rawData, rawName );

    //-------------------------------------------------------------------------
    // Part three - get the sprite data and save it in compressed format
    // -------------------------------------------------------------------------
    // Compress and save the sprite data

    // if ( picHeight > ( picWidth * 4 ) )

    CompressSpriteData( rawData, picWidth, picHeight, picWidth, sprH, rawName2 );

    //-------------------------------------------------------------------------
    // tidy up before exiting
    png_destroy_read_struct( &png_ptr, NULL, NULL ); // <-- destroying the info_ptr (as well).
    fclose( fp );
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Saves the Apollo V4 palette to disk
    @param      palette - Pointer to the palette
    @param      filename - Pointer to the file name
  --------------------------------------------------------------------------*/
void Tools::Save_ApolloV4_Palette( std::vector<png_color>& palette, const std::string& filename )
{
    std::ofstream file( filename, std::ios::binary );

    uint32_t      nSaveVal = palette.size();
    uint8_t       nIndex   = 0;

    if ( !file.is_open() )
    {
        throw std::runtime_error( "Failed to open file for writing" );
    }

    // first the number of colours
    file.write( (char*)&nSaveVal, sizeof( uint32_t ) );

    // now the palette
    for ( auto& color : palette )
    {
        // save the index and colour values in the format used by the Apollo V4
        nSaveVal = nIndex | color.red << 8 | color.green << 16 | color.blue << 24;
        file.write( (char*)&nSaveVal, sizeof( uint32_t ) );
        nIndex++;
    }

    if ( !file )
    {
        throw std::runtime_error( "Failed to write data to file" );
    }

    file.close();
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Compress the RAW image data into compressed sprite data
                and save to disk.
    @param      data - Pointer to the data
    @param      w - Width of the image
    @param      h - Height of the image
    @param      sprW - Width of the sprite
    @param      sprH - Height of the sprite
  --------------------------------------------------------------------------*/
void Tools::CompressSpriteData( std::vector<uint8_t>& data, uint32_t w, uint32_t h, uint32_t sprW, uint32_t sprH, std::string fileName )
{
    std::vector<uint32_t> sprOffsets;
    std::vector<uint8_t>  sprData;
    uint32_t              sprDataStart = 0;
    uint32_t              sprCount     = 0;
    uint32_t              tmpValue     = 0;

    // calculate the offsets
    for ( uint32_t sprDy = 0; sprDy < h; sprDy += sprH )
    {
        for ( uint32_t sprDx = 0; sprDx < w; sprDx += sprW )
        {

            sprDataStart = sprData.size();
            sprOffsets.push_back( sprDataStart );

            for ( uint32_t y = 0; y < sprH; y++ )
            {
                bool     bLoop = true;

                uint32_t x     = 0;
                while ( bLoop )
                {
                    uint8_t sprCmd = 0;
                    // sprCMD is offset to start of drawable data (0-199)
                    // sprCMD 200 = skip 200 pixels, no data
                    // sprCMD 201 = skip line
                    // sprCMD 255 = end of sprite
                    // scan for the first non transparent pixel
                    for ( uint32_t scanBG = 0; scanBG <= 200; scanBG++ )
                    {
                        if ( scanBG + x == ( sprW ) )
                        {
                            x      = sprW;
                            sprCmd = 201;
                            sprData.push_back( sprCmd );
                            bLoop = false;
                            break;
                        }

                        if ( ( data[ ( sprDy + y ) * w + sprDx + x + scanBG ] != 0 ) )
                        {
                            x += scanBG;
                            sprCmd = scanBG;
                            sprData.push_back( sprCmd );
                            break;
                        }

                        if ( scanBG == 200 )
                        {
                            x += scanBG;
                            sprCmd = scanBG;
                            sprData.push_back( sprCmd );
                            break;
                        }
                    }

                    // write the run of pixel data
                    if ( sprCmd < 200 )
                    {
                        // SprCMD is the offset to the start of the pixel data
                        // search for the end of the run of pixel data
                        // and write the length of the data length
                        tmpValue = 0;
                        while ( x + tmpValue < sprW && data[ ( sprDy + y ) * w + sprDx + x + tmpValue ] != 0 )
                        {
                            tmpValue++;
                        }
                        sprData.push_back( tmpValue );

                        while ( tmpValue )
                        {
                            sprData.push_back( data[ ( sprDy + y ) * w + sprDx + x ] );
                            x++;
                            tmpValue--;
                        }
                    }
                }
            }

            // stored the compressed sprite data
            sprData.push_back( 255 );
            sprCount++;
        }
    }

    // save the compressed sprite data to disk...
    fileName += ".SPR";
    std::ofstream file( fileName, std::ios::binary );

    if ( !file.is_open() )
    {
        throw std::runtime_error( "Failed to open file for writing" );
    }

    file << "SPRITEDATA:" << sprCount << "," << sprW << "," << sprH << ":";
    file.write( (char*)sprOffsets.data(), sprOffsets.size() * sizeof( uint32_t ) );
    file.write( (char*)sprData.data(), sprData.size() );
    if ( !file )
    {
        throw std::runtime_error( "Failed to write data to file" );
    }
    file.close();
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Saves the vector to disk
    @param      file_name - Pointer to the file name
  --------------------------------------------------------------------------*/
void Tools::Save_Vector_To_File( const std::vector<uint8_t>& vData, const std::string& filename )
{
    std::ofstream file( filename, std::ios::binary );
    if ( !file.is_open() )
    {
        throw std::runtime_error( "Failed to open file for writing" );
    }

    file.write( (char*)vData.data(), vData.size() );
    if ( !file )
    {
        throw std::runtime_error( "Failed to write data to file" );
    }

    file.close();
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Writes PNG file, please note at present the image needs to be
                opened via Read_PNG first.
    @param      file_name - Pointer to the file name
  --------------------------------------------------------------------------*/
void Tools::Write_PNG( const char* file_name )
{
    FILE*       fp      = fopen( file_name, "wb" );
    png_structp png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    png_init_io( png_ptr, fp );
    png_set_rows( png_ptr, info_ptr, row_pointers );
    png_write_png( png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL );
    png_destroy_write_struct( &png_ptr, &info_ptr );
    fclose( fp );
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Checks image is 8bit indexed. This will open and close the file
                once the check has been made.
    @param      filename - Pointer to the file name
  --------------------------------------------------------------------------*/
bool Tools::Check_8bitIndexed_PNG( const char* filename )
{
    FILE* fp     = fopen( filename, "rb" );
    bool  result = false;

    if ( !fp )
    {
        // perror( "File opening failed" );
        return result;
    }

    png_structp png = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    if ( !png )
    {
        fclose( fp );
        return result;
    }

    png_infop info = png_create_info_struct( png );
    if ( !info )
    {
        png_destroy_read_struct( &png, NULL, NULL );
        fclose( fp );
        return result;
    }

    if ( setjmp( png_jmpbuf( png ) ) )
    {
        png_destroy_read_struct( &png, &info, NULL );
        fclose( fp );
        return result;
    }

    png_init_io( png, fp );
    png_read_info( png, info );

    if ( png_get_color_type( png, info ) != PNG_COLOR_TYPE_PALETTE )
    {
        // printf( "Image is not 8-bit indexed color\n" );
    }
    else
    {
        result = true;
        // printf( "Image is 8-bit indexed color\n" );
    }

    png_destroy_read_struct( &png, &info, NULL );
    fclose( fp );

    return result;
}

/**--------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Merge the palettes
    @param      paletteTo - Pointer to the destination palette
    @param      paletteFrom - Pointer to the source palette
    @param      ToStart - Start index of the destination palette
    @param      FromStart - Start index of the source palette
    @param      FromSize - Size of the source palette
    -----------------------------------------------------------------------*/
bool Tools::MergePalettes( std::vector<uint8_t>& paletteTo, std::vector<uint8_t>& paletteFrom, uint32_t ToStart, uint32_t FromStart, uint32_t FromSize )
{
    bool result = false;

    ToStart++;   // skip index
    FromStart++; // skip index

    // convert the indexes to bytes
    ToStart *= sizeof( uint32_t );
    FromStart *= sizeof( uint32_t );

    // check the palettes are the same size
    if ( paletteTo.size() != paletteFrom.size() )
    {
        return result;
    }

    // check the start indexes are within the palette size
    if ( ToStart > paletteTo.size() || FromStart > paletteFrom.size() )
    {
        return result;
    }

    // check the from size is within the palette size
    if ( FromSize > paletteFrom.size() )
    {
        return result;
    }

    result = true;

    // merge the palettes
    for ( uint32_t nIndex = 0; nIndex < FromSize; nIndex++ )
    {
        paletteTo[ ( ToStart + nIndex * 4 ) + 1 ] = paletteFrom[ ( FromStart + nIndex * 4 ) + 1 ]; // R value
        paletteTo[ ( ToStart + nIndex * 4 ) + 2 ] = paletteFrom[ ( FromStart + nIndex * 4 ) + 2 ]; // G value
        paletteTo[ ( ToStart + nIndex * 4 ) + 3 ] = paletteFrom[ ( FromStart + nIndex * 4 ) + 3 ]; // B value
    }

    return result;
}

/**---------------------------------------------------------------------------
    @ingroup    AmigaGfxLIBTools AmigaGfx Library Tools Module
    @brief      Compress the data using zlib.
    @param      pData - Pointer to the source
    @param      len - size of the source
    @param      pOut - Pointer to the destination
    @param      pLenout - Pointer to the size of the destination
  --------------------------------------------------------------------------*/
void Tools::CompressData( unsigned char* pData, uint32_t len, unsigned char* pOut, unsigned long* pLenout )
{
    // Compress the data
    compress( pOut, pLenout, pData, len );
}

//-----------------------------------------------------------------------------

} // namespace AmigaGfx

//-----------------------------------------------------------------------------
// End of file: Tools.cpp
// ----------------------------------------------------------------------------
