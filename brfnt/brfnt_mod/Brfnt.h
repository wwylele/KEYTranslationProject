#pragma once
#include <stdio.h>
#include <vector>
using namespace std;


typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct RFNT{
    u32 mgc;
    u16 bom;
    u16 version;
    u32 fileLen;
    u16 FINFoff;
    u16 secCount;
};
static_assert(sizeof(RFNT)==0x10,"");
#define mgcRFNT 0x544E4652

struct FINF{
    u32 mgc;
    u32 secLen;
    u8 type;
    u8 leading;
    u16 unk;
    u8 leftMargin;
    u8 u1;
    u8 u2;
    u8 u3;
    u32 TGLPOffPlus8;
    u32 CWDHoffPlus8;
    u32 CMAPoffPlus8;
    u8 height;
    u8 width;
    u8 asc;
    u8 dsc;
};
static_assert(sizeof(FINF)==0x20,"");
#define mgcFINF 0x464E4946

struct TGLP{
    u32 mgc;
    u32 secLen;
    u8 fwm1;
    u8 fhm1;
    u8 cwm1;
    u8 chm1;
    u32 pageLen;
    u16 pageCount;
    u16 format;
    u16 charPerRow;
    u16 charPerCol;
    u16 pageWidth;
    u16 pageHeight;
    u32 dataOff;
    u32 u[4];
    //data...
};
static_assert(sizeof(TGLP)==0x30,"");
#define mgcTGLP 0x504C4754

struct CWDH{
    u32 mgc;
    u32 secLen;
    u32 charCountm1;
    u32 zero;
    //data...
};
static_assert(sizeof(CWDH)==0x10,"");
#define mgcCWDH 0x48445743

#define mgcCMAP 0x50414D43
class Brfnt{
    RFNT rfnt;
    FINF finf;
    TGLP tglp;

    int cw,ch;

    struct Glyph{
        vector<u32> pixels;
        u8 wdhA,wdhB,wdhC;
    };
    vector<Glyph> glyphs;

    struct ContinuousMap{
        u16 firstUtf;
        u16 lastUtf;
        u16 glyphId;
    };
    vector<ContinuousMap> continuousMaps;
    struct DiscreteMap{
        u16 Utf;
        u16 glyphId;
    };
    vector<DiscreteMap> discreteMaps;
public:
    Brfnt();
    ~Brfnt();
    void Load(FILE* f);
    void Save(FILE* f);
    void PrintGlyph(u32 id);
    void Print(u16 utf);
};

