#include "DWindow.h"
#include <QPainter>
#include <QImage>
#include <windows.h>
#include <WinUser.h>
#include <QDesktopWidget>

static HWND g_workerw = 0;

static BOOL __stdcall EnumWndProc(HWND topHandle,LPARAM lp)
{
    HWND p = FindWindowEx(topHandle,0,L"SHELLDLL_DefView",NULL);
    if(p != 0)
    {
        g_workerw = FindWindowEx(0,topHandle,L"WorkerW",NULL);
    }
    return TRUE;
}

DWindow::DWindow(QWindow *parent):QWindow(parent),store(this)
{
    HWND hwndProgram = FindWindow(L"Progman",L"Program Manager");
    SendMessageTimeout(hwndProgram,0x052C,0,0,SMTO_NORMAL,1000,0); // 创建出WorkerW窗口
    EnumWindows(EnumWndProc,0);
    if(g_workerw == 0)
    {
        abort();
    }
    // 从窗口句柄获取窗口指针(WorkerW窗口)
    QWindow* windowDesktop = QWindow::fromWinId((WId)g_workerw);
    this->setParent(windowDesktop);

    QDesktopWidget WndDesk;
    QRect rectFullDesktop = WndDesk.availableGeometry();
    this->setGeometry(rectFullDesktop);
}

DWindow::~DWindow()
{

}

bool DWindow::event(QEvent *e)
{
    if(e->type() == QEvent::Expose || e->type() == QEvent::Resize)
    {
        QRect rect(QPoint(0,0),this->size());
        store.resize(this->size());
        store.beginPaint(rect);

        QPainter painter(store.paintDevice());
        //painter.fillRect(rect,Qt::white);
        //painter.drawText(10,10,"sdfasdgd");
        painter.drawImage(0,0,img);

        store.endPaint();
        store.flush(rect);
    }
    return QWindow::event(e);
}

void DWindow::setImage(QImage &image)
{
    this->img = image;
    // 把图片移到桌面正中央
    QDesktopWidget w;
    QRect rectFullDesktop = w.availableGeometry();
    int x = (rectFullDesktop.width() - image.width()) / 2;
    int y = (rectFullDesktop.height() - image.height()) / 2;
    setGeometry(x,y,image.width(),image.height());
}
