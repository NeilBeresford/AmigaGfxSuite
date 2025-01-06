# Amiga Palettte Merge Tool

## What does it do?

The tool will take two 256 colour palette binary files, outputed by AmigaSpriteCompress, and merge one into another.

This merge will slot in a set range of one palette into another at a different index in that palette.


## Example of use 

> AmigaPaletteMerge SourcePalette.bin ToMergePalette.bin 32 0 96

Looking at the above - the following parameters are explained
 
  1 - SourcePalette.bin  - the palette to add the block of colours to.
  2 - ToMergePalette.bin - the palette to take the block of colours from.
  3 - 32 		 - Index colour to copy to
  4 - 0                  - Start of colour block to merge
  5 - 96                 - Size of colour block to merge (in colours )




