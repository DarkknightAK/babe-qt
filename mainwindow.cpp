#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QStatusBar>
#include <QStringList>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDir>
#include <QDirIterator>
#include <QStringList>
#include "collectionDB.h"
#include<QSqlQuery>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QMimeData>
#include <QMenu>
#include <QWidgetAction>
#include <QButtonGroup>
#include<QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //setWindowFlags(Qt::WindowStaysOnTopHint);
    ui->setupUi(this);

    this->setAcceptDrops(true);
    playback = new QToolBar();
    //playback->setMovable(false);
    this->setWindowTitle(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    //this->adjustSize();
   // this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    //this->setMinimumSize (200, 250);
    this->mini_mode=0;
    //checkCollection();
    settings_widget = new settings();
    babes_widget= new babes();
    //settings_widget->setContentsMargins(0,0,0,0);
    //settings_widget->setStyleSheet("background:red;");
    connect(settings_widget, SIGNAL(toolbarIconSizeChanged(int)),
                         this, SLOT(setToolbarIconSize(int)));
    connect(settings_widget, SIGNAL(collectionDBFinishedAdding(bool)),
                                    this, SLOT(collectionDBFinishedAdding(bool)));

    //collection_db.openCollection("../player/collection.db");
    if(settings_widget->checkCollection())
    {
        populateTableView();
        populateMainList();
    }
    settings_widget->readSettings();
    setToolbarIconSize(settings_widget->getToolbarIconSize());


    setUpViews();


    connect(updater, SIGNAL(timeout()), this, SLOT(update()));
    player->setVolume(100);
    ui->listWidget->setCurrentRow(0);
    QAction *babe, *remove;

    babe = new QAction("Babe it");
    remove = new QAction("Remove from list");

    ui->listWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->listWidget->addAction(babe);
    ui->listWidget->addAction(remove);


    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setColumnHidden(LOCATION, true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->sortByColumn(1,Qt::AscendingOrder);

    auto contextMenu = new QMenu(ui->tableWidget);
    ui->tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);
    auto uninstallAction = new QAction("testing it",contextMenu);
    ui->tableWidget->addAction(uninstallAction);
    connect(ui->tableWidget,SIGNAL(pressed(QModelIndex)),this,SLOT(setUpContextMenu()));
    connect(uninstallAction, SIGNAL(triggered()), this, SLOT(uninstallAppletClickedSlot()));

    QButtonGroup *bg = new QButtonGroup(contextMenu);
               bg->addButton(ui->fav1,1);
               bg->addButton(ui->fav2,2);
               bg->addButton(ui->fav3,3);
               bg->addButton(ui->fav4,4);
               bg->addButton(ui->fav5,5);
//connect(ui->fav1,SIGNAL(enterEvent(QEvent)),this,hoverEvent());
               connect(bg, SIGNAL(buttonClicked(int)),this, SLOT(rateGroup(int)));
auto gr = new QWidget();
auto ty = new QHBoxLayout();
ty->addWidget(ui->fav1);
ty->addWidget(ui->fav2);
ty->addWidget(ui->fav3);
ty->addWidget(ui->fav4);
ty->addWidget(ui->fav5);

gr->setLayout(ty);


               QWidgetAction *chkBoxAction= new QWidgetAction(contextMenu);
               chkBoxAction->setDefaultWidget(gr);

    ui->tableWidget->addAction(chkBoxAction);



    if(ui->listWidget->count() != 0)
    {
        loadTrack();
        player->pause();
        updater->start();

    }





    /*sidebar toolbar*/

    auto *left_spacer = new QWidget();
     left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
     right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

     ui->tracks_view->setToolTip("Search...");
     ui->mainToolBar->addWidget(ui->searchField);

    ui->mainToolBar->addWidget(left_spacer);

    ui->tracks_view->setToolTip("Songs");
    ui->mainToolBar->addWidget(ui->tracks_view);

    ui->albums_view->setToolTip("Albums");
    ui->mainToolBar->addWidget(ui->albums_view);

    ui->babes_view->setToolTip("Babes");
    ui->mainToolBar->addWidget(ui->babes_view);

    ui->playlists_view->setToolTip("Playlists");
    ui->mainToolBar->addWidget(ui->playlists_view);

    ui->queue_view->setToolTip("Queue");
    ui->mainToolBar->addWidget(ui->queue_view);

    ui->info_view->setToolTip("Info");
    ui->mainToolBar->addWidget(ui->info_view);

    ui->settings_view->setToolTip("Setings");
    ui->mainToolBar->addWidget(ui->settings_view);

    ui->mainToolBar->addWidget(right_spacer);

    ui->open_btn->setToolTip("Open...");
    ui->mainToolBar->addWidget(ui->open_btn);

    this->addToolBar(Qt::BottomToolBarArea, ui->mainToolBar);
   // this->setCentralWidget(ui->listView);

    /* playback toolbar*/


    playback->addWidget(ui->hide_sidebar_btn);
    playback->addWidget(ui->backward_btn);
    playback->addWidget(ui->fav_btn);
    playback->addWidget(ui->play_btn);
    playback->addWidget(ui->foward_btn);
    playback->addWidget(ui->shuffle_btn);


    //playback->addWidget();
    //playback->addWidget(ui->horizontalSlider);

    //playback->setIconSize(QSize(16, 16));
    //this->addToolBar(Qt::BottomToolBarArea, playback);

    /*status bar*/
    ui->hide_sidebar_btn->setToolTip("Go Mini");
    ui->shuffle_btn->setToolTip("Shuffle");

    /*
    status = new QToolBar();
    info=new QLabel(" Babe ... \xe2\x99\xa1  \xe2\x99\xa1 \xe2\x99\xa1 ");
    //info->setStyleSheet("color:white;");

    status->addWidget(ui->shuffle_btn);
    auto *sp = new QWidget();
    sp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    status->addWidget(sp);
    status->addWidget(info);
    auto *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    status->addWidget(spacer);
    status->addWidget(ui->hide_sidebar_btn);    
    status->setIconSize(QSize(16, 16));
    status->setMovable(false);

    this->addToolBar(Qt::BottomToolBarArea,status);
    */

    /* setup widgets*/

    utilsBar = new QToolBar();
    utilsBar->addWidget(ui->search);
    utilsBar->setMovable(false);
    ui->search->setPlaceholderText("Search...");

    //this->addToolBar(Qt::BottomToolBarArea,utilsBar);


    views = new QStackedWidget;
    views->addWidget(ui->tableWidget);
    auto* testing = new QLabel("hahaha tetsing this if it might work");
    views->addWidget(testing);
    views->addWidget(babes_widget);
    views->addWidget(settings_widget);

    connect(ui->tracks_view, SIGNAL(clicked()), this, SLOT(tracksView()));
    connect(ui->albums_view, SIGNAL(clicked()), this, SLOT(albumsView()));
    connect(ui->babes_view, SIGNAL(clicked()), this, SLOT(babesView()));
    connect(ui->playlists_view, SIGNAL(clicked()), this, SLOT(playlistsView()));
    connect(ui->queue_view, SIGNAL(clicked()), this, SLOT(queueView()));
    connect(ui->info_view, SIGNAL(clicked()), this, SLOT(infoView()));
    connect(ui->settings_view, SIGNAL(clicked()), this, SLOT(settingsView()));

    main_widget= new QWidget();
    layout = new QGridLayout();
    main_widget->setLayout(layout);
    this->setCentralWidget(main_widget);
    layout->setContentsMargins(6,0,6,0);


    /*album view*/
    auto *album_widget= new QWidget();
    auto *album_view = new QGridLayout();
    auto *album_art = new QLabel();
    album_art->setPixmap(QPixmap("../player/cover.jpg").scaled(200,200,Qt::KeepAspectRatio));

    //album_widget->children();

    //album_art->addAction(chkBoxAction);

    controls = new QWidget(album_art);
    auto controls_layout = new QGridLayout();
controls->setLayout(controls_layout);
controls->setGeometry(0,150,200,50);
controls->setStyleSheet(" background-color: rgba(255, 255, 255, 200);");
playback->setStyleSheet(" background:transparent;");
//ui->seekBar->setStyleSheet("background:transparent; ");
    album_view->addWidget(album_art, 0,0,Qt::AlignTop);

    controls_layout->addWidget(playback,0,0,Qt::AlignHCenter);
    controls_layout->addWidget(ui->seekBar,1,0,Qt::AlignTop);
    album_view->addWidget(ui->listWidget,1,0);

    album_view->setContentsMargins(0,0,0,0);


    album_widget->setStyleSheet("QWidget { padding:0; margin:0; }");
    //album_art->setStyleSheet("background-color:red; padding:0; margin:0;");
    album_art->setStyleSheet("padding:0; margin:0");
    album_widget->setLayout(album_view);
    //auto *btn = new QPushButton("babe me");


    album_widget->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding  );

    layout->addWidget(views, 0,0 );
    layout->addWidget(utilsBar, 1,0 );
    layout->addWidget(album_widget,0,1,0,1, Qt::AlignRight);
    //this->setStyle();



}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::enterEvent(QEvent *event)
{
    qDebug()<<"entered the window";
    controls->show();

}

 void MainWindow::setUpContextMenu()

{

     int row= ui->tableWidget->currentIndex().row(), rate;

     QString url=ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,LOCATION)).toString();
     qDebug()<<"se llamó a menu contextual con url: "<<url;
     QSqlQuery query= settings_widget->getCollectionDB().getQuery("SELECT * FROM tracks WHERE location = '"+url+"'");

        while (query.next())
        {
            rate = query.value(4).toInt();
        }

        setRating(rate);


}
 void MainWindow::leaveEvent(QEvent *event)
{
    qDebug()<<"left the window";
    controls->hide();
    //timer = new QTimer(this);
      /*connect(timer, SIGNAL(timeout()), this, SLOT(hideControls()));

      connect(timer,SIGNAL(timeout()), this, [&timer, this]() {
          qDebug()<<"ime is up";
          timer->stop();
      });*/

        //timer->start(3000);


}

 void MainWindow::hideControls()
 {
     /*qDebug()<<"ime is up";
     controls->hide();
     timer->stop();*/
 }
  void	MainWindow::dragEnterEvent(QDragEnterEvent *event)
 {
     event->accept();
 }

  void	MainWindow::dragLeaveEvent(QDragLeaveEvent *event){
     event->accept();
 }

  void	MainWindow::dragMoveEvent(QDragMoveEvent *event)
 {
     event->accept();
 }

  void	MainWindow::dropEvent(QDropEvent *event)
 {
     QList<QUrl> urls;
     urls = event->mimeData()->urls();
    QStringList list;
     for( auto url  : urls)
     {
         //qDebug()<<url.path();

         if(QFileInfo(url.path()).isDir())
         {
             //QDir dir = new QDir(url.path());
                 QDirIterator it(url.path(), QStringList() << "*.mp4" << "*.mp3" << "*.wav" <<"*.flac" <<"*.ogg", QDir::Files, QDirIterator::Subdirectories);
                 while (it.hasNext())
                 {
                     list<<it.next();

                     //qDebug() << it.next();
                 }

         }else if(QFileInfo(url.path()).isFile())
         {
         list<<url.path();
         }


     }

     playlist.add(list);
     updateList();
     //populateTableView();
     //ui->save->setChecked(false);
     if(shuffle) shufflePlaylist();
 }



void MainWindow::rateGroup(int id)
{
    qDebug()<<"rated with: "<<id;
    int row= ui->tableWidget->currentIndex().row();

    QString url=ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,LOCATION)).toString();


    if(settings_widget->getCollectionDB().check_existance("tracks","location",url))
    {
        if(settings_widget->getCollectionDB().insertInto("tracks","stars",url,id))
        {
            setRating(id);
        }
        qDebug()<<"rating the song";
    }else
    {

    }

}

void MainWindow::setRating(int rate)
{
    switch (rate)
    {
    case 0: ui->fav1->setIcon(QIcon::fromTheme("rating-unrated"));
            ui->fav2->setIcon(QIcon::fromTheme("rating-unrated"));
            ui->fav3->setIcon(QIcon::fromTheme("rating-unrated"));
            ui->fav4->setIcon(QIcon::fromTheme("rating-unrated"));
            ui->fav5->setIcon(QIcon::fromTheme("rating-unrated"));
            break;
        case 1: ui->fav1->setIcon(QIcon::fromTheme("rating"));
                ui->fav2->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav3->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav4->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav5->setIcon(QIcon::fromTheme("rating-unrated"));
                break;
        case 2: ui->fav1->setIcon(QIcon::fromTheme("rating"));
                ui->fav2->setIcon(QIcon::fromTheme("rating"));
                ui->fav3->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav4->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav5->setIcon(QIcon::fromTheme("rating-unrated"));
                break;
        case 3: ui->fav1->setIcon(QIcon::fromTheme("rating"));
                ui->fav2->setIcon(QIcon::fromTheme("rating"));
                ui->fav3->setIcon(QIcon::fromTheme("rating"));
                ui->fav4->setIcon(QIcon::fromTheme("rating-unrated"));
                ui->fav5->setIcon(QIcon::fromTheme("rating-unrated"));

                break;
        case 4: ui->fav1->setIcon(QIcon::fromTheme("rating"));
                ui->fav2->setIcon(QIcon::fromTheme("rating"));
                ui->fav3->setIcon(QIcon::fromTheme("rating"));
                ui->fav4->setIcon(QIcon::fromTheme("rating"));
                 ui->fav5->setIcon(QIcon::fromTheme("rating-unrated"));
                break;
        case 5: ui->fav1->setIcon(QIcon::fromTheme("rating"));
                ui->fav2->setIcon(QIcon::fromTheme("rating"));
                ui->fav3->setIcon(QIcon::fromTheme("rating"));
                ui->fav4->setIcon(QIcon::fromTheme("rating"));
                ui->fav5->setIcon(QIcon::fromTheme("rating"));
                break;

    }
}

void MainWindow::uninstallAppletClickedSlot()
{
    qDebug()<<"right clicked!";


    int row= ui->tableWidget->currentIndex().row();
    qDebug()<<ui->tableWidget->model()->data(ui->tableWidget->model()->index(row,LOCATION)).toString();
}


void MainWindow::setToolbarIconSize(int iconSize)
{
    qDebug()<< "Toolbar icons size changed";
    ui->mainToolBar->setIconSize(QSize(iconSize,iconSize));
    playback->setIconSize(QSize(iconSize,iconSize));
    ui->mainToolBar->update();
    playback->update();
   // this->update();
}

void MainWindow::setUpViews()
{

}


void MainWindow::tracksView()
{
    qDebug()<< "All songs view";
    views->setCurrentIndex(0);
    if(mini_mode!=0) expand();

}

void MainWindow::albumsView()
{
    views->setCurrentIndex(1);
    if(hideSearch)utilsBar->show();
    if(mini_mode!=0) expand();
}
void MainWindow::playlistsView()
{
    views->setCurrentIndex(3);
    if(mini_mode!=0) expand();
}
void MainWindow::queueView()
{
    views->setCurrentIndex(1);
    if(mini_mode!=0) expand();
}
void MainWindow::infoView()
{

    views->setCurrentIndex(0);

    if(mini_mode!=0) expand();
    if(!hideSearch)utilsBar->hide();
}
void MainWindow::babesView()
{
    views->setCurrentIndex(2);
    if(mini_mode!=0) expand();

   /* QString url= QFileDialog::getExistingDirectory();

qDebug()<<url;

    QStringList urlCollection;
//QDir dir = new QDir(url);
    QDirIterator it(url, QStringList() << "*.mp4" << "*.mp3" << "*.wav" <<"*.flac" <<"*.ogg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        urlCollection<<it.next();

        //qDebug() << it.next();
    }

   // collection.add(urlCollection);
    //updateList();
    populateTableView();*/
}
void MainWindow::settingsView()
{
    views->setCurrentIndex(3);
    if(mini_mode!=0) expand();
    if(!hideSearch) utilsBar->hide();
}

void MainWindow::expand()
{
    views->show();
    utilsBar->show();
    hideSearch=false;
    this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    this->resize(600,400);
    ui->hide_sidebar_btn->setToolTip("Go Mini");
    mini_mode=0;
    ui->mainToolBar->actions().at(0)->setVisible(true);
//keepOnTop(false);
}

void MainWindow::go_mini()
{
    //this->setMaximumSize (0, 0);

    views->hide();
    utilsBar->hide();
    hideSearch=true;
    ui->mainToolBar->actions().at(0)->setVisible(false);
   // ui->searchField->setVisible(false);
    this->resize(minimumSizeHint());
    main_widget->resize(minimumSizeHint());
    this->setFixedSize(minimumSizeHint());
    ui->hide_sidebar_btn->setToolTip("Go Extra-Mini");
    mini_mode=1;
//keepOnTop(true);

}

void MainWindow::keepOnTop(bool state)
{

    if (state)
        {//Qt::WindowFlags flags = windowFlags();
setWindowFlags(Qt::WindowStaysOnTopHint);
show();
    }else
    {
        //setWindowFlags(~ Qt::WindowStaysOnTopHint);
       //show();
    }
}

void MainWindow::setStyle()
{

   /* ui->mainToolBar->setStyleSheet(" QToolBar { border-right: 1px solid #575757; } QToolButton:hover { background-color: #d8dfe0; border-right: 1px solid #575757;}");
    playback->setStyleSheet("QToolBar { border:none;} QToolBar QToolButton { border:none;} QToolBar QSlider { border:none;}");
    this->setStyleSheet("QToolButton { border: none; padding: 5px; }  QMainWindow { border-top: 1px solid #575757; }");*/
    //status->setStyleSheet("QToolButton { color:#fff; } QToolBar {background-color:#575757; color:#fff; border:1px solid #575757;} QToolBar QLabel { color:#fff;}" );

}



void MainWindow::on_hide_sidebar_btn_clicked()
{
    if(mini_mode==0)
    {
        go_mini();

    }else if(mini_mode==1)
    {

        ui->listWidget->hide();
        ui->mainToolBar->hide();
       // ui->mainToolBar->hide();
       // ui->tableWidget->hide();
        //this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
        main_widget->resize(minimumSizeHint());
        this->resize(minimumSizeHint());

        this->setFixedSize(210,210);
        ui->hide_sidebar_btn->setToolTip("Expand");
        mini_mode=2;

    }else if(mini_mode==2)
    {
         ui->mainToolBar->show();
        ui->listWidget->show();
        this->resize(minimumSizeHint());
        main_widget->resize(minimumSizeHint());
        this->setFixedSize(minimumSizeHint());
        ui->hide_sidebar_btn->setToolTip("Full View");
        mini_mode=3;
    }else if (mini_mode==3)
    {
        expand();
    }


}

void MainWindow::on_shuffle_btn_clicked()
{
    /*state 0: media-playlist-consecutive-symbolic
            1: media-playlist-shuffle
            2:media-playlist-repeat-symbolic
    */
    if(shuffle_state==0)
    {
        shuffle = true;
        shufflePlaylist();
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-shuffle"));
        ui->shuffle_btn->setToolTip("Repeat");
        shuffle_state=1;

    }else if (shuffle_state==1)
    {

        repeat = true;
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-repeat"));
        ui->shuffle_btn->setToolTip("Consecutive");
        shuffle_state=2;


    }else if(shuffle_state==2)
    {
        repeat = false;
        shuffle = false;
        ui->shuffle_btn->setIcon(QIcon::fromTheme("media-playlist-consecutive-symbolic"));
        ui->shuffle_btn->setToolTip("Shuffle");
        shuffle_state=0;
    }



}

void MainWindow::on_open_btn_clicked()
{
    //bool startUpdater = false;

    //if(ui->listWidget->count() == 0) startUpdater = true;




      QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),"/home/Music", tr("Audio (*.mp3 *.wav *.mp4 *.flac *.ogg)"));
    if(!files.empty())
    {



        playlist.add(files);
        updateList();
        //populateTableView();
        //ui->save->setChecked(false);
        if(shuffle) shufflePlaylist();
        //if(startUpdater) updater->start();
    }
}

void MainWindow::populateTableView()
{

    QSqlQuery query= settings_widget->getCollectionDB().getQuery("SELECT * FROM tracks");

       while (query.next())
       {


           ui->tableWidget->insertRow(ui->tableWidget->rowCount());
           auto *title= new QTableWidgetItem( query.value(0).toString());
           //title->setFlags(title->flags() & ~Qt::ItemIsEditable);

           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, TITLE, title);

           auto *artist= new QTableWidgetItem( query.value(1).toString());
           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, ARTIST, artist);

           auto *album= new QTableWidgetItem( query.value(2).toString());
           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, ALBUM, album);

           auto *location= new QTableWidgetItem( query.value(3).toString());
           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, LOCATION, location);

       }


    /*for (Track track : collection.getTracks() )
    {
     ui->tableWidget->insertRow(ui->tableWidget->rowCount());
     auto *title= new QTableWidgetItem( QString::fromStdString(track.getTitle()));
     //title->setFlags(title->flags() & ~Qt::ItemIsEditable);

     ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, TITLE, title);

     auto *artist= new QTableWidgetItem( QString::fromStdString(track.getArtist()));
     ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, ARTIST, artist);

     auto *album= new QTableWidgetItem( QString::fromStdString(track.getAlbum()));
     ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, ALBUM, album);

     auto *location= new QTableWidgetItem( QString::fromStdString(track.getLocation()));
     ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, LOCATION, location);

    }*/
}

void MainWindow::populateMainList()
{
    QSqlQuery query= settings_widget->getCollectionDB().getQuery("SELECT * FROM tracks WHERE babe = 1");

    QStringList files;
       while (query.next())
       {



        files << query.value(3).toString();

       }

       playlist.add(files);
       updateList();
       //populateTableView();
       //ui->save->setChecked(false);
       if(shuffle) shufflePlaylist();
}

void MainWindow::updateList()
{
    ui->listWidget->clear();
    ui->listWidget->addItems(playlist.getTracksNameList());
}

void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    lCounter = getIndex();

    //ui->play_btn->setChecked(false);
    //ui->searchBar->clear();
    loadTrack();
    player->play();
    updater->start();
    playing= true;
    ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));

}

void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    //QMessageBox::information(NULL,"QTableView Item Double Clicked",index.sibling(ui->tableWidget->currentIndex().row(),LOCATION).data().toString());

   /*
   player->setMedia(QUrl::fromLocalFile(index.sibling(ui->tableWidget->currentIndex().row(),LOCATION).data().toString()));
   player->play();
   updater->start();
   this->setWindowTitle(index.sibling(ui->tableWidget->currentIndex().row(),TITLE).data().toString() +" \xe2\x99\xa1 " +index.sibling(ui->tableWidget->currentIndex().row(),ARTIST).data().toString());
   */
    QStringList files;
    files << index.sibling(ui->tableWidget->currentIndex().row(),LOCATION).data().toString();
    playlist.add(files);
    updateList();
    //populateTableView();
    //ui->save->setChecked(false);
    if(shuffle) shufflePlaylist();
    //if(startUpdater) updater->start();
}

void MainWindow::loadTrack()
{
     current_song_url = QString::fromStdString(playlist.tracks[getIndex()].getLocation());
     player->setMedia(QUrl::fromLocalFile(current_song_url));
     auto qstr = QString::fromStdString(playlist.tracks[getIndex()].getTitle()+" \xe2\x99\xa1 "+playlist.tracks[getIndex()].getArtist());
     this->setWindowTitle(qstr);

     //here check if the song to play is already babe'd and if so change the icon
      if(settings_widget->getCollectionDB().checkQuery("SELECT * FROM tracks WHERE location = '"+current_song_url+"' AND babe = '1'"))
      {
          ui->fav_btn->setIcon(QIcon::fromTheme("face-in-love"));
      }else
      {
          ui->fav_btn->setIcon(QIcon::fromTheme("love"));
      }




     qDebug()<<"Current song playing is: "<< current_song_url;
}

int MainWindow::getIndex()
{
    return ui->listWidget->currentIndex().row();
}



void MainWindow::on_seekBar_sliderMoved(int position)
{
    player->setPosition(player->duration() / 1000 * position);
}






void MainWindow::update()
{   if(!ui->seekBar->isSliderDown())
        ui->seekBar->setValue((double)player->position()/player->duration() * 1000);

    if(player->state() == QMediaPlayer::StoppedState)
    {
        next();
    }
}

void MainWindow::next()
{
    lCounter++;

    if(repeat)
    {
        lCounter--;
    }

    if(lCounter >= ui->listWidget->count())
        lCounter = 0;

    (!shuffle or repeat) ? ui->listWidget->setCurrentRow(lCounter) : ui->listWidget->setCurrentRow(shuffledPlaylist[lCounter]);

    //ui->play->setChecked(false);
    //ui->searchBar->clear();

    loadTrack();
    player->play();

}

void MainWindow::back()
{
     lCounter--;

     if(lCounter < 0)
        lCounter = ui->listWidget->count() - 1;


     (!shuffle) ? ui->listWidget->setCurrentRow(lCounter) : ui->listWidget->setCurrentRow(shuffledPlaylist[lCounter]);

     //ui->play->setChecked(false);
     //ui->searchBar->clear();

     loadTrack();
     player->play();
}

void MainWindow::shufflePlaylist()
{
    shuffledPlaylist.resize(0);

    for(int i = 0; i < ui->listWidget->count(); i++)
    {
        shuffledPlaylist.push_back(i);
    }

    random_shuffle(shuffledPlaylist.begin(), shuffledPlaylist.end());
}

void MainWindow::on_play_btn_clicked()
{
    if(ui->listWidget->count() != 0)
    {
        if(player->state() == QMediaPlayer::PlayingState)
        {
            player->pause();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-start"));
        }
       else
       {
            player->play();
            updater->start();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));
       }
      }
}

void MainWindow::on_backward_btn_clicked()
{
    if(ui->listWidget->count() != 0)
    {
        if(player->position() > 3000)
        {
           player->setPosition(0);
        }
        else
        {

            back();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));
        }
     }
}

void MainWindow::on_foward_btn_clicked()
{
    if(ui->listWidget->count() != 0)
    {
        if(repeat)
        {
            repeat = !repeat;next();repeat = !repeat;
        }
        else
        {
            next();
            ui->play_btn->setIcon(QIcon::fromTheme("media-playback-pause"));
        }
     }
}


void MainWindow::collectionDBFinishedAdding(bool state)
{
    if(state)
    {
        qDebug()<<"now it i time to put the tracks in the table ;)";
        populateTableView();
    }
}






void MainWindow::on_info_view_clicked(bool checked)
{

}

void MainWindow::on_tracks_view_clicked(bool checked)
{



}



void MainWindow::on_fav_btn_clicked()
{

    if(settings_widget->getCollectionDB().checkQuery("SELECT * FROM tracks WHERE location = '"+current_song_url+"' AND babe = '1'"))
    {
        //ui->fav_btn->setIcon(QIcon::fromTheme("face-in-love"));
        qDebug()<<"The song is already babed";
        if(settings_widget->getCollectionDB().insertInto("tracks","babe",current_song_url,0))
        {
            ui->fav_btn->setIcon(QIcon::fromTheme("love"));
        }

    }else
    {

                  if(settings_widget->getCollectionDB().check_existance("tracks","location",current_song_url))
                  {
                      if(settings_widget->getCollectionDB().insertInto("tracks","babe",current_song_url,1))
                      {
                          ui->fav_btn->setIcon(QIcon::fromTheme("face-in-love"));
                      }
                      qDebug()<<"trying to babe sth";
                  }else
                  {
                      //to-do: create a list and a tracks object and send it the new song and then write that track list into the database
                  }
    }






}

void MainWindow::on_searchField_clicked()
{

    if(hideSearch)
    {
        utilsBar->hide();
        hideSearch=false;

    }else
    {

        utilsBar->show();
        hideSearch=true;

    }

    if(mini_mode!=0)
    {
        expand();
        utilsBar->show();
        hideSearch=true;
    }
}

void MainWindow::on_tableWidget_clicked(const QModelIndex &index)
{
    qDebug()<<"right clicked!";
}
