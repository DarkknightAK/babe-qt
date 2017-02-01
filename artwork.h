#ifndef ARTWORK_H
#define ARTWORK_H
#include <QPixmap>
#include <QDebug>
#include <QImage>
#include <QtCore>
#include <QtNetwork>
#include <curl/curl.h>
#include <QUrl>
#include <QHttpPart>
#include <QWidget>
#include <QObject>
#include <QNetworkAccessManager>

class ArtWork : public QObject
{
    Q_OBJECT

public:
    explicit ArtWork(QObject *parent = 0);
    void setData(QString artist, QString album);
    QByteArray getCover();
    QByteArray selectCover(QString url);



private:
    QString url ;
    //QImage cover;
    QPixmap art;
    QString album;
    QString artist;
    QString xmlData;
    QByteArray coverArray;

    //QNetworkReply reply;

private slots:

    void xmlInfo(QNetworkReply* reply);
    void dummy();
signals:
    void pixmapReady(QImage *pix);
    void coverReady(QByteArray array);
    void test();
    void  bitch();
};

#endif // ARTWORK_H