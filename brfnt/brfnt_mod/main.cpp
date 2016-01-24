#define GAMEFONT1_FONT "汉仪黑荔枝体简"
#define GAMEFONT2_FONT "汉仪粗黑简"

#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <string>
#include <Windows.h>
#include "Brfnt.h"
using namespace std;

int fontType;

HDC hDC;
HBITMAP hBmp;
HFONT hFont;
void drawChar(u32* p,int w,int h,u16 utf){
    RECT rc;
    rc.left = rc.top = 0;
    rc.right = w;
    rc.bottom = h;
    FillRect(hDC,&rc,(HBRUSH)GetStockObject(BLACK_BRUSH));
    wchar_t str[2];
    str[1] = 0;
    str[0] = utf;
    if(fontType==1){
        rc.top += 10;
    } else{
        rc.top += 6;
    }
    DrawText(hDC,str,1,&rc,0);
    for(int x = 0; x<w; ++x){
        for(int y = 0; y<h; ++y){
            COLORREF clr = GetPixel(hDC,x,y);
            u8 c = (GetRValue(clr)+GetGValue(clr)+GetBValue(clr))/48;
            p[x+y*w] = c;
        }
    }
}

int main(int argc, char** argv){
    if(argc<4){
        cout<<"Usage: brfnt_mod <base_brfnt> <charset_file> <output_brfnt>";
        return -1;
    }
    cout<<argv[3]<<" < "<<argv[1]<<" + "<<argv[2]<<endl;
    string n;
    Brfnt brfnt;
    //cout<<"input brfnt file:"<<endl;
    //getline(cin,n);
    n = argv[1];
    if(n.find("GameFont1.brfnt")!=string::npos){
        fontType = 1;
    } else fontType = 2;
    FILE* in,*utfs,*out;
    in = fopen(n.data(),"rb");
    brfnt.Load(in);
    fclose(in);

    HWND hDesktop = GetDesktopWindow();
    HDC hDesktopDC = GetDC(hDesktop);
    hDC = CreateCompatibleDC(hDesktopDC);
    hBmp = CreateCompatibleBitmap(hDesktopDC,brfnt.cw,brfnt.ch);
    SelectObject(hDC,hBmp);
    SetTextColor(hDC,RGB(255,255,255));
    ReleaseDC(hDesktop,hDesktopDC);
    SetBkMode(hDC,TRANSPARENT);
    if(fontType==1){
        hFont = CreateFont(36,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,VARIABLE_PITCH,TEXT(GAMEFONT1_FONT));
    } else{
        hFont= CreateFont(24,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,VARIABLE_PITCH,TEXT(GAMEFONT2_FONT));
    }
    SelectObject(hDC,hFont);

    //cout<<"charset file:"<<endl;
    //getline(cin,n);
    n = argv[2];
    utfs = fopen(n.data(),"rb");
    u16 utf;
    while(fread(&utf,2,1,utfs)){
        u8 A,B,C;
        A = 1;
        B = fontType==1 ? 0x1E : 0x18;
        C = fontType==1 ? 0x21 : 0x19;
        u32 *p = brfnt.AddChar(utf,A,B,C);
        drawChar(p,brfnt.cw,brfnt.ch,utf);
    }
    fclose(utfs);
    //cout<<"output brfnt file:"<<endl;
    //getline(cin,n);
    n = argv[3];
    out = fopen(n.data(),"wb");
    brfnt.Save(out);
    fclose(out);
}