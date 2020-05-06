#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWebSettings>
#include <QDebug>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElementCollection>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QFileInfo>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // webView的设置
    QWebSettings* settings = ui->webView->settings();
    settings->setAttribute(QWebSettings::AutoLoadImages,false);
    settings->setAttribute(QWebSettings::JavascriptEnabled,false);

    connect(ui->webView,SIGNAL(loadProgress(int)),
            this,SLOT(onProgress(int)));
    connect(ui->webView,SIGNAL(loadFinished(bool)),
            this,SLOT(onLoadFinished(bool)));

    exitFlag = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    QMainWindow::closeEvent(e);
    exitFlag = true;
}

void MainWindow::on_btnLoad_clicked()
{
    QUrl url = ui->lineEdit->text();
    ui->webView->load(url);
}

void MainWindow::onProgress(int progress)
{
    QString title = QString::number(progress) + "%";
    setWindowTitle(title);
}

void MainWindow::onLoadFinished(bool ok)
{
    qDebug() << "加载完成";
    setWindowTitle(ui->webView->url().toString());

    QWebElementCollection elements = ui->webView->page()->mainFrame()->findAllElements("body > div.main > div > div.pic_main > div > div.col-main > div.main-wrap > div.pic-meinv > a > img");
    for(int i=0; i< elements.count(); i++)
    {
         if(exitFlag)    return;

         const QWebElement &ele = elements.at(i);
         QString url = ele.attribute("src");
         QImage img = downloadImage(url);

         QFileInfo info(url);
         QString fileName = info.fileName();
         img.save(QString("F:/pic/") +fileName);    // 保存图片到文件,默认为当前目录
         qDebug() << "保存图片成功:" << QString("F:/pic/") +fileName;

         dskWnd.setImage(img);
         dskWnd.show();

         // 延时2秒钟,避免访问过快导致网站封本地IP; 而且这样写不会像sleep那样阻塞界面
         QEventLoop loop;
         QTimer::singleShot(2*1000,&loop,SLOT(quit()));
         loop.exec();
    }

    // 转到下一页面
    elements = ui->webView->page()->mainFrame()->findAllElements("body > div.main > div > div.pic_main > div > div.col-main > div.main-wrap > div.pic-meinv > div.pic-o > div > a");
    if(elements.count() <= 0)
    {
        qDebug() << "no next page";
        return;
    }
    QString nextPageUrl = elements.at(0).attribute("href");
    ui->webView->load(QUrl(nextPageUrl));
}

QImage MainWindow::downloadImage(QString &url)
{
    QNetworkAccessManager mgr;
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkReply* reply = mgr.get(request);    // 异步下载,无论图片是否下载完,都直接返回
    QEventLoop loop;
    connect(reply,SIGNAL(finished()),
            &loop, SLOT(quit()));
    loop.exec();   // 卡住,等待quit

    QByteArray data = reply->readAll();
    QImage image = QImage::fromData(data);
    return image;
}
