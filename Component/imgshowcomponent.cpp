#include "imgshowcomponent.h"
#include "Bin/header.h"

#include "Core/FileHandle.h"

QList<ImgShowComponent*> imgShowList;


static const char *const cursor_xpm[] = {
    "15 15 3 1",
    "   c None",
    ".  c yellow",   //.的颜色
    "*  c #aa0000",   //*的颜色
    "     .....     ",
    "   .. *** ..   ",
    "  .   ***   .  ",
    " .    ***    . ",
    " .    ***    . ",
    ".     ***     .",
    ".     ***     .",
    ".*************.",
    ".     ***     .",
    ".     ***     .",
    " .    ***    . ",
    " .    ***    . ",
    "  .   ***   .  ",
    "   .. *** ..   ",
    "     .....     "
};

ImgShowComponent::ImgShowComponent(QWidget *parent):OpenGLWidget(parent)
{
    //允许鼠标 允许接收文件
    setMouseTracking(true);
    setAcceptDrops(true);

    //设置大小
    setContentsMargins( 0, 0, 0, 0);


    pixmap = new QPixmap(this->width(),this->height());
    pixmap->fill(QColor(255,255,255));
    pixmap->load("P:/smartcar/qt/UpperMonitor/img.bmp");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    //mainLayout->setHorizontalSpacing(0);

    imgLabel = new QLabel(this);
    imgLabel->setContentsMargins(0,0,0,0);
    imgLabel->setFrameStyle(QFrame::NoFrame);
    imgLabel->setStyleSheet(QStringLiteral("border: 0px; background-color: rgba(255, 255, 255, 0.0);color:#CCC;"));
    imgLabel->setAlignment(Qt::AlignCenter);
    imgLabel->setMinimumSize(IMG_COL*4,IMG_ROW*4);

    //imgLabel->resize(188,120);
    imgLabel->setText("imgLabel");//底层文字..

    mainLayout->addWidget(imgLabel);
    textLabel = new QLabel(this);
    //textLabel->resize(600,24);
    textLabel->setMaximumSize(9999,24);

    textLabel->setStyleSheet(QStringLiteral("border: 0px; background-color: rgba(1, 1, 1, 0.3);color:#fff;font-size:14px;font-weight:bold;padding:0 20px;"));
    textLabel->setText("textLabel");

    //setCursor(Qt::CrossCursor);

    setCursor(QCursor(QPixmap(cursor_xpm)));

    mainLayout->addWidget(textLabel);

    setLayout(mainLayout);
    mainLayout->setContentsMargins(0,0,0,0);

    connect(this,&ImgShowComponent::updateSurface,this,static_cast<void (ImgShowComponent::*)()>(&ImgShowComponent::repaint));

    QHBoxLayout* ImgShowSettingsLayout = new QHBoxLayout;

    QPushButton* pushBtn = new QPushButton("Setting",this);
    ImgShowSettingsLayout->addWidget(pushBtn);

    QLineEdit* saveDirLineEdit = new QLineEdit("P:/smartcar/monitor/2019/{name}.bmp",this);
    //saveDirLineEdit->setMaximumHeight(30);
    ImgShowSettingsLayout->addWidget(saveDirLineEdit);

    QPushButton* AutoSaveBtn = new QPushButton("Auto",this);
    ImgShowSettingsLayout->addWidget(AutoSaveBtn);

    QPushButton* saveOneBtn = new QPushButton("Save",this);
    ImgShowSettingsLayout->addWidget(saveOneBtn);

    AutoSaveBtn->setCheckable(true);


    QDateTime timestamp = QDateTime::currentDateTime();
    QString times = timestamp.toString("yyMMdd");
    saveDirLineEdit->setText(QString("F:/monitorPics/2019/%1/").arg(times));

    connect(AutoSaveBtn,&QPushButton::toggled,[=](bool checked){
        fileHandle->isAutoSave = !this->isAutoSave;
        fileHandle->saveDir = saveDirLineEdit->text();
        saveDirLineEdit->setEnabled(!checked);
        if(fileHandle->isAutoSave == true)
        {
            if(fileHandle->mkDir(fileHandle->saveDir))
            {
                qDebug()<<QString("%1 目錄創建成功!").arg(fileHandle->saveDir);
            }
        }
    });
    mainLayout->addLayout(ImgShowSettingsLayout);

    emit updateSurface();
    //connect(this,&ImgShowCompnent::updateSurface,this,&ImgShowCompnent::paintGL);
}

ImgShowComponent::~ImgShowComponent()
{
    //imgProcThread.quit();
    //imgProcThread.wait();
}

void ImgShowComponent::paintEvent(QPaintEvent *event)
{
    static int i = 0;
    //qDebug()<<"paint"<<++i<<event->rect();
    Q_UNUSED(event);
    QPainter painter(this);

    QRect parentRect=   QRect(imgLabel->x(),imgLabel->y(),  imgLabel->width()   ,    imgLabel->height() + textLabel->height() );//绘图区域 这里是全部绘图..
    QRect pixRect   =   QRect(0,0,pixmap->width(),pixmap->height());
    painter.drawPixmap(parentRect,*pixmap,pixRect);
}

void ImgShowComponent::mouseMoveEvent(QMouseEvent *event)
{
    static QPoint lastPos(0,0);
    if(receivedCnt == 0)
    {
        return;
    }

    int16_t Ysite,Xsite;
    //TODO 超出图像范围.那么停止更新信息.
    Ysite = Limit(event->pos().y()   * IMG_ROW/ (this->height() - 27),IMG_TOP,IMG_BOTTOM);
    Xsite = Limit(event->pos().x()  * IMG_COL/ this->width(),IMG_LEFT,IMG_RIGHT);

    if(lastPos.x() == Xsite && lastPos.y() == Ysite)
    {
        return;
    }

    lastPos.setX(Xsite);
    lastPos.setY(Ysite);


    if(Ysite >= 0 && Xsite >=0 && Ysite < IMG_ROW && Xsite <IMG_COL)
    {

        QString pStr("%1,%2");
        pStr = pStr.arg(Xsite).arg(Ysite)//这里pos是坐标，你把坐标按比例转换为你的刻度尺就可以了
                ;
        textLabel->setText(pStr);
        emit PosMove(QPoint(Xsite,Ysite));
    }else{
        textLabel->setText("pStr");

    }
}


void ImgShowComponent::dragEnterEvent(QDragEnterEvent *event)
{
    //拖到窗口上的时候的处理事件
    //如果为文件，则支持拖放
    if (event->mimeData()->hasFormat("application/x-qt-windows-mime;value=\"DragImageBits\""))
    {
        qDebug()<<"type valid";
        event->accept();
    }else{
        qDebug()<<"type invalid";
        event->acceptProposedAction();
    }

    //event->setDropAction(Qt::MoveAction);

    qDebug()<<"Drop "<<event->mimeData()->text()<<"\r\n";

}

//跑道原始图像链表
static QList<QImage> trackPicsList;//原始图像队列
static QStandardItemModel model;
static QImage dropImageTemp;

void ImgShowComponent::dropEvent(QDropEvent *event)
{
    //拖动的处理 Todo : 增加文件拖动列表
    //qDebug()<<"Update Drop Img";
    QList<QUrl> urls = event->mimeData()->urls();

    if(urls.isEmpty())  return;

    emit dropSignals(urls);
    return;
    if(urls.size() == 1)
    {
        //这里是拖入的图像 在验证为符合类型的图像之后,保存进入队列.
        //比如判断图片类型,长宽高..但是判断应该是需要先加载到缓存中的.
        //如果是不符合标准的图像.留在 “缓冲区”

        //加载图像到..
        dropImageTemp.load(urls[0].toLocalFile());
        //QImage image(urls[0].toLocalFile());

        trackPicsList.append(dropImageTemp);

        pixmap->fromImage(dropImageTemp);


        //发送处理命令
        emit dropSignal(dropImageTemp);

        //显示出来
        *pixmap = QPixmap::fromImage(dropImageTemp);
        //*pixmap = QPixmap::fromImage(dropImageTemp);
        emit updateSurface();
    }
}

void ImgShowComponent::updateImgWidget(QPixmap packedPixmap)
{
    //qDebug()<<packedPixmap;
    *pixmap = packedPixmap;
    receivedCnt++;
    emit updateSurface();
}
void ImgShowComponent::updateImgWidgetPtr(QPixmap* packedPixmapPtr)
{
    qDebug()<<packedPixmapPtr;
    pixmap = packedPixmapPtr;
    receivedCnt++;
    emit updateSurface();
}


void ImgShowComponent::updateImage(QImage imagePtr)
{
    //显示出来
    *pixmap = QPixmap::fromImage(imagePtr,Qt::AutoDither);
    receivedCnt++;

    emit updateSurface();
}
