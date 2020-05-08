#ifndef IMGPROC_H
#define IMGPROC_H
#include <QObject>
#include <QImage>
#include <QPainter>

#include "imgprocdisplay.h"
#include "Bin/header.h"

//#define IsBlack(row,col)

typedef unsigned char BYTE;
typedef int16_t LINE;

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

    explicit ImgProc(int type = 0,QObject *parent = nullptr);
    void UpdateCache(QImage* imagePtr);
    void UpdateCache(uchar* imgArrayPtr);

    void doProc(void);


    //大津法..
    void Process_OSTU(void);
    void Process_OSTU_Section(int i);
    void SobelOnePoint(BYTE* imgPtr,PointGradTypeDef* g,LINE row,LINE col);


    void Process1(BYTE* imgPtr,LINE startRow,LINE endRow);

    void Process2(BYTE* imgPtr,LINE startRow,LINE endRow);

    void ProcessCanny(BYTE* imgPtr,LINE startRow,LINE endRow,LINE startCol,LINE endCol);


    void ProcessSimpleCanny(BYTE* imgPtr,LINE startRow,LINE endRow,LINE startCol,LINE endCol);

    void GetOnePointSobel(BYTE (*imageRaw)[120][188],PointGradTypeDef* g,LINE row,LINE col);
    uint8 GetOnePointNMS(BYTE (*imageRaw)[120][188],LINE row,LINE col);//对某点进行极大值抑制.

    BYTE GetAreaThreshold(BYTE (*imageRaw)[120][188],LINE row,LINE col);
    void CreateAreaThresholdMap(BYTE (*imageRaw)[120][188]);
    void ProcessSimpleCannyV2(BYTE* imgPtr,LINE startRow,LINE endRow,LINE startCol,LINE endCol);

    void ProcessImage(BYTE* imgPtr,LINE startRow,LINE endRow,LINE startCol,LINE endCol);


    void CurveFillter(void);
    //void FindJumpPointInRange


    void ProcessSimpleCanny2();

    int currentSection = 0;


protected:
    void timerEvent(QTimerEvent * timeEvent);


signals:
    void UpdateGrayScaleChart(int *chart);

private:
    int processType;

    int globalTH;


public slots:
    void updateOSTUSection(int section);
};

#endif // IMGPROC_H
