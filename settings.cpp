#include "settings.h"
#include "ui_settings.h"
#include "QDebug"
#include <QDirIterator>
#include <QFileDialog>
#include <QDebug>
#include <fstream>
#include<iostream>
#include<QStringList>
#include <QThread>
#include<collectionDB.h>
#include<QMessageBox>
#include <QFrame>
#include <QFileSystemWatcher>

settings::settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settings)
{
    ui->setupUi(this);
    //QFrame frame = new QFrame();

    //collectionDBPath=QDir().absolutePath()+collectionDBPath;
    qDebug()<<"Getting collectionDB info from: "<<collectionDBPath;
    qDebug()<<"Getting settings info from: "<<settingPath;

    QDir collectionDBPath_dir (collectionDBPath);
    QDir settingsPath_dir (settingPath);



    if (!collectionDBPath_dir.exists()) settingsPath_dir.mkpath(".");
    if (!settingsPath_dir.exists()) settingsPath_dir.mkpath(".");

    connect(this, SIGNAL(collectionPathChanged(QString)), this, SLOT(populateDB(QString)));
    connect(&collection_db, SIGNAL(DBactionFinished(bool)),this, SLOT(finishedAddingTracks(bool)));
    connect(&collection_db, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
    //thread = new QThread(parent);
    //thread->setTerminationEnabled=true;
    //collection_db.moveToThread(thread);
    //connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    //  connect(thread, SIGNAL(started()), &collection_db, SLOT(addTrack()));
    ui->progressBar->hide();
    about_ui = new About();


}

settings::~settings()
{
    delete ui;
    collection_db.closeConnection();
}

CollectionDB& settings::getCollectionDB()
{
    return collection_db;
}


void settings::on_toolbarIconSize_activated(const QString &arg1)
{
    //qDebug () <<arg1;
    iconSize=arg1.toInt();
    setSettings({"toolbarIconSize=",arg1});

    emit toolbarIconSizeChanged(iconSize);
}



void settings::on_open_clicked()
{
    QString url= QFileDialog::getExistingDirectory();

    //qDebug()<<url;
    if(!collectionPaths.contains(url) && url.size()!=0)
    {
        ui->collectionPath->addItem(url);
        collectionPaths<<url;
        qDebug()<<"Collection dir added: "<<url;
        setSettings({"collectionPath=",url});
        emit collectionPathChanged(url);
    }
}

void settings::setSettings(QStringList setting)
{
    std::string strNew;
    //std::string strReplace;
    strNew = setting.at(0).toStdString()+setting.at(1).toStdString();
    bool replace=false;
    /**/
   // qDebug()<<setting.at(0);
    std::ifstream settings(settingPath.toStdString()+settingsName.toStdString());
    QStringList newline;
    std::string line;
    while (std::getline(settings, line))
    {
       //qDebug()<<get_setting;
       if(QString::fromStdString(line).contains(setting.at(0)))
       {
           if (!QString::fromStdString(line).contains("collectionPath="))
           {
               replace=true;
               newline<<QString::fromStdString(strNew);
           }

       }else
       {
           newline<<QString::fromStdString(line);
       }
    }

    if(replace)
    {
        std::ofstream write(settingPath.toStdString()+settingsName.toStdString());

        for(auto ln : newline)
        {
             write << ln.toStdString() << std::endl;
        }

    }else
    {
        std::ofstream write(settingPath.toStdString()+settingsName.toStdString(), std::ios::app);
        write << strNew<< std::endl;
    }
}

void settings::addToWatcher(QStringList paths)
{
    auto watcher= new QFileSystemWatcher(paths);
    // watcher->addPath(path);
    connect(watcher,SIGNAL(fileChanged(QString)),this,SLOT(handleFileChanged(QString)));
       connect(watcher,SIGNAL(directoryChanged(QString)),this,SLOT(handleDirectoryChanged(QString)));
      // connect(watcher,SIGNAL((QString)),this,SLOT(handleFileChanged(QString)));



}

void settings::collectionWatcher()

{
    QSqlQuery query=collection_db.getQuery("SELECT * FROM tracks");
QStringList files;
QStringList dirs;
      while (query.next())
      {
         if (!dirs.contains(QFileInfo (query.value(3).toString()).dir().path())) dirs << QFileInfo (query.value(3).toString()).dir().path();


          files<<query.value(3).toString();

      }

      addToWatcher(files+dirs);

     // for(auto m: dirs) qDebug()<<m;


}

void settings::handleFileChanged(QString file)
{

    qDebug()<<"this file changed: "<< file;
    emit fileChanged(file);
}

void settings::handleDirectoryChanged(QString dir)
{

    qDebug()<<"this directory changed: "<< dir;
    emit dirChanged(dir);
}

void settings::readSettings()
{
    std::ifstream settings(settingPath.toStdString()+settingsName.toStdString());
    std::string line;
    while (std::getline(settings, line))
    {
       auto get_setting = QString::fromStdString(line);
       //qDebug()<<get_setting;
       if(get_setting.contains("collectionPath="))
       {
           collectionPaths<<get_setting.replace("collectionPath=","");
           qDebug()<< "Setting the cPath: " << get_setting.replace("collectionPath=","");
           ui->collectionPath->addItem(get_setting.replace("collectionPath=",""));




           //connect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(handleFileChanged(const QString&)));
       }





       if(get_setting.contains("toolbarIconSize="))
       {
           iconSize=get_setting.replace("toolbarIconSize=","").toInt();
           qDebug()<< "Setting the tSize: " << iconSize;

           switch(iconSize)
           {
               case 16: ui->toolbarIconSize->setCurrentIndex(0); break;
               case 22: ui->toolbarIconSize->setCurrentIndex(1); break;
               case 24: ui->toolbarIconSize->setCurrentIndex(2); break;
               default: qDebug()<<"error setting icon size";
           }
       }
    }
}




bool settings::checkCollection()
{
   // QString collection_db_path="collection.db";
    QFileInfo check_db (collectionDBPath+collectionDBName);
    if (check_db.exists())
    {
       qDebug()<<"The CollectionDB does not exists.";
       //collection_db.setCollectionDB(collection_db_path);
       //qDebug()<<"Ahora obtener la informacion de ella y populate tableView";
       //populateTableView();
       collection_db.openCollection(collectionDBPath+collectionDBName);
        collectionWatcher();
       return true;
    }else
    {
        qDebug()<<"The CollectionDB doesn't exists. Going to create the database and tables";
        collection_db.openCollection(collectionDBPath+collectionDBName);
        collection_db.prepareCollectionDB();

        return false;
    }
}

void settings::populateDB(QString path)
{


    qDebug() << "Function Name: " << Q_FUNC_INFO << "new path for database action: "<< path;

    QStringList urlCollection;
//QDir dir = new QDir(url);

    if(QFileInfo(path).isDir())

    {
        QDirIterator it(path, QStringList() << "*.mp4" << "*.mp3" << "*.wav" <<"*.flac" <<"*.ogg" <<"*.m4a", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            urlCollection<<it.next();

            //qDebug() << it.next();
        }
    }else if(QFileInfo(path).isFile())
    {
        urlCollection<<path;
        qDebug ()<< path;
    }

    Playlist *collection= new Playlist();
    collection->addClean(urlCollection);
    //updateList();
   collection_db.setTrackList(collection->getTracks());
   ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(urlCollection.size());
    ui->progressBar->show();
  collection_db.addTrack();
   // thread->start();




//collection_db.start();

    /*for(auto tr:urlCollection)
    {
        qDebug()<<tr;
    }
    //populateTableView();*/
}

void settings::finishedAddingTracks(bool state)
{
    if(state)
    {
        qDebug()<<"good to hear it g¿finished yay!";
        ui->progressBar->hide();
        collectionWatcher();
        //collection_db.closeConnection();
    //thread->terminate();

        emit collectionDBFinishedAdding(true);

    }

    //qDebug()<<"good to hear it g¿finished yay!!!!!!";
}



void settings::on_pushButton_clicked()
{
   //QMessageBox::about(this, "Babe Tiny Music Player","Version: 0.0 Alpha\nWritten and designed\nby: Camilo Higuita");
    about_ui->show();
}


void settings::on_pushButton_2_clicked()
{
    QMessageBox::aboutQt(this,"Babe Qt...");
}
