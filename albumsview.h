#ifndef ALBUMSVIEW_H
#define ALBUMSVIEW_H

#include <QWidget>
#include <QSqlQuery>
#include <QGridLayout>
#include <album.h>
#include <QFrame>
#include <collectionDB.h>
#include <playlist.h>
#include <QListWidget>
#include <babetable.h>
#include <QComboBox>
namespace Ui {
class AlbumsView;
}

class AlbumsView : public QWidget
{
    Q_OBJECT

public:
    explicit AlbumsView(QWidget *parent = 0);
    ~AlbumsView();
    void populateTableView(QSqlQuery query);
    void passConnection(CollectionDB *con);
    void flushGrid();
    QSlider *slider;
    QComboBox *order;
    QFrame *utilsFrame;
    BabeTable *albumTable;
    enum ALBUMSVIEW_H{ TITLE, ARTIST, ART };

private:

    QListWidget *grid;
    QList<Album*> albumsList;
    QFrame *albumBox_frame;
    QFrame *line_h;
    Album *cover;
    CollectionDB *connection;
    Playlist *playlist;

    QToolButton *closeBtn;


    int albumSize=120;

private slots:
    void getAlbumInfo(QStringList info);
    void albumTable_clicked(QStringList list);
    void albumTable_rated(QStringList list);
    void albumTable_babeIt(QStringList list);
    void albumHover();
    void albumsSize(int value);
    void orderChanged(QString order);
    void hideAlbumFrame();
signals:
    void songClicked(QStringList url);
    void songRated(QStringList url);
    void songBabeIt(QStringList url);
    void albumOrderChanged(QString order);

};

#endif // ALBUMSVIEW_H
