#include "ImgProc.h"
#include "QDebug"

#include "math.h"

static int testNum = 0;

///////////////////////////////一些基本,特殊方法//////////////////////////////
void GetGrayScale(uchar *ptr,short grayScale[])
{
    //还得输出个最大的值
    //int grayLevel[256] = {0};
    //如果太大了 不统计..
    //输入图片 返回统计信息..
    QRgb a;
    uchar (*imageRaw)[120][188] = (uchar (*)[120][188])ptr;

    //uchar (*imageRaw)[120][188];
    //imageRaw = (uchar (*)[120][188])grayImage->bits();


    for (int row = 0;row < IMG_ROW; row++)
    {
        for(int col = 0;col < IMG_COL; col++ )
        {
            grayScale[ (*imageRaw)[row][col] ] ++;
        }

    }

    for(int i = 0;i<120;i++)
    {
        //qDebug()<<(*imageRaw)[119-i][i];
        //qDebug()<<qGray(grayImage->pixel(i,119-i))<<(*imageRaw)[119-i][i];
        //grayImage->setPixel(i,119 - i,i);
        //qDebug()<<qGray(grayImage->pixel(i,119-i))<<(*imageRaw)[119-i][i];
    }

}




ImgProc::ImgProc(QObject *parent) : QObject(parent)
{
    display = new ImgProcDisplay(this);
    //image = new QImage(188,120,QImage::Format_Grayscale8);
    //imgArrayPtr = (uchar *)image->bits();
    //imgArray = (uchar (*)[IMG_ROW][IMG_COL])image->bits();
}

void ImgProc::UpdateCache(QImage* imagePtr)
{
    //image = imagePtr;
    imgArrayPtr = (uchar *)imagePtr->bits();
}
void ImgProc::UpdateCache(uchar* ptr)
{
    //image = imagePtr;
    imgArrayPtr = ptr;
}


//originalImage
void ImgProc::test(void)
{
    short grayScale[256] = {0};
    GetGrayScale(imgArrayPtr,grayScale);

    display->H.value("H").painter->setPen(QPen(Qt::white, 20, Qt::SolidLine));
    display->H.value("H").painter->drawText(100,50,QString("%1").arg(testNum++));

    qDebug()<<grayScale;

    Process_OSTU();
}


void ImgProc::Process_OSTU(void)
{

    int th = 0;
    const int GrayScale = 256;	//单通道图像总灰度256级
    int pixCount[GrayScale] = {0};//每个灰度值所占像素个数
    int pixSum = IMG_ROW * IMG_COL;//图像总像素点
    float pixPro[GrayScale] = {0};//每个灰度值所占总像素比例
    float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;

    uchar (*imageRaw)[120][188] = (uchar (*)[120][188])imgArrayPtr;


    //imgArray = (uchar (*)[IMG_ROW][IMG_COL])image->bits();

    for(int i = 0; i < IMG_COL; i++)
    {
        for(int j = 0; j < IMG_ROW; j++)
        {
            pixCount[ (*imageRaw)[j][i] ]++;//统计每个灰度级中像素的个数
        }
    }

    UpdateGrayScaleChart(pixCount);

    for(int i = 0; i < GrayScale; i++)
    {
        pixPro[i] = pixCount[i] * 1.0f / pixSum;//计算每个灰度级的像素数目占整幅图像的比例
    }



    for(int i = 0; i < GrayScale; i++)//遍历所有从0到255灰度级的阈值分割条件，测试哪一个的类间方差最大
    {
        w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;
        for(int j = 0; j < GrayScale; j++)
        {
            if(j <= i)//背景
            {
                w0 += pixPro[j];
                u0tmp += j * pixPro[j];
            }
            else//前景
            {
                w1 += pixPro[j];
                u1tmp += j * pixPro[j];
            }
        }
        u0 = u0tmp / w0;
        u1 = u1tmp / w1;
        deltaTmp = (float)(w0 *w1* pow((u0 - u1), 2)); //类间方差公式 g = w1 * w2 * (u1 - u2) ^ 2
        if(deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            th = i;
        }
    }
    qDebug()<<"Result : TH"<<th;


}

//图像分析部分..
