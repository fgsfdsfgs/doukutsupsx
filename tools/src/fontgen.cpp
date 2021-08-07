#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FONT_COLS 16
#define FONT_ROWS_ANSI 6
#define FONT_ROWS_SJIS 6

#ifndef _WIN32
#error "This program requires GDI for the backwards ass font rendering."
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ddraw.h>

static LPDIRECTDRAW ddraw;
static LPDIRECTDRAWSURFACE surf;

static void cleanup(void) {
  if (surf) surf->Release();
  if (ddraw) ddraw->Release();
}

int main(int argc, char **argv) {
  if (argc < 6) {
    printf("usage: fontgen -j|-a <fontname> <charwidth> <charheight> <outfile>\n");
    return -1;
  }

  const DWORD charset = !strcmp(argv[1], "-j") ? SHIFTJIS_CHARSET : ANSI_CHARSET;
  const char *fntname = argv[2];
  const char *outfile = argv[5];
  int chw = atoi(argv[3]);
  int chh = atoi(argv[4]);
  const int chrows = (charset == SHIFTJIS_CHARSET) ? FONT_ROWS_SJIS : FONT_ROWS_ANSI;

  if (chw <= 0 || chw > 32) chw = 6;
  if (chh <= 0 || chh > 32) chh = 12;

  if (DirectDrawCreate(NULL, &ddraw, NULL) != DD_OK) {
    fprintf(stderr, "error: could not create ddraw context\n");
    return -2;
  }

  ddraw->SetCooperativeLevel(NULL, DDSCL_NORMAL);

  printf("%s %dx%d\n", fntname, chw, chh);

  atexit(cleanup);

  DDSURFACEDESC desc;
  memset(&desc, 0, sizeof(desc));
  desc.dwSize = sizeof(desc);
  desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
  desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
  desc.dwWidth = FONT_COLS * chw;
  desc.dwHeight = chrows * chh;
  const HRESULT res = ddraw->CreateSurface(&desc, &surf, NULL);
  if (res != DD_OK) {
    fprintf(stderr, "error: could not create ddraw surface: %08x\n", res);
    return -3;
  }

  DDCOLORKEY ddcolorkey;
  ddcolorkey.dwColorSpaceLowValue = 0;
  ddcolorkey.dwColorSpaceHighValue = 0;
  surf->SetColorKey(DDCKEY_SRCBLT, &ddcolorkey);

  const DWORD quality = NONANTIALIASED_QUALITY;
  HFONT font = CreateFontA(chh, chw, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, charset, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, quality, FIXED_PITCH | FF_DONTCARE, fntname);
  if (!font) {
    fprintf(stderr, "error: could not create font with the specified properties\n");
    return -4;
  }

  const DWORD color = 0x00FFFFFF;
  const DWORD fillcolor = 0x00000000;
  HDC hdc;

  DDBLTFX ddbltfx;
  memset(&ddbltfx, 0, sizeof(ddbltfx));
  ddbltfx.dwSize = sizeof(ddbltfx);
  ddbltfx.dwFillColor = fillcolor;
  RECT rc = { 0, 0, (int)desc.dwWidth, (int)desc.dwHeight };
  surf->Blt(&rc, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

  surf->GetDC(&hdc);
  {
    HGDIOBJ hgdiobj = SelectObject(hdc, font);
    SetBkColor(hdc, fillcolor);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    char text[FONT_COLS];
    for (int row = 0; row < chrows; ++row) {
      for (int ch = 0; ch < FONT_COLS; ++ch)
        text[ch] = ' ' + FONT_COLS * row + ch;
      TextOutA(hdc, 0, row * chh, text, FONT_COLS);
    }
    SelectObject(hdc, hgdiobj);
  }
  surf->ReleaseDC(hdc);

  surf->GetDC(&hdc);
  {
    HDC hmemdc = CreateCompatibleDC(hdc);
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    surf->GetSurfaceDesc(&desc);
    HBITMAP hbmp = CreateCompatibleBitmap(hdc, desc.dwWidth, desc.dwHeight);
    HBITMAP hprevbmp = (HBITMAP)SelectObject(hmemdc, hbmp);
    BitBlt(hmemdc, 0, 0, desc.dwWidth, desc.dwHeight, hdc, 0, 0, SRCCOPY);
    if (OpenClipboard(NULL)) { 
      EmptyClipboard();
      SetClipboardData(CF_BITMAP,hbmp);
      CloseClipboard();
      printf("font image is now in your clipboard\n");
    } else {
      fprintf(stderr, "error: could not open clipboard\n");
    }
    SelectObject(hmemdc, hprevbmp);
    DeleteDC(hmemdc);
  }
  surf->ReleaseDC(hdc);

  DeleteObject(font);

  return 0;
}
