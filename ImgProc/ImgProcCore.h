#ifndef IMGPROCCORE_H
#define IMGPROCCORE_H

#include <QObject>
#include <QImage>
#include <QPixmap>

#include "ImgProc/ImgProc.h"

//本文件只跟处理图像相关 不进行渲染.一定注意!
class ImgProcCore : public QObject
{
    Q_OBJECT
public:
    ImgProc* imgProc;//实现方法
    ImgProcDisplay* display;

    QPixmap *pixmap;//显示的图片


    explicit ImgProcCore(QObject *parent = nullptr);

    void UpdateImgProc();


signals:
    void updateSurface(QPixmap*);
    void AddMsgSignal(uint type,QString* msg);

public slots:
    //传入的参数应该是结构体等..而且需要方便识别号
    void ImageInterface(QImage* image);
    void ByteArrayInterface(QByteArray* byteArray);
    void CharArrayInterface(uchar* charArray);
    void PosMoveSlot(QPoint p);

};

#endif // IMGPROCCORE_H
