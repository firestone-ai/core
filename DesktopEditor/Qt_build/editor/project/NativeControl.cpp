#include "NativeControl.h"
#include "../../../DesktopEditor/editor/Control.h"

#include <QGLContext>
#include <QThread>

class CEditorCtrlWrapper : public CEditorCtrl
{
public:
    CNativeCtrl* m_pCtrl;
    CVideoFrame* m_pTargetVRAM;

public:
    CEditorCtrlWrapper() : CEditorCtrl()
    {
        m_pCtrl = NULL;
        m_pTargetVRAM = NULL;
    }
    virtual ~CEditorCtrlWrapper()
    {
        m_pCtrl = NULL;
        RELEASEOBJECT(m_pTargetVRAM);
    }

    virtual void InvalidateRectNative(int x, int y, int w, int h)
    {
        if (m_pCtrl != NULL)
            m_pCtrl->InvalidateRectNative(x, y, w, h);
    }
};

// Native control
void CNativeCtrl::slot_threadRepaint()
{
    repaint();
}

CNativeCtrl::CNativeCtrl(QWidget *parent, const char *name) : QGLWidget(parent)
{
    connect(this, SIGNAL(signal_threadRepaint()), SLOT(slot_threadRepaint()), Qt::QueuedConnection);
    setAttribute(Qt::WA_NoBackground);
    setAttribute(Qt::WA_NoSystemBackground);

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMouseTracking(true);
    grabKeyboard();

    m_pWrapper = new CEditorCtrlWrapper();
    m_pWrapper->m_pCtrl = this;

    m_pWrapper->m_strFontsDirectory = L"";
}
CNativeCtrl::~CNativeCtrl()
{
}

void CNativeCtrl::InitSDK(const std::wstring& sFontsPath, const std::wstring& sSdkPath)
{
    m_pWrapper->m_strFontsDirectory = sFontsPath;
    std::wstring sScriptPath = sSdkPath;
    m_pWrapper->m_oWorkJS.m_oThreadJS.StartFromScript(sScriptPath);
}

void CNativeCtrl::OpenFile(const std::wstring& sFilePath)
{
    m_pWrapper->InternalOpenFile(sFilePath);
    m_pWrapper->InternalCalculateFile();
}

void CNativeCtrl::SetZoom(double dZoom)
{
    m_pWrapper->SetZoom(dZoom);
}

void CNativeCtrl::ChangeCountPagesInBlock()
{
    int nCountOld = m_pWrapper->GetPagesInBlock();
    m_pWrapper->SetPagesInBlock((nCountOld == 1) ? 2 : 1);
}

void CNativeCtrl::initializeGL()
{
    m_pWrapper->m_oDevicePainter.m_oFrameControls.m_oFrame.Init(m_pWrapper->GetCountThreadsUse());
    m_pWrapper->Resize(100, 100);
    m_pWrapper->InternalInit();

    m_pWrapper->m_oWorkJS.m_oThreadJS.SetMainCtrl((CEditorCtrl*)m_pWrapper);
}

void CNativeCtrl::paintGL()
{
    if (!m_pWrapper || !m_pWrapper->m_pSkin)
        return;

    CTemporaryCS oCS(&m_pWrapper->m_oCS_Places);

    CVideoFrameControls* pVRAM_Worker = &m_pWrapper->m_oDevicePainter.m_oFrameControls;

	pVRAM_Worker->m_oFrame.SetCurrentCtx();

    DWORD dwTime1 = NSTimers::GetTickCount();

    glClearColor(m_pWrapper->m_pSkin->BackgroundColor.R / 255.0,
                 m_pWrapper->m_pSkin->BackgroundColor.G / 255.0,
                 m_pWrapper->m_pSkin->BackgroundColor.B / 255.0,
                 1);
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat _width = this->width() * this->devicePixelRatio();
    GLfloat _height = this->height() * this->devicePixelRatio();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, _width, 0.0, _height, -1.0, 1.0);

    glEnable(GL_BLEND);

    glColor3f(1,1,1);

    glViewport(0, 0, _width, _height);

    pVRAM_Worker->m_oFrame.UnSetCurrentCtx();

    pVRAM_Worker->m_oFrame.SetSizes((int)_width, (int)_height);

    // pages
    Aggplus::Rect oClipRect(m_pWrapper->m_oViewer.X,
                            m_pWrapper->m_oViewer.Y,
                            m_pWrapper->m_oViewer.Width,
                            m_pWrapper->m_oViewer.Height);

    if (m_pWrapper->m_oDrawingDocument.m_lStartBlock >= 0)
    {
        for (int i = (int)m_pWrapper->m_oDrawingDocument.m_lStartBlock;
             i <= m_pWrapper->m_oDrawingDocument.m_lEndBlock; ++i)
        {
            NSDrawingDocument::CBlockPages* pBlock = &m_pWrapper->m_oDrawingDocument.m_arBlocks[i];
            for (int j = 0; j < pBlock->m_arPages.GetCount(); ++j)
            {
                Aggplus::Rect oRect = pBlock->m_arPages[j]->m_oGlobalBounds;
                oRect.Offset(m_pWrapper->m_oViewer.X, m_pWrapper->m_oViewer.Y);

                {
                    m_pWrapper->m_oCacheDocument.DrawGL(&m_pWrapper->m_oDevicePainter,
                                          oClipRect,
                                          oRect,
                                          pBlock->m_arPages[j]->m_lPageIndex);
                }
            }
        }
    }

    // overlay
    m_pWrapper->m_oCS_Overlay.Enter();

    if (NULL != m_pWrapper->m_oOverlay.m_pBlitDIB)
    {
        //glEnable(GL_BLEND);

        pVRAM_Worker->m_oFrame.SetCurrentCtx();

        glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

        pVRAM_Worker->m_oFrame.UnSetCurrentCtx();

        Aggplus::Rect oRectOverlay = m_pWrapper->m_oOverlay.GetOverlayBlitRect();

        pVRAM_Worker->m_oFrame.DrawTexture2(m_pWrapper->m_oOverlay.m_pBlitDIB->m_pFrame,
                                           oRectOverlay.X,
                                           oRectOverlay.Y,
                                           oRectOverlay.Width,
                                           oRectOverlay.Height,
                                           true);

        /*
        pVRAM_Worker->m_oFrame.DrawTexture(m_pWrapper->m_oOverlay.m_pBlitDIB->m_pFrame,
                                           oRectOverlay.X,
                                           oRectOverlay.Y,
                                           true);
        */

        //glDisable(GL_BLEND);
    }
    m_pWrapper->m_oCS_Overlay.Leave();

    // target!!!
    if (m_pWrapper->m_oTarget.IsNeedDraw())
    {
        m_pWrapper->m_oTarget.Lock();

        int nT_W = 0;
        int nT_H = 0;
        m_pWrapper->m_oTarget.GetTargetPosition(nT_W, nT_H);

        int _target_x = m_pWrapper->m_oViewer.X + nT_W;
        int _target_y = m_pWrapper->m_oViewer.Y + nT_H;
        int _target_w = m_pWrapper->m_oTarget.m_oDIB.m_lWidth;
        int _target_h = m_pWrapper->m_oTarget.m_oDIB.m_lHeight;

        m_pWrapper->m_oDrawingChecker.SetOldTarget(_target_x, _target_y, _target_w, _target_h);

        bool bIsChanged = m_pWrapper->m_oTarget.IsChangedTarget();

        if (bIsChanged)
            RELEASEOBJECT(m_pWrapper->m_pTargetVRAM);

        if (NULL == m_pWrapper->m_pTargetVRAM)
        {
            m_pWrapper->m_oTarget.m_oDIB.CopyToVRAM(&pVRAM_Worker->m_oFrame);
            m_pWrapper->m_pTargetVRAM = m_pWrapper->m_oTarget.m_oDIB.m_pFrame;
            m_pWrapper->m_oTarget.m_oDIB.m_pFrame = NULL;
        }

        pVRAM_Worker->m_oFrame.DrawTexture(m_pWrapper->m_pTargetVRAM, _target_x, _target_y, true);

        m_pWrapper->m_oTarget.UnLock();
    }

    m_pWrapper->m_oDevicePainter.LockPaint();

    // buttons
    pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pButtonRulers,
                                       m_pWrapper->m_oButtonRulers.X,
                                       m_pWrapper->m_oButtonRulers.Y, true);

    pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pButtonNextPage,
                                       m_pWrapper->m_oNext.X,
                                       m_pWrapper->m_oNext.Y, true);

    pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pButtonPrevPage,
                                       m_pWrapper->m_oPrev.X,
                                       m_pWrapper->m_oPrev.Y, true);

    // scrolls
    pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pVerScroll,
                                       m_pWrapper->m_oVerScroll.X,
                                       m_pWrapper->m_oVerScroll.Y, true);

    if (m_pWrapper->m_bIsHorScrollVisible)
    {
        pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pHorScroll,
                                           m_pWrapper->m_oHorScroll.X,
                                           m_pWrapper->m_oHorScroll.Y, true);
    }

    pVRAM_Worker->m_oFrame.SetCurrentCtx();

    glDisable(GL_BLEND);

    glPopMatrix();

    glFlush();

    pVRAM_Worker->m_oFrame.UnSetCurrentCtx();

    m_pWrapper->m_oDevicePainter.LeavePaint();


    DWORD dwTime2 = NSTimers::GetTickCount();

    /*
    FILE* file = fopen("/home/oleg/activex/AVS/Sources/TeamlabOffice/trunk/ServerComponents/DesktopEditor/log.txt", "a+");
    fprintf(file, "time: %d\n", (int)(dwTime2 - dwTime1));
    fclose(file);
    */
}

void CNativeCtrl::resizeGL(int width, int height)
{
    int nDevicePixelRatio = this->devicePixelRatio();
    m_pWrapper->Resize(this->width() * nDevicePixelRatio, this->height() * nDevicePixelRatio);
}

void CNativeCtrl::closeEvent(QCloseEvent* e)
{
    m_pWrapper->Stop();
    m_pWrapper->m_oTarget.Stop();
}

void CNativeCtrl::mousePressEvent(QMouseEvent* e)
{
    m_pWrapper->m_oMouseController.X = e->pos().x() * this->devicePixelRatio();
    m_pWrapper->m_oMouseController.Y = e->pos().y() * this->devicePixelRatio();

    Qt::MouseButton button = e->button();
    if (button == Qt::LeftButton)
        m_pWrapper->m_oMouseController.Button = CMouseController::mbtLeft;
    else if (button == Qt::RightButton)
        m_pWrapper->m_oMouseController.Button = CMouseController::mbtRight;
    else
        m_pWrapper->m_oMouseController.Button = CMouseController::mbtCenter;

    Qt::KeyboardModifiers mods = e->modifiers();

    m_pWrapper->m_oKeyboardController.ShiftKey = (0 != (mods & Qt::ShiftModifier)) ? true : false;
    m_pWrapper->m_oKeyboardController.CtrlKey = (0 != (mods & Qt::ControlModifier)) ? true : false;
    m_pWrapper->m_oKeyboardController.AltKey = (0 != (mods & Qt::AltModifier)) ? true : false;

    m_pWrapper->m_oMouseController.CheckDownCallback(true);
    m_pWrapper->InternalOnMouseDown();
}
void CNativeCtrl::mouseMoveEvent(QMouseEvent* e)
{
    m_pWrapper->m_oMouseController.X = e->pos().x() * this->devicePixelRatio();
    m_pWrapper->m_oMouseController.Y = e->pos().y() * this->devicePixelRatio();

    Qt::KeyboardModifiers mods = e->modifiers();

    m_pWrapper->m_oKeyboardController.ShiftKey = (0 != (mods & Qt::ShiftModifier)) ? true : false;
    m_pWrapper->m_oKeyboardController.CtrlKey = (0 != (mods & Qt::ControlModifier)) ? true : false;
    m_pWrapper->m_oKeyboardController.AltKey = (0 != (mods & Qt::AltModifier)) ? true : false;

    m_pWrapper->m_oMouseController.CheckMoveCallback();
    m_pWrapper->InternalOnMouseMove();
}

void CNativeCtrl::mouseReleaseEvent(QMouseEvent* e)
{
    m_pWrapper->m_oMouseController.X = e->pos().x() * this->devicePixelRatio();
    m_pWrapper->m_oMouseController.Y = e->pos().y() * this->devicePixelRatio();

    Qt::MouseButton button = e->button();
    if (button == Qt::LeftButton)
        m_pWrapper->m_oMouseController.Button = CMouseController::mbtLeft;
    else if (button == Qt::RightButton)
        m_pWrapper->m_oMouseController.Button = CMouseController::mbtRight;
    else
        m_pWrapper->m_oMouseController.Button = CMouseController::mbtCenter;

    Qt::KeyboardModifiers mods = e->modifiers();

    m_pWrapper->m_oKeyboardController.ShiftKey = (0 != (mods & Qt::ShiftModifier)) ? true : false;
    m_pWrapper->m_oKeyboardController.CtrlKey = (0 != (mods & Qt::ControlModifier)) ? true : false;
    m_pWrapper->m_oKeyboardController.AltKey = (0 != (mods & Qt::AltModifier)) ? true : false;

    m_pWrapper->InternalOnMouseUp();
}
void CNativeCtrl::wheelEvent(QWheelEvent* event)
{
    int nDelta = (int)event->delta() >> 2;

    Qt::KeyboardModifiers mods = event->modifiers();

    m_pWrapper->m_oWhellController.WhellDeltaX = 0;
    m_pWrapper->m_oWhellController.WhellDeltaY = 0;

    if (Qt::Vertical == event->orientation() && (0 == (mods & Qt::ShiftModifier)))
        m_pWrapper->m_oWhellController.WhellDeltaY = -nDelta;
    else
        m_pWrapper->m_oWhellController.WhellDeltaX = -nDelta;

    m_pWrapper->InternalOnMouseWhell();
    event->ignore();
}

void CNativeCtrl::keyPressEvent(QKeyEvent* e)
{
    std::wstring s = e->text().toStdWString();

    int nKey = e->key();

    int nKeyCorrect = -1;
    switch (nKey)
    {
    case Qt::Key_Escape:
        nKeyCorrect = 27;
        break;
    case Qt::Key_Enter:
        nKeyCorrect = 13;
        break;
    case Qt::Key_Delete:
        nKeyCorrect = 46;
        break;
    case Qt::Key_Backspace:
        nKeyCorrect = 8;
        break;
    case Qt::Key_Insert:
        nKeyCorrect = 45;
        break;
    case Qt::Key_Home:
        nKeyCorrect = 36;
        break;
    case Qt::Key_End:
        nKeyCorrect = 35;
        break;
    case Qt::Key_Tab:
        nKeyCorrect = 9;
        break;
    case Qt::Key_PageUp:
        nKeyCorrect = 33;
        break;
    case Qt::Key_PageDown:
        nKeyCorrect = 34;
        break;
    case Qt::Key_Left:
        nKeyCorrect = 37;
        break;
    case Qt::Key_Up:
        nKeyCorrect = 38;
        break;
    case Qt::Key_Right:
        nKeyCorrect = 39;
        break;
    case Qt::Key_Down:
        nKeyCorrect = 40;
        break;
    default:
        break;
    }

    if (nKeyCorrect != -1)
    {
        m_pWrapper->m_oKeyboardController.KeyCode   = nKeyCorrect;
        m_pWrapper->m_oKeyboardController.CharCode  = nKeyCorrect;
    }
    else
    {
        m_pWrapper->m_oKeyboardController.KeyCode   = nKey;
        m_pWrapper->m_oKeyboardController.CharCode  = nKey;
    }

    Qt::KeyboardModifiers mods = e->modifiers();

    m_pWrapper->m_oKeyboardController.ShiftKey = (0 != (mods & Qt::ShiftModifier)) ? true : false;
    m_pWrapper->m_oKeyboardController.CtrlKey = (0 != (mods & Qt::ControlModifier)) ? true : false;
    m_pWrapper->m_oKeyboardController.AltKey = (0 != (mods & Qt::AltModifier)) ? true : false;

    if (s.length() == 0 || -1 != nKeyCorrect)
    {
        m_pWrapper->InternalOnKeyDown();
    }
    else
    {
        m_pWrapper->m_oKeyboardController.KeyCode  = (int)s.c_str()[0];
        m_pWrapper->m_oKeyboardController.CharCode  = (int)s.c_str()[0];
        m_pWrapper->InternalOnKeyDown();
        m_pWrapper->InternalOnKeyPress();
    }

    e->ignore();
}

void CNativeCtrl::keyReleaseEvent(QKeyEvent* e)
{
    Qt::KeyboardModifiers mods = e->modifiers();

    m_pWrapper->m_oKeyboardController.ShiftKey = (0 != (mods & Qt::ShiftModifier)) ? true : false;
    m_pWrapper->m_oKeyboardController.CtrlKey = (0 != (mods & Qt::ControlModifier)) ? true : false;
    m_pWrapper->m_oKeyboardController.AltKey = (0 != (mods & Qt::AltModifier)) ? true : false;

    m_pWrapper->InternalOnKeyUp();
}

void CNativeCtrl::InvalidateRectNative(int x, int y, int w, int h)
{
    emit signal_threadRepaint();
}

//////////////////
#ifdef _USE_WEB_MENU_

class CEditorCtrlWrapper2 : public CEditorCtrl
{
public:
    COpenGLSceneCtrl* m_pCtrl;

public:
    CEditorCtrlWrapper2() : CEditorCtrl()
    {
        m_pCtrl = NULL;
    }
    virtual ~CEditorCtrlWrapper2()
    {
        m_pCtrl = NULL;
    }

    virtual void InvalidateRectNative(int x, int y, int w, int h)
    {
        if (m_pCtrl != NULL)
            m_pCtrl->InvalidateRectNative(x, y, w, h);
    }
};

void COpenGLSceneCtrl::_native_OnResize(int X, int Y, int W, int H)
{
    m_nEditorX = X;
    m_nEditorY = Y;
    m_nEditorW = W;
    m_nEditorH = H;

    if (NULL == m_pWrapper)
    {
        m_pWrapper = new CEditorCtrlWrapper2();
        m_pWrapper->m_pCtrl = this;

        m_pWrapper->m_oDevicePainter.m_oFrameControls.m_oFrame.Init();
        m_pWrapper->Resize(m_nEditorW, m_nEditorH);

        m_pWrapper->InternalInit();

        m_pWrapper->m_oWorkJS.m_oThreadJS.SetMainCtrl((CEditorCtrl*)m_pWrapper);

        InitSDK(m_sFontsPath, m_sSdkPath);
        OpenFile(m_sFilePath);
    }
    else
    {
        m_pWrapper->Resize(m_nEditorW, m_nEditorH);
    }
}

void COpenGLSceneCtrl::_native_OnMouse(int type, int x, int y, int flags)
{
    if (m_pWrapper == NULL)
        return;

    int m_fDeviceScale = 1;
    m_pWrapper->m_oMouseController.X = (int)((x - m_nEditorX) * m_fDeviceScale);
    m_pWrapper->m_oMouseController.Y = (int)((y - m_nEditorY) * m_fDeviceScale);

    m_pWrapper->m_oKeyboardController.CtrlKey = ((flags & 0x01) == 0x01) ? true : false;
    m_pWrapper->m_oKeyboardController.AltKey = ((flags & 0x02) == 0x02) ? true : false;
    m_pWrapper->m_oKeyboardController.ShiftKey = ((flags & 0x04) == 0x04) ? true : false;

    switch (type)
    {
        case 0:
        {
            m_pWrapper->m_oMouseController.Button = CMouseController::mbtLeft;
            m_pWrapper->m_oMouseController.CheckDownCallback(true);
            m_pWrapper->InternalOnMouseDown();
            break;
        }
        case 1:
        {
            m_pWrapper->InternalOnMouseMove();
            break;
        }
        case 2:
        {
            m_pWrapper->InternalOnMouseUp();
            break;
        }
    }
}

void COpenGLSceneCtrl::_native_OnKeyboard(int type, int keycode, int charcode, int flags)
{
    if (m_pWrapper == NULL)
        return;

    m_pWrapper->m_oKeyboardController.KeyCode = keycode;
    m_pWrapper->m_oKeyboardController.CharCode = charcode;

    m_pWrapper->m_oKeyboardController.CtrlKey = ((flags & 0x01) == 0x01) ? true : false;
    m_pWrapper->m_oKeyboardController.AltKey = ((flags & 0x02) == 0x02) ? true : false;
    m_pWrapper->m_oKeyboardController.ShiftKey = ((flags & 0x04) == 0x04) ? true : false;

    switch (type)
    {
        case 0:
            m_pWrapper->InternalOnKeyDown();
            break;
        case 1:
            m_pWrapper->InternalOnKeyPress();
            break;
        case 2:
            m_pWrapper->InternalOnKeyUp();
            break;
    }
}

void COpenGLSceneCtrl::slot_threadRepaint()
{
    this->update(0, 0, this->width(), this->height());
}

void COpenGLSceneCtrl::InvalidateRectNative(int x, int y, int w, int h)
{
    emit signal_threadRepaint();
}

void COpenGLSceneCtrl::drawBackground(QPainter *painter, const QRectF &rect)
{
    if (NULL == m_pWrapper)
    {
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();
        return;
    }

    /*
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    return;
    */

    CEditorCtrl* m_pCPlusPlusWrapper = (CEditorCtrl*)m_pWrapper;

    CTemporaryCS* pCS = NULL;
    if (!m_pCPlusPlusWrapper || !m_pCPlusPlusWrapper->m_pSkin)
        return;

    if (m_pCPlusPlusWrapper->m_bIsNeedCheckCSOnNativeBlitting)
        pCS = new CTemporaryCS(&m_pCPlusPlusWrapper->m_oCS);

    CVideoFrameControls* pVRAM_Worker = &m_pCPlusPlusWrapper->m_oDevicePainter.m_oFrameControls;

    NSCriticalSection::CRITICAL_SECTION* pCS_GL = pVRAM_Worker->m_oFrame.GetLocker();

    //pCS_GL->Enter();
    //pVRAM_Worker->m_oFrame.SetCurrentCtx();

    DWORD dwTime1 = NSTimers::GetTickCount();

    glClearColor(m_pCPlusPlusWrapper->m_pSkin->BackgroundColor.R / 255.0,
                 m_pCPlusPlusWrapper->m_pSkin->BackgroundColor.G / 255.0,
                 m_pCPlusPlusWrapper->m_pSkin->BackgroundColor.B / 255.0,
                 1);
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat _width = this->width() * m_pView->devicePixelRatio();
    GLfloat _height = this->height() * m_pView->devicePixelRatio();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, _width, 0.0, _height, -1.0, 1.0);

    glEnable(GL_BLEND);

    glColor3f(1,1,1);

    glViewport(0, 0, _width, _height);

    pVRAM_Worker->m_oFrame.SetSizes((int)_width, (int)_height);

    // pages
    Aggplus::Rect oClipRect(m_pCPlusPlusWrapper->m_oViewer.X,
                            m_pCPlusPlusWrapper->m_oViewer.Y,
                            m_pCPlusPlusWrapper->m_oViewer.Width,
                            m_pCPlusPlusWrapper->m_oViewer.Height);

    oClipRect.Offset(m_nEditorX, m_nEditorY);

    if (m_pCPlusPlusWrapper->m_oDrawingDocument.m_lStartBlock >= 0)
    {
        for (int i = (int)m_pCPlusPlusWrapper->m_oDrawingDocument.m_lStartBlock;
             i <= m_pCPlusPlusWrapper->m_oDrawingDocument.m_lEndBlock; ++i)
        {
            NSDrawingDocument::CBlockPages* pBlock = &m_pCPlusPlusWrapper->m_oDrawingDocument.m_arBlocks[i];
            for (int j = 0; j < pBlock->m_arPages.GetCount(); ++j)
            {
                Aggplus::Rect oRect = pBlock->m_arPages[j]->m_oGlobalBounds;
                oRect.Offset(m_pCPlusPlusWrapper->m_oViewer.X, m_pCPlusPlusWrapper->m_oViewer.Y);
                oRect.Offset(m_nEditorX, m_nEditorY);

                {
                    //pVRAM_Worker->m_oFrame.UnSetCurrentCtx();
                    pCS_GL->Leave();
                    m_pCPlusPlusWrapper->m_oCacheDocument.DrawGL(&m_pCPlusPlusWrapper->m_oDevicePainter,
                                          oClipRect,
                                          oRect,
                                          pBlock->m_arPages[j]->m_lPageIndex);
                    pCS_GL->Enter();
                    //pVRAM_Worker->m_oFrame.SetCurrentCtx();
                }
            }
        }
    }

    // overlay
    if (m_pCPlusPlusWrapper->m_oOverlay.m_oBounds.max_x >= m_pCPlusPlusWrapper->m_oOverlay.m_oBounds.min_x &&
        m_pCPlusPlusWrapper->m_oOverlay.m_oBounds.max_y >= m_pCPlusPlusWrapper->m_oOverlay.m_oBounds.min_y &&
        NULL != pVRAM_Worker->m_pOverlay)
    {
        //glEnable(GL_BLEND);
        glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

        pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pOverlay,
                                           m_pCPlusPlusWrapper->m_oViewer.X + m_nEditorX,
                                           m_pCPlusPlusWrapper->m_oViewer.Y + m_nEditorY,
                                           true);

        //glDisable(GL_BLEND);
    }

    // buttons
    pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pButtonRulers,
                                       m_pCPlusPlusWrapper->m_oButtonRulers.X + m_nEditorX,
                                       m_pCPlusPlusWrapper->m_oButtonRulers.Y + m_nEditorY, true);

    pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pButtonNextPage,
                                       m_pCPlusPlusWrapper->m_oNext.X + m_nEditorX,
                                       m_pCPlusPlusWrapper->m_oNext.Y + m_nEditorY, true);

    pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pButtonPrevPage,
                                       m_pCPlusPlusWrapper->m_oPrev.X + m_nEditorX,
                                       m_pCPlusPlusWrapper->m_oPrev.Y + m_nEditorY, true);

    // scrolls
    pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pVerScroll,
                                       m_pCPlusPlusWrapper->m_oVerScroll.X + m_nEditorX,
                                       m_pCPlusPlusWrapper->m_oVerScroll.Y + m_nEditorY, true);

    if (m_pCPlusPlusWrapper->m_bIsHorScrollVisible)
    {
        pVRAM_Worker->m_oFrame.DrawTexture(pVRAM_Worker->m_pHorScroll,
                                           m_pCPlusPlusWrapper->m_oHorScroll.X + m_nEditorX,
                                           m_pCPlusPlusWrapper->m_oHorScroll.Y + m_nEditorY, true);
    }

    glDisable(GL_BLEND);

    glPopMatrix();

    glFlush();

    //pVRAM_Worker->m_oFrame.UnSetCurrentCtx();

    //pCS_GL->Leave();

    RELEASEOBJECT(pCS);

    DWORD dwTime2 = NSTimers::GetTickCount();
}

void COpenGLSceneCtrl::InitSDK(const std::wstring& sFontsPath, const std::wstring& sSdkPath)
{
    m_pWrapper->m_strFontsDirectory = sFontsPath;
    std::wstring sScriptPath = sSdkPath;
    m_pWrapper->m_oWorkJS.m_oThreadJS.StartFromScript(sScriptPath);
}

void COpenGLSceneCtrl::OpenFile(const std::wstring& sFilePath)
{
    m_pWrapper->InternalOpenFile(sFilePath);
    m_pWrapper->InternalCalculateFile();
}

void COpenGLSceneCtrl::LockGL()
{
    if (m_pWrapper)
        m_pWrapper->m_oDevicePainter.m_oFrameControls.m_oFrame.GetLocker()->Enter();
}

void COpenGLSceneCtrl::UnlockGL()
{
    if (m_pWrapper)
        m_pWrapper->m_oDevicePainter.m_oFrameControls.m_oFrame.GetLocker()->Leave();
}

#endif
