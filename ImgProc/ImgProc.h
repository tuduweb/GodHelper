#ifndef IMGPROC_H
#define IMGPROC_H
#include <QObject>
#include <QImage>
#include <QPainter>

#include "imgprocdisplay.h"
#include "Bin/header.h"

//#define IsBlack(row,col)





class ImgProc : public QObject
{
    Q_OBJECT
public:
    ImgProcDisplay *display;
    QImage originalImage;//原始图像..
    QImage* image;
    uchar* imgArrayPtr;

    explicit ImgProc(QObject *parent = nullptr);
    void UpdateCache(QImage* imagePtr);
    void UpdateCache(uchar* imgArrayPtr);

    void test(void);


    //大津法..
    void Process_OSTU(void);

signals:
    void UpdateGrayScaleChart(int *chart);

public slots:
};

#endif // IMGPROC_H
