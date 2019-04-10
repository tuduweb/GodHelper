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

//指针地址 值 判断测试
void ImgProc::timerEvent(QTimerEvent * event)
{
    //qDebug()<<this->imgArrayPtr;
}

ImgProc::ImgProc(QObject *parent) : QObject(parent)
{
    display = new ImgProcDisplay(this);
    //image = new QImage(188,120,QImage::Format_Grayscale8);
    //imgArrayPtr = (uchar *)image->bits();
    //imgArray = (uchar (*)[IMG_ROW][IMG_COL])image->bits();
    this->startTimer(2000);

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

    Process2(imgArrayPtr,95,15);
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


//数据源 宽 高
BYTE FastOSTU(const BYTE* pIn9, int width9, int height9)
{

    #define GrayScale 256
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width9 * height9/4;
    unsigned char threshold = 0;
    const unsigned char* data = pIn9;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    unsigned long int gray_sum = 0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height9; i+=2)
    {
        for (j = 0; j < width9; j+=2)
        {
            pixelCount[(int)data[i * width9 + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width9 + j];       //灰度值总和
        }
    }

    //计算每个像素值的点在整幅图像中的比例

    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;

    }

    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;



        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
        for (j = 0; j < GrayScale; j++)
        {

                w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
                u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值 //////// 灰度值 * 比例 =

               w1=1-w0;
               u1tmp=gray_sum/pixelSum-u0tmp;

                u0 = u0tmp / w0;              //背景平均灰度
                u1 = u1tmp / w1;              //前景平均灰度
                u = u0tmp + u1tmp;            //全局平均灰度
                deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
                if (deltaTmp > deltaMax)
                {
                    deltaMax = deltaTmp;
                    threshold = j;
                }
                if (deltaTmp < deltaMax)
                {
                    break;
                }

         }

    return threshold;
}

#define CPPCODE(code) code


//单纯跟踪左边沿的CANNY[SOBEL算子]
void Sobel(void)
{
    //
}


//求全局的sobel算子
//
void FullSobel(void)
{
    //
}



static int borderPic[IMG_ROW][IMG_COL] = {0};//全局 带回拐
static int borderLeft[IMG_ROW] = {0};//不带回拐 一行一个
static int borderRight[IMG_ROW] = {0};//不带回拐 一行一个

static int borderGradX[IMG_ROW][IMG_COL] = {0};
static int borderGradY[IMG_ROW][IMG_COL] = {0};
static int borderGrad[IMG_ROW][IMG_COL] = {0};


void ImgProc::SobelOnePoint(BYTE* imgPtr,PointGradTypeDef* g,LINE row,LINE col)
{
    BYTE (*imageRaw)[120][188] = (uchar (*)[120][188])imgPtr;

    g->gradY = (*imageRaw)[row - 1][col+1] - (*imageRaw)[row - 1][col-1]
            + ( (*imageRaw)[row][col+1] << 1 ) - ( (*imageRaw)[row - 1][col-1] << 1 ) + (*imageRaw)[row + 1][col+1] - (*imageRaw)[row - 1][col-1];

    g->gradX = -(*imageRaw)[row - 1][col-1] - ( (*imageRaw)[row - 1][col] << 1 )
            - (*imageRaw)[row - 1][col+1] + (*imageRaw)[row + 1][col + 1] + (*imageRaw)[row + 1][col - 1] + ( (*imageRaw)[row + 1][col] << 1 );

    g->grad = (int)(( Abs(g->gradX) + Abs(g->gradY) )*1.0f / 2 + 0.5f);
    g->gradYX = g->gradY*1.0f / g->gradX;
}
//SOBEL梯度跟踪算法
void ImgProc::Process1(BYTE* imgPtr,LINE startRow,LINE endRow)
{
    //计算近处全局灰度 隔行 隔列1 0 1 0 1 0 1
    BYTE (*imageRaw)[120][188] = (uchar (*)[120][188])imgPtr;
    BYTE* rowPtr = (*imageRaw)[IMG_BOTTOM - 7];

    BYTE th = FastOSTU(rowPtr,IMG_COL,7);//阈值

    //阈值限制.. 这里是大津法动态阈值.如果在十字的情况是会出问题的..所以需要添加判断方法


    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgb(135,206,250)), 1, Qt::SolidLine)));


    //初始化..
    memset(borderPic,0,sizeof(borderPic));
    memset(borderLeft,0,sizeof(borderLeft));
    memset(borderRight,0,sizeof(borderRight));
    memset(borderGradX,0,sizeof(borderGradX));
    memset(borderGradY,0,sizeof(borderGradY));
    memset(borderGrad,0,sizeof(borderGrad));

    LINE row = startRow;
    LINE col = IMG_COL/2 - 1;

    for(;row >= startRow - 7; --row)
    {
        rowPtr = (*imageRaw)[row];
        //从某一列开始
        for(col = IMG_COL/2 - 1; col < IMG_COL; col++)
        {
            //右边
#if 1
            if(rowPtr[col] < th && borderRight[row] == 0)
            {
                display->DrawPoint(col,row);
                qDebug()<<row<<col;
                borderRight[row] = col;
            }
#endif
#if 1
            //左边
            if(rowPtr[IMG_COL - col] < th && borderLeft[row] == 0)
            {
                display->DrawPoint(IMG_COL - col,row);
                qDebug()<<row<<col;
                borderLeft[row] = IMG_COL - col;
            }
#endif
        }
    }


    //对求出来的几个点进行连续性判断..

    //Sobel算子 跟踪边沿
    col = (borderLeft[row + 1] + borderLeft[row + 3]) / 2;
    row += 2;//从上个大津法跳过的地方开始..
    CPPCODE(display->H.value("H").painter->setPen(QPen(Qt::red, 1, Qt::SolidLine)));
    display->DrawPoint(col,row);

    float tanYX = 0;

    for(int cnt = 0;InRange(col,IMG_LEFT,IMG_RIGHT) && InRange(row,endRow,IMG_BOTTOM)&& cnt < 90;cnt++)
    {
        borderGradY[row][col] = (*imageRaw)[row - 1][col+1] - (*imageRaw)[row - 1][col-1]
                + ( (*imageRaw)[row][col+1] << 1 ) - ( (*imageRaw)[row][col-1] << 1 ) + (*imageRaw)[row + 1][col+1] - (*imageRaw)[row + 1][col-1];

        borderGradX[row][col] = -(*imageRaw)[row - 1][col-1] - ( (*imageRaw)[row - 1][col] << 1 )
                - (*imageRaw)[row - 1][col+1] + (*imageRaw)[row + 1][col + 1] + (*imageRaw)[row + 1][col - 1] + ( (*imageRaw)[row + 1][col] << 1 );

        borderGrad[row][col] = (int)(( borderGradX[row][col] + borderGradY[row][col] )*1.0f / 2 + 0.5f);

        CPPCODE(display->H.value("H").painter->setPen(QPen(Qt::blue, 1, Qt::SolidLine)));

        //指向下一个点..
        tanYX = borderGradY[row][col]*1.0f / borderGradX[row][col];
        if(Absf(tanYX) > 3.0776835f)//2.4142135624 //3.0776835372
        {
            row -= 1;
        }else if(tanYX > 0.7265425f)//0.7265425280
        {
            row -= 1;
            col += 1;
        }else if(tanYX < -0.7265425f)
        {
            row -= 1;
            col -= 1;
        }else if(tanYX > 0.0f)
        {
            col += 1;
        }else{
            col -= 1;
        }

        display->DrawPoint(col,row);
        qDebug()<<QString("Paint (%1,%2) gradY %3 gradX %4   tanYX %5")
                  .arg(col).arg(row).arg(borderGradY[row][col]).arg(borderGradX[row][col]).arg(tanYX);
    }
}



struct PointTypeDef
{
    LINE x;
    LINE y;
};

void ImgProc::Process2(BYTE* imgPtr,LINE startRow,LINE endRow)
{
    PointTypeDef lastPoint,prevPoint;
    PointTypeDef cPoint,lPoint,rPoint;
    //计算近处全局灰度 隔行 隔列1 0 1 0 1 0 1
    BYTE (*imageRaw)[120][188] = (uchar (*)[120][188])imgPtr;
    BYTE* rowPtr = (*imageRaw)[IMG_BOTTOM - 7];

    BYTE th = FastOSTU(rowPtr,IMG_COL,7);//阈值

    //阈值限制.. 这里是大津法动态阈值.如果在十字的情况是会出问题的..所以需要添加判断方法


    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgb(135,206,250)), 1, Qt::SolidLine)));


    //初始化..
    memset(borderPic,0,sizeof(borderPic));
    memset(borderLeft,0,sizeof(borderLeft));
    memset(borderRight,0,sizeof(borderRight));
    memset(borderGradX,0,sizeof(borderGradX));
    memset(borderGradY,0,sizeof(borderGradY));
    memset(borderGrad,0,sizeof(borderGrad));
    prevPoint.x = prevPoint.y = lastPoint.y = lastPoint.x = 0;
    cPoint.x = cPoint.y = lPoint.x = lPoint.y = rPoint.x = rPoint.y = 0;

    LINE row = startRow;
    LINE col = IMG_COL/2 - 1;


    //大津法判断起始边界 减少运算量
    //我们有理由相信近处的边界误差较小
    for(;row >= startRow - 7; --row)
    {
        rowPtr = (*imageRaw)[row];
        //从某一列开始
        for(col = IMG_COL/2 - 1; col < IMG_COL; col++)
        {
            //右边
#if 1
            if(rowPtr[col] < th && borderRight[row] == 0)
            {
                display->DrawPoint(col,row);
                qDebug()<<row<<col;
                borderRight[row] = col;
            }
#endif
#if 1
            //右边
            if(rowPtr[IMG_COL - col] < th && borderLeft[row] == 0)
            {
                display->DrawPoint(IMG_COL - col,row);
                qDebug()<<row<<col;
                borderLeft[row] = IMG_COL - col;
            }
#endif
        }
    }


    //对求出来的几个点进行连续性判断..

    //Sobel算子 跟踪边沿
    col = (borderLeft[row + 1] + borderLeft[row + 3]) / 2;
    row += 2;//从上个大津法跳过的地方开始..
    CPPCODE(display->H.value("H").painter->setPen(QPen(Qt::red, 1, Qt::SolidLine)));
    display->DrawPoint(col,row);

    cPoint.x = col;
    rPoint.y = lPoint.y = cPoint.y = row;
    lPoint.x = col - 1;
    rPoint.x = col + 1;

    float tanYX = 0;

    PointGradTypeDef l,c,r;
    for(int cnt = 0;InRange(col,IMG_LEFT + 1,IMG_RIGHT - 1) && InRange(row,endRow + 1,IMG_BOTTOM - 1)&& cnt < 90;cnt++)
    {

        SobelOnePoint(imgPtr,&c,cPoint.y,cPoint.x);

        SobelOnePoint(imgPtr,&l,lPoint.y,lPoint.x);

        SobelOnePoint(imgPtr,&r,rPoint.y,rPoint.x);


        //三点综合判断..选取权值最高的点当做...

        #if 0
        if(l.grad > c.grad && l.grad > r.grad)
        {
            col = lPoint.x;
            borderGradY[row][col] = l.gradY;
            borderGradX[row][col] = l.gradX;
            borderGrad[row][col] = l.grad;
            qDebug("Move Left");
        }else if(r.grad > c.grad && r.grad > l.grad)
        {
            col = rPoint.x;
            borderGradY[row][col] = r.gradY;
            borderGradX[row][col] = r.gradX;
            borderGrad[row][col] = r.grad;
            qDebug("Move Right");
        }else// if(c.grad >= l.grad && c.grad >= r.grad)
        #endif
        {

            borderGradY[row][col] = c.gradY;
            borderGradX[row][col] = c.gradX;
            borderGrad[row][col] = c.grad;
        }



        /*
        borderGradY[row][col] = (*imageRaw)[row - 1][col+1] - (*imageRaw)[row - 1][col-1]
                + ( (*imageRaw)[row][col+1] << 1 ) - ( (*imageRaw)[row][col-1] << 1 ) + (*imageRaw)[row + 1][col+1] - (*imageRaw)[row + 1][col-1];

        borderGradX[row][col] = -(*imageRaw)[row - 1][col-1] - ( (*imageRaw)[row - 1][col] << 1 )
                - (*imageRaw)[row - 1][col+1] + (*imageRaw)[row + 1][col + 1] + (*imageRaw)[row + 1][col - 1] + ( (*imageRaw)[row + 1][col] << 1 );

        borderGrad[row][col] = (int)(( borderGradX[row][col] + borderGradY[row][col] )*1.0f / 2 + 0.5f);
        */

        CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(170,238,0,50), 1, Qt::SolidLine)));

        //指向下一个点..
        tanYX = borderGradY[row][col]*1.0f / borderGradX[row][col];


        qDebug()<<QString("Paint (%1,%2) gradY %3 gradX %4   tanYX %5")
                  .arg(col).arg(row).arg(borderGradY[row][col]).arg(borderGradX[row][col]).arg(tanYX);

        if(Absf(tanYX) > 3.0776835f)//2.4142135624 //3.0776835372
        {
            row -= 1;
            lPoint.y = rPoint.y = cPoint.y = row;
            lPoint.x = cPoint.x - 1;
            rPoint.x = cPoint.x + 1;
        }else if(tanYX > 0.7265425f)//0.7265425280
        {
            row -= 1;
            col += 1;
            cPoint.y = row;
            cPoint.x = col;

            lPoint.y = row - 1;
            lPoint.x = col - 1;

            rPoint.y = row + 1;
            rPoint.x = col + 1;
        }else if(tanYX < -0.7265425f)
        {
            row -= 1;
            col -= 1;
            cPoint.y = row;
            cPoint.x = col;

            lPoint.y = row + 1;
            lPoint.x = col + 1;

            rPoint.y = row - 1;
            rPoint.x = col - 1;
        }else if(tanYX > 0.0f)
        {
            col += 1;
            cPoint.y = row;
            cPoint.x = col;

            lPoint.y = row - 1;
            lPoint.x = col;

            rPoint.y = row + 1;
            rPoint.x = col;
        }else{
            col -= 1;
            cPoint.y = row;
            cPoint.x = col;

            lPoint.y = row - 1;
            lPoint.x = col;

            rPoint.y = row + 1;
            rPoint.x = col;
        }

        //强行递推..
        if( (lastPoint.x == col && lastPoint.y == row) || (prevPoint.x == col && prevPoint.y == row) )
        {
            row -= 1;
            cPoint.y -= 1;
            lPoint.y -= 1;
            rPoint.y -= 1;
        }

        prevPoint = lastPoint;

        lastPoint.x = col;
        lastPoint.y = row;


        display->DrawPoint(col,row);
    }

}


//
