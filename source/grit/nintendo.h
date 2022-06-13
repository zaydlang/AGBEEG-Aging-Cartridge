
//{{BLOCK(nintendo)

//======================================================================
//
//	nintendo, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 528 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x32 
//	Total size: 512 + 33792 + 2048 = 36352
//
//	Time-stamp: 2022-06-12, 22:13:13
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.16
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_NINTENDO_H
#define GRIT_NINTENDO_H

#define nintendoTilesLen 33792
extern const unsigned int nintendoTiles[8448];

#define nintendoMapLen 2048
extern const unsigned short nintendoMap[1024];

#define nintendoPalLen 512
extern const unsigned short nintendoPal[256];

#endif // GRIT_NINTENDO_H

//}}BLOCK(nintendo)
