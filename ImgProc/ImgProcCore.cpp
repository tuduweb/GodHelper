#include "ImgProcCore.h"
#include <QDebug>
#include <QPainter>

//debug类和绘图类主要是imgProc使用！一定注意..
//封装绘图类..


//封装debug信息类..


#include "Core/FileHandle.h"

ImgProcCore::ImgProcCore(QObject *parent) : QObject(parent)
{
    ////////////////////////////////////////
    /// 图像处理线程..Class
    /// 主要用于处理图像.存在于QT和智能车的c程序,opencv等之间图像处理的桥梁
    /// 2019年3月17日15:43:57
    imgProc = new ImgProc(this);
    display = imgProc->display;

    pixmap = new QPixmap(188,120);
    pixmap->fill(QColor(255,255,255));

    qDebug()<<"Init .. ImgProcCore ";
    qDebug()<<this;
}

void ImgProcCore::ImageInterface(QImage* image)
{

    //imgProc->image = &image;
    imgProc->UpdateCache(image);
    display->PainterStart();


    //uchar (*imageRaw)[120][188];
    //imageRaw = (uchar (*)[120][188])image.bits();
    //QImage img(188,120,QImage::QImage::Format_Grayscale8);

    //display->AddDrawingChannel("ss");
    //display->PainterStart();
    imgProc->test();



    //接收原始图像数据的处理..
    //这里先规定是188*120的格式
    //那么首先要进行的是二值化,提取边沿等算法了吧..

    QPainter base;
    QImage cover(image->width(),image->height(),QImage::Format_ARGB32);
    base.begin(&cover);//QPainter::begin: Cannot paint on an image with the QImage::Format_Indexed8 format
    //执行算法....
    qDebug()<<"imgProcCore";
    base.drawImage(QRect(0,0,image->width(),image->height()),*image);
    //base.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
    //base.drawRect(4,5,50,50);




    base.drawImage(QRect(0,0,image->width(),image->height()),*display->H.value("H").cover);


    display->PainterEnd();
    base.end();

    *pixmap = QPixmap::fromImage(cover);


    emit updateSurface(pixmap);

}

void ImgProcCore::ByteArrayInterface(QByteArray* byteArray)
{
    QImage image;
    //EG 22560.188*120
    //根据图像大小..判别设置或者直接拒绝..
    if( 22560 == (*byteArray).count())
    {
        image.fromData(*byteArray);
        pixmap->fromImage(image);
        //发出更新
        //image.fromData(byteArray);
    }
}

void ImgProcCore::CharArrayInterface(uchar* charArray)
{
    imgProc->UpdateCache(charArray);
    display->PainterStart();


    imgProc->test();

    QPainter base;
    QImage image(charArray,IMG_COL,IMG_ROW,QImage::Format_Grayscale8);//把数据转成图片
    QImage cover(image.width(),image.height(),QImage::Format_ARGB32);//新建底层图像.颜色区域全开
    base.begin(&cover);//QPainter::begin: Cannot paint on an image with the QImage::Format_Indexed8 format
    base.drawImage(QRect(0,0,image.width(),image.height()),image);


    base.drawImage(QRect(0,0,image.width(),image.height()),*display->H.value("H").cover);


    display->PainterEnd();
    base.end();

    *pixmap = QPixmap::fromImage(cover);


    emit updateSurface(pixmap);

    if(true == fileHandle->isAutoSave)
    {
        fileHandle->SaveImage(image);
    }
}

QString strPos;
void ImgProcCore::PosMoveSlot(QPoint p)
{
    PointGradTypeDef grad;
    imgProc->SobelOnePoint(imgProc->imgArrayPtr,&grad,p.y(),p.x());
    strPos = QString("(%1,%2) %3 %4").arg(p.y()).arg(p.x()).arg(grad.gradY).arg(grad.gradX);
    qDebug()<<strPos;
    //emit AddMsgSignal(0,&strPos);

}


