#ifndef _WASM_GRAPHICS_
#define _WASM_GRAPHICS_

#include "../../../../GraphicsRenderer.h"
#include "../../../../pro/Graphics.h"
#include "../../../../../common/officedrawingfile.h"
#include "../../../../../../XpsFile/XpsFile.h"
#include "../../../../../../DjVuFile/DjVu.h"
#include "../../../../../../PdfReader/PdfReader.h"

class CGraphicsFileDrawing
{
private:
    IOfficeDrawingFile* pReader;
    NSFonts::IApplicationFonts* pApplicationFonts;
public:
    CGraphicsFileDrawing()
    {
        pReader = NULL;
        pApplicationFonts = NSFonts::NSApplication::Create();
    }
    ~CGraphicsFileDrawing()
    {
        RELEASEOBJECT(pReader);
        RELEASEOBJECT(pApplicationFonts);
    }
    bool  Open   (BYTE* data, DWORD length, int nType)
    {
        if (nType == 0)
            pReader = new PdfReader::CPdfReader(pApplicationFonts);
        else if (nType == 1)
            pReader = new CDjVuFile(pApplicationFonts);
        else if (nType == 2)
            pReader = new CXpsFile(pApplicationFonts);
        if (!pReader)
            return false;
        return pReader->LoadFromMemory(data, length);
    }
    int   GetPagesCount()
    {
        return pReader->GetPagesCount();
    }
    void  GetPageInfo  (int nPageIndex, int& nWidth, int& nHeight, int& nPageDpiX)
    {
        double dPageDpiX, dPageDpiY;
        double dWidth, dHeight;
        pReader->GetPageInfo(nPageIndex, &dWidth, &dHeight, &dPageDpiX, &dPageDpiY);
        nWidth = dWidth;
        nHeight = dHeight;
        nPageDpiX = dPageDpiX;
    }
    BYTE* GetPage      (int nPageIndex, int nRasterW, int nRasterH)
    {
        return pReader->ConvertToPixels(nPageIndex, nRasterW, nRasterH, true);
    }
    BYTE* GetGlyphs    (int nPageIndex, int nRasterW, int nRasterH)
    {
        return pReader->GetGlyphs(nPageIndex, nRasterW, nRasterH);
    }
    BYTE* GetLinks     (int nPageIndex, int nRasterW, int nRasterH)
    {
        return pReader->GetLinks(nPageIndex, nRasterW, nRasterH);
    }
    BYTE* GetStructure()
    {
        return pReader->GetStructure();
    }
};

#endif // _WASM_GRAPHICS_
