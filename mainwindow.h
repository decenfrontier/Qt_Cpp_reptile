#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include "DWindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent* e);

private slots:
    void on_btnLoad_clicked();
    void onProgress(int progress);
    void onLoadFinished(bool ok);

private:
    Ui::MainWindow *ui;
    DWindow dskWnd;
    bool exitFlag;  // 程序退出标志位

private:
    QImage downloadImage(QString& url); // 下载一张图片
};

#endif // MAINWINDOW_H
