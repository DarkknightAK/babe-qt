#include "infoview.h"
#include "ui_infoview.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QToolButton>
InfoView::InfoView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoView)
{
    ui->setupUi(this);
    artist= new Album(":Data/data/cover.png",120,100);
    ui->lyricsText->setLineWrapMode(QTextEdit::NoWrap);
    ui->lyricsText->setStyleSheet("QTextBrowser{background-color: #575757; color:white;}");


    artist->titleVisible(false);
    artist->borderColor=true;
    auto artistContainer = new QWidget();
    artistContainer->setBackgroundRole(QPalette::Dark);
    auto artistCLayout = new QHBoxLayout();
    auto *left_spacer = new QWidget();
    left_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *right_spacer = new QWidget();
    right_spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    artistCLayout->addWidget(left_spacer);
    artistCLayout->addWidget(artist);
    artistCLayout->addWidget(right_spacer);
    artistContainer->setLayout(artistCLayout);
    artistContainer->setMaximumSize(200,200);
    ui->artistLayout->insertWidget(0,artistContainer);

    infoUtils = new QWidget();
    //artistContainer->setStyleSheet("QWidget{background-color: #575757; color:white;}");
    infoUtils->setFixedWidth(200);
    auto infoUtils_layout= new QHBoxLayout();
    infoUtils_layout->setContentsMargins(0,0,0,0);
    infoUtils_layout->setSpacing(0);
    auto similarBtn = new QToolButton();
    similarBtn->setAutoRaise(true);
    similarBtn->setIcon(QIcon::fromTheme("similarartists-amarok"));


    auto moreBtn = new QToolButton();
    moreBtn->setAutoRaise(true);
    moreBtn->setIcon(QIcon::fromTheme("filename-discnumber-amarok"));

    auto loveBtn = new QToolButton();
    loveBtn->setAutoRaise(true);
    loveBtn->setIcon(QIcon::fromTheme("preferences-media-playback-amarok"));

    hideBtn = new QToolButton();
    connect(hideBtn,SIGNAL(clicked()),this,SLOT(hideArtistInfo()));

    hideBtn->setAutoRaise(true);
    hideBtn->setIcon(QIcon::fromTheme("hide_table_column"));

    infoUtils_layout->addWidget(similarBtn);
    infoUtils_layout->addWidget(moreBtn);
    infoUtils_layout->addWidget(loveBtn);
    infoUtils_layout->addWidget(hideBtn);
    infoUtils->setLayout(infoUtils_layout);


}

InfoView::~InfoView()
{
    delete ui;
}

void InfoView::hideArtistInfo()
{
    qDebug()<<"hide artist info";
    if(hide)
    {
        ui->artistFrame->hide();
        infoUtils->hide();
        hideBtn->setIcon(QIcon::fromTheme("show_table_column"));
        hide=false;
    }else
    {
        ui->artistFrame->show();
        infoUtils->show();
        hideBtn->setIcon(QIcon::fromTheme("hide_table_column"));
        hide=true;
    }

}


void InfoView::setAlbumInfo(QString info)
{

    qDebug()<<info;
    if(info.isEmpty())
    {
        ui->albumText->hide(); ui->frame_4->hide();
        ui->frame_5->hide();
    }else
    {
         ui->albumText->show(); ui->frame_4->show();
         ui->frame_5->show();
    ui->albumText->setHtml(info);
    }
}

void InfoView::setAlbumArt(QByteArray array)
{

}

void InfoView::setArtistInfo(QString info)
{
    ui->artistText->setHtml(info);
}

void InfoView::setArtistArt(QByteArray array)
{
    artist->putPixmap(array);
}

void InfoView::setLyrics(QString lyrics)
{
    ui->lyricsText->setHtml(lyrics);
     ui->lyricsLayout->setAlignment(Qt::AlignCenter);
     ui->lyricsFrame->show();
}
