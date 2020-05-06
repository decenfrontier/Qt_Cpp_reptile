#ifndef DWindow_H
#define DWindow_H

#include <QWindow>  // 和windows平台相关的窗口
#include <QBackingStore>    // 用于绘制QWindow


class DWindow : public QWindow
{

public:
    explicit DWindow(QWindow *parent = 0);
    ~DWindow();

    bool event(QEvent* e);
    void setImage(QImage& image);
private:
    QBackingStore store;
    QImage img;
};

#endif // DWindow_H
