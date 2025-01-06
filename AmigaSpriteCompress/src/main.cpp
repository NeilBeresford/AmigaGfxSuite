/**----------------------------------------------------------------------------

    @file       main.cpp
    @defgroup   AmigaGfxCalc AmigaGfxCalc
    @brief      Main entry and control point for the AmigaGfxCalc

    @copyright  Neil Beresford 2024

Notes:

    Messy test code, please forgive!

-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>
#include <png.h>

#include "../../AmigaGfxLib/inc/AmigaGfxLib.h"

void main_ScriptedConvert( void );

//-----------------------------------------------------------------------------
// Namespace access
//-----------------------------------------------------------------------------

using namespace AmigaGfx;

//-----------------------------------------------------------------------------
// External Functionality
//-----------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    // This will parse the command line and then
    // Check the command line arguments

    if ( argc == 1 )
    {
        main_ScriptedConvert();
        return EXIT_SUCCESS;
    }

    if ( argc < 4 )
    {
        std::cout << "Usage: AmigaGfxCalc <PNG filename> <sprW> <sprH>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string pngFileName = argv[ 1 ];
    uint32_t    sprWidth    = std::stoi( argv[ 2 ] );
    uint32_t    sprHeight   = std::stoi( argv[ 3 ] );

    // Test for correct PNG format ...
    if ( pngFileName.ends_with( ".png" ) )
    {
        std::cout << "Processing: " << pngFileName << std::endl;
        Tools& tools = Tools::getInstance();
        if ( tools.Check_8bitIndexed_PNG( pngFileName.c_str() ) == false )
        {
            std::cout << "Image " << pngFileName << " is not 8 bit indexed " << std::endl;
            return EXIT_FAILURE;
        }
        tools.Read_PNG( pngFileName.c_str(), sprWidth, sprHeight );
        std::cout << "Finisshed." << std::endl;
    }
    else
    {
        std::cout << "Usage: AmigaGfxCalc <PNG filename> <sprW> <sprH>" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

#if 0

    const char* ImageName    = "D:/NewProjects/Amiga/AmigaGfxSuit/Trunk/Files/SPRITE-0001.png";
    const char* OutImageName = "D:/NewProjects/Amiga/AmigaGfxSuit/Trunk/Files/waccusedOut.png";

    // Test code ...
    std::cout << "AmigaSpriteCompress" << std::endl;

    // get the tools instance
    Tools&      tools = Tools::getInstance();
    FileManager fileManager;

    uint32_t numFiles = fileManager.listAllFiles( "D:/Emulation/Worms/Sprites - Copy" );

    for ( uint32_t nIndex = 0; nIndex < numFiles; nIndex++ )
    {
        std::string fileName = fileManager.processFileList( nIndex );

        if ( fileName.ends_with( ".png" ) )
        {
            std::cout << "Processing: " << fileName << std::endl;
            tools.Read_PNG( fileName.c_str() );
            tools.Write_PNG( fileName.c_str() );
        }
    }



    //-------------------------------------------------------------------------
    // Test the PNG library
    //-------------------------------------------------------------------------
    std::cout << "Testing PNG Library..." << std::endl;

    std::cout << "Check image is 8bit indexed..." << std::endl;
    if ( tools.Check_8bitIndexed_PNG( ImageName ) == false )
    {
        std::cout << "Image " << ImageName << " is not 8 bit indexed " << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Loading image..." << std::endl;
    tools.Read_PNG( ImageName );
    std::cout << "Saving image..." << std::endl;
    tools.Write_PNG( OutImageName );

    //-------------------------------------------------------------------------
#if 0
    //-------------------------------------------------------------------------
    // Test the ZLIB library
    //-------------------------------------------------------------------------
    std::cout << "Testing ZLIB compress..." << std::endl;
    // ... load in a file
    std::cout << "Loading file..." << std::endl;
    using namespace std;
    ifstream file( "D:/NewProjects/Amiga/AmigaGfxSuit/Trunk/Files/SPRITE-0001.png", ios::in | ios::binary );
    if ( !file )
    {
        cerr << "Can't open file " << endl;
        return 1;
    }
    // compress it
    vector<unsigned char> data( ( istreambuf_iterator<char>( file ) ), istreambuf_iterator<char>() );
    unsigned long         len = data.size();
    tools.CompressData( &data[ 0 ], data.size(), &data[ 0 ], &len );
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // report the results
    //-------------------------------------------------------------------------
    std::cout << "Compression: " << data.size() << " to " << len << " ( " << ( (float)len / (float)data.size() ) * 100.0f << "% )" << std::endl;
    std::cout << "End Tests" << std::endl;
    //-------------------------------------------------------------------------
#endif

    return EXIT_SUCCESS;
#endif
}

//-----------------------------------------------------------------------------
// Internal Functionality
//-----------------------------------------------------------------------------

void main_ScriptedConvert( void )
{
    // Test code ...
    std::cout << "AmigaSpriteCompress" << std::endl;

    // get the tools instance
    Tools&      tools = Tools::getInstance();
    FileManager fileManager;

    uint32_t    numFiles = fileManager.listAllFiles( "./" );

    for ( uint32_t nIndex = 0; nIndex < numFiles; nIndex++ )
    {
        std::string fileName = fileManager.processFileList( nIndex );

        if ( fileName.ends_with( ".png" ) )
        {
            std::cout << "Processing: " << fileName << std::endl;
            tools.Read_PNG( fileName.c_str(), 60, 60 );
            tools.Write_PNG( fileName.c_str() );
        }
    }
}

//-----------------------------------------------------------------------------
// End of file: main.cpp
// ----------------------------------------------------------------------------
