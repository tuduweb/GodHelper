#ifndef IMGPROC_H
#define IMGPROC_H
#include <QObject>
#include <QImage>
#include <QPainter>

#include "imgprocdisplay.h"
#include "Bin/header.h"

//#define IsBlack(row,col)

typedef unsigned char BYTE;
typedef int LINE;

struct PointGradTypeDef
{
    int gradX;
    int gradY;
    int grad;
    float gradYX;
};

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
    void SobelOnePoint(BYTE* imgPtr,PointGradTypeDef* g,LINE row,LINE col);


    void Process1(BYTE* imgPtr,LINE startRow,LINE endRow);

    void Process2(BYTE* imgPtr,LINE startRow,LINE endRow);

protected:
    void timerEvent(QTimerEvent * timeEvent);

signals:
    void UpdateGrayScaleChart(int *chart);

public slots:
};

#endif // IMGPROC_H
