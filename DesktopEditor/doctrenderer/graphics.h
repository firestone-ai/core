#ifndef CGRAPHICS_H
#define CGRAPHICS_H

#include <string>

#include "../common/Types.h"
#include "../common/File.h"
#include "../graphics/pro/Graphics.h"

namespace NSGraphics
{
    enum EGrStateType
    {
        gstPen		= 0,
        gstBrush	= 1,
        gstPenBrush	= 2,
        gstState	= 3,
        gstNone		= 4
    };

    struct IGrState
    {
    public:
        EGrStateType m_eType;
        IGrState() { m_eType = gstNone; }
        virtual ~IGrState() {}
    };

    struct CGrStatePen : public IGrState
    {
        NSStructures::CPen m_oPen;
        CGrStatePen() { m_eType = gstPen; }
    };

    struct CGrStateBrush : public IGrState
    {
        NSStructures::CBrush m_oBrush;
        CGrStateBrush() { m_eType = gstBrush; }
    };

    struct CGrStatePenBrush : public IGrState
    {
        NSStructures::CPen   m_oPen;
        NSStructures::CBrush m_oBrush;

        CGrStatePenBrush() { m_eType = gstPenBrush; }
    };

    struct CHist_Clip
    {
        Aggplus::CDoubleRect Rect;
        bool				 IsIntegerGrid;
        Aggplus::CMatrix	 Transform;

        CHist_Clip() : IsIntegerGrid(false) {}
    };

    struct CGrStateState : public IGrState
    {
        Aggplus::CMatrix	     Transform;
        bool				     IsIntegerGrid;
        std::vector<CHist_Clip*> Clips;

        CGrStateState()
        {
            m_eType = gstState;
            IsIntegerGrid = false;
        }
        ~CGrStateState()
        {
            for(CHist_Clip* pClip : Clips)
                RELEASEOBJECT(pClip);
            Clips.clear();
        }
    };

    struct CGrState
    {
        std::vector<IGrState*>   States;
        std::vector<CHist_Clip*> Clips;

        CGrState() {}
        ~CGrState()
        {
            for(IGrState* pState : States)
                RELEASEOBJECT(pState);
            States.clear();

            for(CHist_Clip* pState : Clips)
                RELEASEOBJECT(pState);
            Clips.clear();
        }
    };

    class CGraphics
    {
    public:
        static std::wstring m_sApplicationFontsDirectory;
        static std::wstring m_sApplicationImagesDirectory;
        static std::wstring m_sApplicationThemesDirectory;

    private:
        NSFonts   ::IApplicationFonts* m_pApplicationFonts;
        NSGraphics::IGraphicsRenderer* m_pRenderer;
        CBgraFrame m_oFrame;
        CGrState   m_oGrState;

    public:
        CGraphics() {}
        ~CGraphics()
        {
            RELEASEINTERFACE(m_pRenderer);
            RELEASEINTERFACE(m_pApplicationFonts);
        }

        void init(double width_px, double height_px, double width_mm, double height_mm);
        void EndDraw() {}
        void put_GlobalAlpha(bool enable, double globalAlpha);
        void Start_GlobalAlpha() {}
        void End_GlobalAlpha();
        // pen methods
        void p_color(int r, int g, int b, int a);
        void p_width(double w);
        void p_dash(size_t length, double* dash);
        // brush methods
        void b_color1(int r, int g, int b, int a);
        void b_color2(int r, int g, int b, int a);
        void transform(double sx, double shy, double shx, double sy, double tx, double ty);
        void CalculateFullTransform();
        // path commands
        void _s();
        void _e();
        void _z();
        void _m(double x, double y);
        void _l(double x, double y);
        void _c(double x1, double y1, double x2, double y2, double x3, double y3);
        void _c2(double x1, double y1, double x2, double y2);
        void ds();
        void df();
        // canvas state
        void save() {}
        void restore();
        void clip();
        void reset();
        void transform3(double sx, double shy, double shx, double sy, double tx, double ty);
        void FreeFont() {}
        void ClearLastFont() {}
        // images
        void drawImage(const std::wstring& img, double x, double y, double w, double h, BYTE alpha);
        // text
        void GetFont() {}
        void font() {}
        void SetFont(const std::wstring& path, int face, double size, int style);
        void SetTextPr() {}
        void GetTextPr() {}
        void FillText(double x, double y, int text);
        void t() {}
        void t2() {}
        void tg(int text, double x, double y);
        void charspace() {}
        // private methods
        void private_FillGlyph()  {}
        void private_FillGlyphC() {}
        void private_FillGlyph2() {}
        void SetIntegerGrid(bool param);
        bool GetIntegerGrid();
        void DrawStringASCII() {}
        void DrawStringASCII2() {}
        void DrawHeaderEdit(double yPos);
        void DrawFooterEdit(double yPos);
        void DrawLockParagraph () {}
        void DrawLockObjectRect() {}
        void DrawEmptyTableLine(double x1, double y1, double x2, double y2);
        void DrawSpellingLine  (double y0, double x0, double x1, double w);
        // smart methods for horizontal / vertical lines
        void drawHorLine (BYTE align, double y, double x, double r, double penW);
        void drawHorLine2(BYTE align, double y, double x, double r, double penW);
        void drawVerLine (BYTE align, double x, double y, double b, double penW);
        // мега крутые функции для таблиц
        void drawHorLineExt(BYTE align, double y, double x, double r, double penW, double leftMW, double rightMW);
        void rect     (double x, double y, double w, double h);
        void TableRect(double x, double y, double w, double h);
        // функции клиппирования
        void AddClipRect(double x, double y, double w, double h);
        void RemoveClipRect() {}
        void SetClip    (double x, double y, double w, double h) {}
        void RemoveClip() {}
        void drawCollaborativeChanges(double x, double y, double w, double h, int r, int g, int b, int a);
        void drawMailMergeField      (double x, double y, double w, double h);
        void drawSearchResult        (double x, double y, double w, double h);
        void drawFlowAnchor          (double x, double y) {}
        void SavePen();
        void RestorePen();
        void SaveBrush();
        void RestoreBrush();
        void SavePenBrush();
        void RestorePenBrush();
        void SaveGrState();
        void RestoreGrState();
        void StartClipPath();
        void EndClipPath();
        void StartCheckTableDraw() {}
        void EndCheckTableDraw(bool bIsRestore) {}
        void SetTextClipRect(double _l, double _t, double _r, double _b) {}
        void AddSmartRect   (double x,  double y,  double w,  double h, double pen_w);
        void CheckUseFonts2() {}
        void UncheckUseFonts2() {}
        void Drawing_StartCheckBounds() {}
        void Drawing_EndCheckBounds() {}
        void DrawPresentationComment() {}
        void DrawPolygon() {}
        void DrawFootnoteRect() {}
        // new methods
        std::string toDataURL(std::wstring type);
    };
}

#endif // CGRAPHICS_H
