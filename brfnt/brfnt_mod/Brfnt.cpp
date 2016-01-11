#include "Brfnt.h"
#include <assert.h>

u32 ce(u32 i){
    u8 *p = (u8*)&i;
    return (p[0]<<24)|
        (p[1]<<16)|
        (p[2]<<8)|
        p[3];
}
u16 ce(u16 i){
    u8 *p = (u8*)&i;
    return
        (p[0]<<8)|
        p[1];
}

Brfnt::Brfnt(){
}


Brfnt::~Brfnt(){
}

void Brfnt::Load(FILE* f){
    fseek(f,0,SEEK_SET);
    fread(&rfnt,sizeof(rfnt),1,f);
    assert(rfnt.mgc==mgcRFNT);

    fseek(f,ce(rfnt.FINFoff),SEEK_SET);
    fread(&finf,sizeof(finf),1,f);
    assert(finf.mgc==mgcFINF);

    fseek(f,ce(finf.TGLPOffPlus8)-8,SEEK_SET);
    fread(&tglp,sizeof(tglp),1,f);
    assert(tglp.mgc==mgcTGLP);
    assert(ftell(f)==ce(tglp.dataOff));

    assert(ce(tglp.format)==0);

    vector<vector<u32>> pages(ce(tglp.pageCount));
    for(int ii = 0; ii<ce(tglp.pageCount); ++ii){
        pages[ii].resize(ce(tglp.pageWidth)*ce(tglp.pageHeight));
        for(int biy = 0; biy<ce(tglp.pageHeight)/8; ++biy){
            for(int bix = 0; bix<ce(tglp.pageWidth)/8; ++bix){
                for(int piy = 0; piy<8; ++piy){
                    for(int pix2 = 0; pix2<4; ++pix2){
                        u8 d,d1,d2;
                        fread(&d,1,1,f);
                        d2 = (d&0xF)*0x11;
                        d1 = (d>>4)*0x11;
                        pages[ii][
                            bix*8+pix2*2+
                                (biy*8+piy)*ce(tglp.pageWidth)
                        ] = d1;
                        pages[ii][
                            bix*8+pix2*2+1+
                                (biy*8+piy)*ce(tglp.pageWidth)
                        ] = d2;
                    }
                }
            }
        }
    }

    fseek(f,ce(finf.CWDHoffPlus8)-8,SEEK_SET);
    CWDH cwdh;
    fread(&cwdh,sizeof(cwdh),1,f);
    assert(cwdh.mgc==mgcCWDH);
    glyphs.resize(ce(cwdh.charCountm1)+1);
    cw = finf.width+1;
    ch = finf.height+1;
    for(u32 i = 0; i<ce(cwdh.charCountm1)+1; ++i){
        glyphs[i].pixels.resize(cw*ch);
        fread(&glyphs[i].wdhA,1,1,f);
        fread(&glyphs[i].wdhB,1,1,f);
        fread(&glyphs[i].wdhC,1,1,f);
        int pageId = i/(ce(tglp.charPerRow)*ce(tglp.charPerCol));
        int offInPage = i%(ce(tglp.charPerRow)*ce(tglp.charPerCol));
        int coli = offInPage%ce(tglp.charPerRow);
        int rowi = offInPage/ce(tglp.charPerRow);
        for(int x = 0; x<cw; ++x){
            for(int y = 0; y<ch; ++y){
                glyphs[i].pixels[x+y*cw] =
                    pages[pageId][
                        coli*cw+x+
                            (rowi*ch+y)*ce(tglp.pageWidth)
                    ];
            }
        }
    }

    continuousMaps.clear();
    discreteMaps.clear();
    fseek(f,ce(finf.CMAPoffPlus8)-8,SEEK_SET);
    while(1){
        u32 mgc;
        fread(&mgc,4,1,f); assert(mgc==mgcCMAP);
        fseek(f,4,SEEK_CUR);
        u16 firstUtf,lastUtf;
        fread(&firstUtf,2,1,f); firstUtf = ce(firstUtf);
        fread(&lastUtf,2,1,f); lastUtf = ce(lastUtf);
        fseek(f,4,SEEK_CUR);
        u32 nextCMAPoffPlus8;
        fread(&nextCMAPoffPlus8,4,1,f);
        if(lastUtf!=0xFFFF){
            u16 glyphId;
            fread(&glyphId,2,1,f); glyphId = ce(glyphId);
            continuousMaps.push_back(ContinuousMap{
                firstUtf,lastUtf,glyphId
            });
        } else{
            u16 count;
            fread(&count,2,1,f); count = ce(count);
            DiscreteMap disM;
            for(int i = 0; i<count; ++i){
                fread(&disM,4,1,f);
                disM.Utf = ce(disM.Utf);
                disM.glyphId = ce(disM.glyphId);
                discreteMaps.push_back(disM);
            }
        }
        if(!ce(nextCMAPoffPlus8))break;
        fseek(f,ce(nextCMAPoffPlus8)-8,SEEK_SET);
    }

}

void Brfnt::PrintGlyph(u32 id){
    for(int y = 0; y<ch; ++y){
        for(int x = 0; x<cw; ++x){
            putchar(glyphs[id].pixels[x+y*cw] ? '*' : ' ');
        }
        puts("");
    }
}
void Brfnt::Print(u16 utf){
    for(auto& cm:continuousMaps){
        if(utf>=cm.firstUtf && utf<=cm.lastUtf){
            PrintGlyph(utf-cm.firstUtf+cm.glyphId);
            break;
        }
    }

    for(auto& dm:discreteMaps){
        if(utf==dm.Utf){
            PrintGlyph(dm.glyphId);
            break;
        }
    }
}

void Brfnt::Save(FILE* f){
    tglp.pageCount = ce((u16)(1+glyphs.size()/(ce(tglp.charPerCol)*ce(tglp.charPerRow))));

    vector<vector<u32>> pages(ce(tglp.pageCount));
    for(int ii = 0; ii<ce(tglp.pageCount); ++ii){
        pages[ii].resize(ce(tglp.pageWidth)*ce(tglp.pageHeight));
        memset(pages[ii].data(),0,ce(tglp.pageWidth)*ce(tglp.pageHeight)*4);
    }

    for(u32 i = 0; i<glyphs.size(); ++i){
        int pageId = i/(ce(tglp.charPerRow)*ce(tglp.charPerCol));
        int offInPage = i%(ce(tglp.charPerRow)*ce(tglp.charPerCol));
        int coli = offInPage%ce(tglp.charPerRow);
        int rowi = offInPage/ce(tglp.charPerRow);
        for(int x = 0; x<cw; ++x){
            for(int y = 0; y<ch; ++y){
                pages[pageId][
                        coli*cw+x+
                            (rowi*ch+y)*ce(tglp.pageWidth)
                    ] = glyphs[i].pixels[x+y*cw];
            }
        }
    }

    fseek(f,0,SEEK_SET);
    //skip RFNT and FINF
    fseek(f,0x30,SEEK_CUR);
    tglp.secLen = ce((u32)(sizeof(tglp)+ce(tglp.pageCount)*ce(tglp.pageLen)));
    fwrite(&tglp,sizeof(tglp),1,f);
    for(int ii = 0; ii<ce(tglp.pageCount); ++ii){
        for(int biy = 0; biy<ce(tglp.pageHeight)/8; ++biy){
            for(int bix = 0; bix<ce(tglp.pageWidth)/8; ++bix){
                for(int piy = 0; piy<8; ++piy){
                    for(int pix2 = 0; pix2<4; ++pix2){
                        u8 d,d1,d2;
                        d1 = pages[ii][
                            bix*8+pix2*2+
                                (biy*8+piy)*ce(tglp.pageWidth)
                        ];
                        d2 = pages[ii][
                            bix*8+pix2*2+1+
                                (biy*8+piy)*ce(tglp.pageWidth)
                        ];
                        d = (d2&0xF)|((d1&0xF)<<4);
                        fwrite(&d,1,1,f);
                    }
                }
            }
        }
    }

    finf.CWDHoffPlus8 = ce((u32)(ftell(f)+8));
    CWDH cwdh;
    cwdh.mgc = mgcCWDH;
    cwdh.charCountm1 = ce((u32)(glyphs.size()-1));
    cwdh.zero = 0;
    u32 secLen = glyphs.size()*3+0x10;
    u32 padding = secLen%8 ? 8-secLen%8 : 0;
    secLen += padding;
    cwdh.secLen = ce(secLen);
    fwrite(&cwdh,sizeof(cwdh),1,f);
    for(u32 i = 0; i<glyphs.size(); ++i){
        fwrite(&glyphs[i].wdhA,1,1,f);
        fwrite(&glyphs[i].wdhB,1,1,f);
        fwrite(&glyphs[i].wdhC,1,1,f);
    }
    fseek(f,padding,SEEK_CUR);

    finf.CMAPoffPlus8 = ce((u32)(ftell(f)+8));
    u32 nextCMAPoffPlus8;
    u32 b32;
    u16 b16;
    for(u32 i = 0; i<continuousMaps.size(); ++i){
        nextCMAPoffPlus8 = ftell(f)+8+0x18;
        b32 = mgcCMAP;
        fwrite(&b32,4,1,f);
        b32 = ce((u32)0x18);
        fwrite(&b32,4,1,f);
        b16 = ce(continuousMaps[i].firstUtf);
        fwrite(&b16,2,1,f);
        b16 = ce(continuousMaps[i].lastUtf);
        fwrite(&b16,2,1,f);
        b32 = 0;
        fwrite(&b32,4,1,f);
        b32 = ce(nextCMAPoffPlus8);
        fwrite(&b32,4,1,f);
        b16 = ce(continuousMaps[i].glyphId);
        fwrite(&b16,2,1,f);
        b16 = 0;
        fwrite(&b16,2,1,f);
    }
    b32 = mgcCMAP;
    fwrite(&b32,4,1,f);
    u32 cmapLen = 0x16 + 4 * discreteMaps.size();
    padding=cmapLen%8 ? 8-cmapLen%8 : 0;
    cmapLen += padding;
    b32 = ce(cmapLen);
    fwrite(&b32,4,1,f);
    b16 = 0;
    fwrite(&b16,2,1,f);
    b16 = 0xFFFF;
    fwrite(&b16,2,1,f);
    b32 = ce((u32)0x00020000);
    fwrite(&b32,4,1,f);
    b32 = 0;
    fwrite(&b32,4,1,f);
    b16 = ce((u16)discreteMaps.size());
    fwrite(&b16,2,1,f);
    for(u32 i = 0; i<discreteMaps.size(); ++i){
        b16 = ce(discreteMaps[i].Utf);
        fwrite(&b16,2,1,f);
        b16 = ce(discreteMaps[i].glyphId);
        fwrite(&b16,2,1,f);
    }
    u8 zero = 0;
    for(int i = 0; i<padding; ++i)fwrite(&zero,1,1,f);
    rfnt.fileLen = ce((u32)ftell(f));
    fseek(f,0,SEEK_SET);
    rfnt.secCount = ce((u16)(4+continuousMaps.size()));
    fwrite(&rfnt,sizeof(rfnt),1,f);
    fwrite(&finf,sizeof(finf),1,f);
}