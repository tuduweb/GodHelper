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
void ImgProc::doProc(void)
{
    short grayScale[256] = {0};
    GetGrayScale(imgArrayPtr,grayScale);

    //数字显示
    //display->H.value("H").painter->setPen(QPen(Qt::white, 20, Qt::SolidLine));
    //display->H.value("H").painter->drawText(IMG_COL /2  - 10,10,QString("%1").arg(testNum++));

    qDebug()<<grayScale;

    //Process_OSTU();

    Process_OSTU_Section(currentSection);

    //Process2(imgArrayPtr,95,15);


    ProcessSimpleCannyV2(imgArrayPtr,IMG_BOTTOM - 2,35,IMG_LEFT,IMG_RIGHT);
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

void ImgProc::updateOSTUSection(int section)
{
    if(section < 0 || section >= IMG_ROW)
    {
        qDebug() << "SECTION 错误";
    }else{
        Process_OSTU_Section(section);
        currentSection = section;
    }

}


void ImgProc::Process_OSTU_Section(int section)
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
        for(int j = section; j < IMG_ROW; j++)
        {
            pixCount[ (*imageRaw)[j][i] ]++;//统计每个灰度级中像素的个数
        }
    }

    //更新直方表?
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
            pixelCount[(unsigned char)data[i * width9 + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(unsigned char)data[i * width9 + j];       //灰度值总和
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

    g->gradY = (*imageRaw)[row - 1][col+1] + (*imageRaw)[row + 1][col + 1] + ( (*imageRaw)[row][col+1] << 1 )
            - (*imageRaw)[row - 1][col-1] - ( (*imageRaw)[row][col-1] << 1 ) - (*imageRaw)[row + 1][col - 1];

    g->gradX = -(*imageRaw)[row - 1][col-1] - ( (*imageRaw)[row - 1][col] << 1 ) - (*imageRaw)[row - 1][col+1]
            + (*imageRaw)[row + 1][col - 1] + ( (*imageRaw)[row + 1][col] << 1 ) + (*imageRaw)[row + 1][col + 1];

    g->grad = (int)Abs(g->gradY)+1;//(( Abs(g->gradX) + Abs(g->gradY) )*1.0f / 2 + 0.5f);


    //防止为0的情况
    if(g->gradX == 0)
        g->gradYX = g->gradY*1000;
    else
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
        if(borderGradY[row][col] == 0)
        {
            //row -= 1;
        }else{
            tanYX = - borderGradX[row][col]*1.0f / borderGradY[row][col];
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

        #if 1
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


//区域canny算法
//智能车中的canny算法 没有使用高斯模糊步骤
struct CannyGradTypeDef{
    int gradX;
    int gradY;
    int grad;
    float atan;
};
CannyGradTypeDef grad[IMG_ROW][IMG_COL];

void ImgProc::ProcessCanny(BYTE* imgPtr,LINE startRow,LINE endRow,LINE startCol,LINE endCol)
{
    BYTE (*imageRaw)[IMG_ROW][IMG_COL] = (uchar (*)[IMG_ROW][IMG_COL])imgPtr;
    PointGradTypeDef g;


    //确定矩形区域 远处为0行 近处为MAX - 1 行
    LINE row = startRow - 1,col = startCol + 1;

    for(;row > endRow;--row)
    {
        for(col = startCol + 1;col < endCol;++col)
        {
            SobelOnePoint(imgPtr,&g,row,col);
            grad[row][col].gradX = g.gradX;
            grad[row][col].gradY = g.gradY;
            grad[row][col].atan = atan2(g.gradY,g.gradX);
        }
    }

}

//简化版canny by小王
void ImgProc::ProcessSimpleCanny(BYTE* imgPtr,LINE startRow,LINE endRow,LINE startCol,LINE endCol)
{

    //基本思路 用阈值取出底部的部分边界...在使用canny提取其中的一条边界...再直接极大值抑制或者使用其他方式.得到唯一的一条连通线
    //Q:那要是硬性断开了怎么办呢..
    //那应该就是连不起来了..用常规方法跟线？
    //那怎么确认?后面的是合格的呢? 不知道啊 TAT


    //have a try


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
    for(;row >= startRow - 50; --row)
    {
        rowPtr = (*imageRaw)[row];
        //从某一列开始
        for(col = IMG_COL/2 - 1; col < IMG_COL; col++)
        {
            //右边
#if 1
            if(rowPtr[col] < th)
            {
                display->DrawPoint(col,row);
                qDebug()<<row<<col;
                if(borderRight[row] == 0)
                    borderRight[row] = col;//记录初始点..
            }
#endif
#if 1
            //右边
            if(rowPtr[IMG_COL - col] < th)
            {
                display->DrawPoint(IMG_COL - col,row);
                qDebug()<<row<<col;
                if(borderLeft[row] == 0)
                    borderLeft[row] = IMG_COL - col;//记录初始点..
            }
#endif
        }
    }

    PointGradTypeDef g;

    //测试一下代码..先计算其中所有的点的梯度,方向..并且 多计算几个点的..
    for(row = startRow;row >= startRow - 50;--row)
    {
        for(col = borderLeft[row] + 5;col > startCol;--col)
        {
            SobelOnePoint(imgPtr,&g,row,col);
            grad[row][col].gradX = g.gradX;
            grad[row][col].gradY = g.gradY;
            grad[row][col].grad = g.grad;//(Abs(g.gradY) > 255 ? 255 : Abs(g.gradY));//仅仅使用横向的值测试..
            grad[row][col].atan = atan2(g.gradY,g.gradX);

            if( col > borderLeft[row])
            {
                CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgb(0,grad[row][col].grad,grad[row][col].grad)))));
            }else{
                CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgb(grad[row][col].grad,grad[row][col].grad,180)))));
            }

            CPPCODE(display->DrawPoint(col,row));

        }
    }

    //非极大值抑制
    int g1,g2,g3,g4;
    float dTemp1,dTemp2,dTemp;
    float weight = 0;
    int multiply;

    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgb(255,255,255)))));


    for( row = startRow - 1; row >= startRow - 50 + 1;--row)
    {
        for(col = borderLeft[row + 1] + 4;col > startCol + 1;--col)
        {
            dTemp1 = dTemp2 = dTemp = 0;
            g1 = g2 = g3 = g4 = 0;
            weight = 0;

            if(grad[row][col].gradX == 0)
            {
                //梯度为0的情况..
                //X(竖直)方向梯度为0.. 向着左右
                g1 = g2 = grad[row][col - 1].grad;//(*imageRaw)[row][col - 1];
                g3 = g4 = grad[row][col + 1].grad;//(*imageRaw)[row][col + 1];
            }else if(grad[row][col].gradY == 0)
            {
                //Y(水平)方向梯度为0.. 向着上下
                g1 = g2 = grad[row + 1][col].grad;//(*imageRaw)[row + 1][col];
                g3 = g4 = grad[row - 1][col].grad;//(*imageRaw)[row - 1][col];
            }
            else
            {
                //梯度不为0的情况

                //纵轴 x方向.. 而且是上面的减去下面的
                //横轴 y方向.. 是右边的减去左边的..
                weight = Absf(grad[row][col].gradX * 1.0f / grad[row][col].gradY);//这里已经排除了等于0的情况..
                multiply = grad[row][col].gradX * grad[row][col].gradY;

                if(weight > 1)
                {
                    weight = 1 / weight;

                    //靠近X轴 X轴是纵轴 ..那么根据正负选择..方向.. 方向..
                    //以下为靠近X轴逻辑 纵轴 . row +- 1


                    //GX > GY靠近竖直轴
                    if(multiply > 0)
                    {
                        // 向右,向下.. 右下 右下点.右 左上点.左
                        g1 = grad[row + 1][col + 1].grad;//(*imageRaw)[row + 1][col + 1];
                        g3 = grad[row - 1][col - 1].grad;//(*imageRaw)[row - 1][col - 1];

                        g2 = grad[row + 1][col].grad;//(*imageRaw)[row + 1][col];
                        g4 = grad[row - 1][col].grad;//(*imageRaw)[row - 1][col];
                    }else if(multiply < 0)
                    {
                        // < 0 向右.向上.. or 向左.向下
                        g1 = grad[row + 1][col - 1].grad;//(*imageRaw)[row + 1][col - 1];
                        g3 = grad[row - 1][col + 1].grad;//(*imageRaw)[row - 1][col + 1];

                        g2 = grad[row + 1][col].grad;//(*imageRaw)[row + 1][col];
                        g4 = grad[row - 1][col].grad;//(*imageRaw)[row - 1][col];
                    }

                }else if(weight > 1){
                    //weight < 1

                    //坐标轴 . 向下 向右为正.
                    //靠近Y轴 即水平轴

                    if(multiply > 0)
                    {
                        // 向右,向下.. 右下 右下点.右 左上点.左
                        g1 = grad[row + 1][col + 1].grad;//(*imageRaw)[row + 1][col + 1];
                        g3 = grad[row - 1][col - 1].grad;//(*imageRaw)[row - 1][col - 1];


                        g2 = grad[row][col + 1].grad;//(*imageRaw)[row + 1][col];
                        g4 = grad[row][col - 1].grad;//(*imageRaw)[row - 1][col];

                    }else if(multiply < 0)
                    {
                        // < 0 向右.向上.. or 向左.向下
                        g1 = grad[row + 1][col - 1].grad;//(*imageRaw)[row + 1][col + 1];
                        g3 = grad[row - 1][col + 1].grad;//(*imageRaw)[row - 1][col - 1];

                        g2 = grad[row][col + 1].grad;//(*imageRaw)[row + 1][col];
                        g4 = grad[row][col - 1].grad;//(*imageRaw)[row - 1][col];
                    }


                }else{
                    //weight = 1
                    //相等的情况.那么相邻的正好落在了斜对角上..

                    if(multiply > 0)
                    {
                        g1 = g2 = grad[row + 1][col + 1].grad;
                        g3 = g4 = grad[row - 1][col - 1].grad;
                    }else{//<0的情况..
                        g1 = g2 = grad[row + 1][col - 1].grad;
                        g3 = g4 = grad[row - 1][col + 1].grad;
                    }

                }



            }//end if..grad

            //Jugde
            //weight X/Y

            //计算出临时点..
            dTemp1 = g1 * weight + g2 * (1 - weight);
            dTemp2 = g3 * weight + g4 * (1 - weight);

            //点的类型判断.. 当前点 vs 两个临时点..

            //点..保留..
            if(grad[row][col].grad > dTemp1 && grad[row][col].grad > dTemp2)
            {
                qDebug()<<QString("(%1,%2)%3 %4 %5").arg(row).arg(col).arg(grad[row][col].grad).arg(dTemp1).arg(dTemp2);
                CPPCODE(display->DrawPoint(col,row));
            }

        }
    }

}





#if 0
void ImgProc::ProcessSimpleCannyV2(BYTE* imgPtr,LINE startRow,LINE endRow,LINE startCol,LINE endCol)
{

    PointTypeDef lastPoint,prevPoint;
    PointTypeDef cPoint,lPoint,rPoint;
    //计算近处全局灰度 隔行 隔列1 0 1 0 1 0 1
    BYTE (*imageRaw)[120][188] = (uchar (*)[120][188])imgPtr;
    BYTE* rowPtr = (*imageRaw)[IMG_BOTTOM - 30];

    BYTE th = FastOSTU(rowPtr,IMG_COL,20);//阈值

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

    LINE searchStartCol = IMG_COL/2 - 1;

    //大津法判断起始边界 减少运算量
    //我们有理由相信近处的边界误差较小
    for(;row >= startRow - 60; --row)
    {
        rowPtr = (*imageRaw)[row];
        //从某一列开始
        for(col = searchStartCol; col < IMG_RIGHT; col++)
        {
            //右边
#if 1
            if(rowPtr[col] < th)
            {
                CPPCODE(display->DrawPoint(col,row));
                //qDebug()<<row<<col;
                if(borderRight[row] == 0)
                    borderRight[row] = col;//记录初始点..
            }
#endif
        }
        for(col = searchStartCol; col > IMG_LEFT; col--)
        {
#if 1
            //右边
            if(rowPtr[col] < th)
            {
                CPPCODE(display->DrawPoint(col,row));
                //qDebug()<<row<<col;
                if(borderLeft[row] == 0)
                    borderLeft[row] = col;//记录初始点..
            }
#endif
        }
        searchStartCol = (borderLeft[row] + borderRight[row])>>1;
    }


    PointGradTypeDef g;

    //测试一下代码..先计算其中所有的点的梯度,方向..并且 多计算几个点的..
    for(row = startRow;row >= startRow - 60;--row)
    {
        for(col = endCol;col > startCol;--col)
        {
            SobelOnePoint(imgPtr,&g,row,col);
            grad[row][col].gradX = g.gradX;
            grad[row][col].gradY = g.gradY;
            grad[row][col].grad = g.grad;//(Abs(g.gradY) > 255 ? 255 : Abs(g.gradY));//仅仅使用横向的值测试..
            grad[row][col].atan = atan2(g.gradY,g.gradX);

            if( col < borderLeft[row] || col > borderRight[row])
            {
                CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgb(grad[row][col].grad,grad[row][col].grad,180)))));

            }else{
                CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgb(0,grad[row][col].grad,grad[row][col].grad)))));

            }

            //
            //CPPCODE(display->DrawPoint(col,row));

        }
    }



    //非极大值抑制
    int g1,g2,g3,g4;
    float dTemp1,dTemp2,dTemp;
    float weight = 0;
    int multiply;

    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgba(255,255,255,100)))));


    for( row = startRow - 1; row >= startRow - 60 + 1;--row)
    {

        for(col = Limit(borderRight[row + 1] + 20,IMG_LEFT + 1,endCol - 1);col > Limit(borderLeft[row + 1] - 20,startCol + 1,IMG_RIGHT - 1);--col)
        {
            dTemp1 = dTemp2 = dTemp = 0;
            g1 = g2 = g3 = g4 = 0;
            weight = 0;

            if(grad[row][col].gradX == 0)
            {
                //梯度为0的情况..
                //X(竖直)方向梯度为0.. 向着左右
                g1 = g2 = grad[row][col - 1].grad;//(*imageRaw)[row][col - 1];
                g3 = g4 = grad[row][col + 1].grad;//(*imageRaw)[row][col + 1];
            }else if(grad[row][col].gradY == 0)
            {
                //Y(水平)方向梯度为0.. 向着上下
                g1 = g2 = grad[row + 1][col].grad;//(*imageRaw)[row + 1][col];
                g3 = g4 = grad[row - 1][col].grad;//(*imageRaw)[row - 1][col];
            }
            else
            {
                //梯度不为0的情况

                //纵轴 x方向.. 而且是上面的减去下面的
                //横轴 y方向.. 是右边的减去左边的..
                weight = Absf(grad[row][col].gradX * 1.0f / grad[row][col].gradY);//这里已经排除了等于0的情况..
                multiply = grad[row][col].gradX * grad[row][col].gradY;

                if(weight > 1)
                {
                    weight = 1 / weight;

                    //靠近X轴 X轴是纵轴 ..那么根据正负选择..方向.. 方向..
                    //以下为靠近X轴逻辑 纵轴 . row +- 1


                    //GX > GY靠近竖直轴
                    if(multiply > 0)
                    {
                        // 向右,向下.. 右下 右下点.右 左上点.左
                        g1 = grad[row + 1][col + 1].grad;//(*imageRaw)[row + 1][col + 1];
                        g3 = grad[row - 1][col - 1].grad;//(*imageRaw)[row - 1][col - 1];

                        g2 = grad[row + 1][col].grad;//(*imageRaw)[row + 1][col];
                        g4 = grad[row - 1][col].grad;//(*imageRaw)[row - 1][col];
                    }else if(multiply < 0)
                    {
                        // < 0 向右.向上.. or 向左.向下
                        g1 = grad[row + 1][col - 1].grad;//(*imageRaw)[row + 1][col - 1];
                        g3 = grad[row - 1][col + 1].grad;//(*imageRaw)[row - 1][col + 1];

                        g2 = grad[row + 1][col].grad;//(*imageRaw)[row + 1][col];
                        g4 = grad[row - 1][col].grad;//(*imageRaw)[row - 1][col];
                    }

                }else if(weight < 1){
                    //weight < 1

                    //坐标轴 . 向下 向右为正.
                    //靠近Y轴 即水平轴

                    if(multiply > 0)
                    {
                        // 向右,向下.. 右下 右下点.右 左上点.左
                        g1 = grad[row + 1][col + 1].grad;//(*imageRaw)[row + 1][col + 1];
                        g3 = grad[row - 1][col - 1].grad;//(*imageRaw)[row - 1][col - 1];


                        g2 = grad[row][col + 1].grad;//(*imageRaw)[row + 1][col];
                        g4 = grad[row][col - 1].grad;//(*imageRaw)[row - 1][col];

                    }else if(multiply < 0)
                    {
                        // < 0 向右.向上.. or 向左.向下
                        g1 = grad[row + 1][col - 1].grad;//(*imageRaw)[row + 1][col + 1];
                        g3 = grad[row - 1][col + 1].grad;//(*imageRaw)[row - 1][col - 1];

                        g2 = grad[row][col + 1].grad;//(*imageRaw)[row + 1][col];
                        g4 = grad[row][col - 1].grad;//(*imageRaw)[row - 1][col];
                    }


                }else{
                    //weight = 1
                    //相等的情况.那么相邻的正好落在了斜对角上..

                    if(multiply > 0)
                    {
                        g1 = g2 = grad[row + 1][col + 1].grad;
                        g3 = g4 = grad[row - 1][col - 1].grad;
                    }else{//<0的情况..
                        g1 = g2 = grad[row + 1][col - 1].grad;
                        g3 = g4 = grad[row - 1][col + 1].grad;
                    }

                }



            }//end if..grad

            //Jugde
            //weight X/Y

            //计算出临时点..
            dTemp1 = g1 * weight + g2 * (1 - weight);
            dTemp2 = g3 * weight + g4 * (1 - weight);

            //点的类型判断.. 当前点 vs 两个临时点..

            //点..保留..
            if(grad[row][col].grad > dTemp1 && grad[row][col].grad > dTemp2 && grad[row][col].grad > th)
            {
                //qDebug()<<QString("(%1,%2)%3 %4 %5").arg(row).arg(col).arg(grad[row][col].grad).arg(dTemp1).arg(dTemp2);
                CPPCODE(display->DrawPoint(col,row));
                //break;
            }

        }
    }

    CPPCODE(qDebug()<<"THTHTH"<<th);


}
#endif

typedef enum{
    NormalRoad,
    StartLine,
    Obstacle,
    SmallRing,
    HugeRing,
    AutoRing,
    CrossRoad,
}RoadType_e;

//一些设置..
typedef struct{
    uint8 endRow;
    uint8 startRow;
}ImageInfoTypeDef;

typedef struct{
    uint8 endRow;
    uint8 startRow;
    float pitchAngle;//图像俯仰角

    //反光处理相关

    //斑马线相关
    uint8 zebraRow;//找到的斑马线的行数
    //起跑线相关

    //环岛相关..

    //断路相关

    //障碍相关

}ImageStatusTypeDef;


typedef struct
{
    //从近处到远处的顺序 startRow > endRow
    int16_t startRow;
    int16_t endRow;
}BreakLineItemTypeDef;
typedef struct
{
    uint8 size;//大小
    BreakLineItemTypeDef item[5];
}BreakLineTypeDef;

typedef enum :uint8
{
    NoBorder,
    TempBorder,
    ContinueFind,
    WeakBorder,
    StrongBorder,
    RealBorder,
}JumpPoint_e;
typedef struct _imgproc_side
{
    JumpPoint_e borderType[IMG_ROW];
    int16_t border[IMG_ROW];
    int16_t noneCnt;

    //拓展
    BreakLineTypeDef breakLine;
}IMGPROC_SIDE;

typedef struct _imgproc_struct
{
    int16_t endRow;

    IMGPROC_SIDE left;
    IMGPROC_SIDE right;
    int16_t middleLine[IMG_ROW];
    int16_t pathWidth[IMG_ROW];
    int16_t whiteCnt;//全白行cnt
}IMGPROC_STRUCT, *IMGPROC_STRUCT_PTR;

typedef struct
{
    LINE row;
    LINE col;
}PosTypeDef;


ImageStatusTypeDef imageStatus;
ImageInfoTypeDef imageInfo;
IMGPROC_STRUCT imgProcData;

PointGradTypeDef grads[IMG_ROW][IMG_COL];



void ImgProc::GetOnePointSobel(BYTE (*imageRaw)[120][188],PointGradTypeDef* g,LINE row,LINE col)
{
    //0为标志位 为不存在..
    if(grads[row][col].grad != 0)
    {

    }else{

        //BYTE (*imageRaw)[120][188] = (uchar (*)[120][188])imgPtr;

        grads[row][col].gradY = (*imageRaw)[row - 1][col+1] + ( (*imageRaw)[row][col+1] << 1 ) + (*imageRaw)[row + 1][col + 1]
                - (*imageRaw)[row - 1][col-1] - ( (*imageRaw)[row][col-1] << 1 ) - (*imageRaw)[row + 1][col - 1];

        grads[row][col].gradX = (*imageRaw)[row + 1][col - 1] + ( (*imageRaw)[row + 1][col] << 1 ) + (*imageRaw)[row + 1][col + 1]
                -(*imageRaw)[row - 1][col-1] - ( (*imageRaw)[row - 1][col] << 1 ) - (*imageRaw)[row - 1][col + 1];

        grads[row][col].grad = (int)(( Abs(grads[row][col].gradX) + Abs(grads[row][col].gradY) )*1.0f / 2 + 0.5f) + 1;


        //防止为0的情况
        if(grads[row][col].gradX == 0)
            grads[row][col].gradYX = grads[row][col].gradY<<10;//*1024
        else
            grads[row][col].gradYX = grads[row][col].gradY*1.0f / grads[row][col].gradX;
    }

    g = &grads[row][col];
}

//对某点进行极大值抑制判断..
static PointGradTypeDef g1,g2,g3,g4;
static float dTemp1,dTemp2,dTemp,weight;
static int multiply;
uint8 ImgProc::GetOnePointNMS(BYTE (*imageRaw)[120][188], LINE row, LINE col)
{

    PointGradTypeDef g;
    //得到当前的sobel
    GetOnePointSobel(imageRaw,&g,row,col);


    dTemp1 = dTemp2 = dTemp = 0;
    //g1 = g2 = g3 = g4 = g;
    weight = 0;


    if(g.gradX == 0)
    {
        //梯度为0的情况..
        //X(竖直)方向梯度为0.. 向着左右
        GetOnePointSobel(imageRaw,&g1,row,col - 1);
        GetOnePointSobel(imageRaw,&g3,row,col + 1);
        g1 = g2;//(*imageRaw)[row][col - 1];
        g3 = g4;//(*imageRaw)[row][col + 1];
    }else if(g.gradY == 0)
    {
        //Y(水平)方向梯度为0.. 向着上下
        GetOnePointSobel(imageRaw,&g1,row + 1,col);
        GetOnePointSobel(imageRaw,&g3,row - 1,col);
        g1 = g2;//(*imageRaw)[row + 1][col];
        g3 = g4;//(*imageRaw)[row - 1][col];
    }
    else
    {
        //梯度不为0的情况

        //纵轴 x方向.. 而且是上面的减去下面的
        //横轴 y方向.. 是右边的减去左边的..
        weight = Absf(g.gradYX);//这里已经排除了等于0的情况.. // weight =
        multiply = g.gradX * g.gradY;

        if(weight < 1)
        {

            //靠近X轴 X轴是纵轴 ..那么根据正负选择..方向.. 方向..
            //以下为靠近X轴逻辑 纵轴 . row +- 1


            //GX > GY靠近竖直轴
            if(multiply > 0)
            {
                // 向右,向下.. 右下 右下点.右 左上点.左
                GetOnePointSobel(imageRaw,&g1,row + 1,col + 1);
                GetOnePointSobel(imageRaw,&g3,row - 1,col - 1);
                //g1 = &grads[row + 1][col + 1];//(*imageRaw)[row + 1][col + 1];
                //g3 = &grads[row - 1][col - 1];//(*imageRaw)[row - 1][col - 1];
                GetOnePointSobel(imageRaw,&g2,row + 1,col);
                GetOnePointSobel(imageRaw,&g4,row - 1,col);
                //g2 = &grads[row + 1][col];//(*imageRaw)[row + 1][col];
                //g4 = &grads[row - 1][col];//(*imageRaw)[row - 1][col];
            }else if(multiply < 0)
            {
                // < 0 向右.向上.. or 向左.向下
                GetOnePointSobel(imageRaw,&g1,row + 1,col - 1);
                GetOnePointSobel(imageRaw,&g3,row - 1,col + 1);
                //g1 = &grads[row + 1][col - 1];//(*imageRaw)[row + 1][col - 1];
                //g3 = &grads[row - 1][col + 1];//(*imageRaw)[row - 1][col + 1];
                GetOnePointSobel(imageRaw,&g2,row + 1,col);
                GetOnePointSobel(imageRaw,&g4,row - 1,col);
                //g2 = &grads[row + 1][col];//(*imageRaw)[row + 1][col];
                //g4 = &grads[row - 1][col];//(*imageRaw)[row - 1][col];
            }

        }else if(weight > 1){
            weight = 1 / weight; // < 1

            //坐标轴 . 向下 向右为正.
            //靠近Y轴 即水平轴

            if(multiply > 0)
            {
                // 向右,向下.. 右下 右下点.右 左上点.左
                GetOnePointSobel(imageRaw,&g1,row + 1,col + 1);
                GetOnePointSobel(imageRaw,&g3,row - 1,col - 1);
                //g1 = &grads[row + 1][col + 1];//(*imageRaw)[row + 1][col + 1];
                //g3 = &grads[row - 1][col - 1];//(*imageRaw)[row - 1][col - 1];

                GetOnePointSobel(imageRaw,&g2,row + 1,col);
                GetOnePointSobel(imageRaw,&g4,row - 1,col);
                //g2 = &grads[row][col + 1];//(*imageRaw)[row + 1][col];
                //g4 = &grads[row][col - 1];//(*imageRaw)[row - 1][col];

            }else if(multiply < 0)
            {
                // < 0 向右.向上.. or 向左.向下
                GetOnePointSobel(imageRaw,&g1,row + 1,col - 1);
                GetOnePointSobel(imageRaw,&g3,row - 1,col + 1);
                //g1 = &grads[row + 1][col - 1];//(*imageRaw)[row + 1][col + 1];
                //g3 = &grads[row - 1][col + 1];//(*imageRaw)[row - 1][col - 1];
                GetOnePointSobel(imageRaw,&g2,row + 1,col);
                GetOnePointSobel(imageRaw,&g4,row - 1,col);
                //g2 = &grads[row][col + 1];//(*imageRaw)[row + 1][col];
                //g4 = &grads[row][col - 1];//(*imageRaw)[row - 1][col];
            }


        }else{
            //weight = 1
            //相等的情况.那么相邻的正好落在了斜对角上..

            if(multiply > 0)
            {
                GetOnePointSobel(imageRaw,&g1,row + 1,col + 1);
                g1 = g2;
                GetOnePointSobel(imageRaw,&g3,row - 1,col - 1);
                g3 = g4;
            }else{//<0的情况..
                GetOnePointSobel(imageRaw,&g1,row + 1,col - 1);
                g1 = g2;
                GetOnePointSobel(imageRaw,&g3,row - 1,col + 1);
                g3 = g4;
            }

        }



    }//end if..grad

    //Jugde
    //weight X/Y

    //计算出临时点..
    dTemp1 = g1.grad * weight + g2.grad * (1 - weight);
    dTemp2 = g3.grad * weight + g4.grad * (1 - weight);

    //点的类型判断.. 当前点 vs 两个临时点..

    //点..保留..
    if(g.grad > dTemp1 && g.grad > dTemp2 && g.grad)
    {
        //在这里只有极大值抑制.

        //qDebug()<<QString("(%1,%2)%3 %4 %5").arg(row).arg(col).arg(grad[row][col].grad).arg(dTemp1).arg(dTemp2);
        //CPPCODE(display->DrawPoint(col,row));
        //break;
        return 'Y';
    }else{
        return 'N';
    }

}

typedef struct{
    LINE row;//发生的行数..
    LINE limitRow;//处理到哪里就失效了.
    char dir;//'L' / 'R' / Default 0
}CrossRoadFlagTypeDef;

//阈值分割
#define AREA_THRESHOLD_SIZE 4
#define THRESHOLD_MAX 90
#define THRESHOLD_MIN 55
typedef struct{
    LINE startRow;
    struct{
        LINE start;
        LINE end;
    }range;
    BYTE threshold;
}AreaThresholdTypeDef;

//static BYTE thresholdArray[20] = {0};
static AreaThresholdTypeDef areaThresholdRules[AREA_THRESHOLD_SIZE + 1] = {
    { IMG_BOTTOM , { IMG_BOTTOM - 10 , IMG_BOTTOM - 26 } , 0 },
    { IMG_BOTTOM - 20 , { IMG_BOTTOM - 25 , IMG_BOTTOM - 45 }, 0 },
    { IMG_BOTTOM - 50 , { IMG_BOTTOM - 40 , IMG_BOTTOM - 70 }, 0 },
    { IMG_BOTTOM - 70 , { IMG_BOTTOM - 65 , IMG_BOTTOM - 80 }, 0 },
    { IMG_TOP , { 0 , 0 }, 0 },
};
//最好的办法是初始化的时候建立映射表..
BYTE ImgProc::GetAreaThreshold(BYTE (*imageRaw)[120][188],LINE row,LINE col)
{
    //白色 高阈值 黑色 低阈值
    //划分区域. end 34 9...
    /////////////////////////////////////////
#if 1
    //动态方法 不建立映射表
    for(int i = 0;i < AREA_THRESHOLD_SIZE;++i)
    {
        //确定当前的分割范围..
        if(row <= areaThresholdRules[i].startRow && row > areaThresholdRules[i + 1].startRow)
        {
            if(areaThresholdRules[i].threshold == 0)
            {
                areaThresholdRules[i].threshold = FastOSTU((*imageRaw)[areaThresholdRules[i].range.end],IMG_COL,areaThresholdRules[i].range.start - areaThresholdRules[i].range.end);
                if(areaThresholdRules[i].threshold > THRESHOLD_MAX)
                {
                    //这里的限制值应该根据前面正确的阈值动态选择吧..手动设置的呀应该只是最极端情况下的限制值?
                    areaThresholdRules[i].threshold = THRESHOLD_MAX;
                    //阈值限制..
                    CPPCODE(qDebug()<<QString("%1->%2 TH:%3 -> <<Threshold Limit>>").arg(areaThresholdRules[i].range.start).arg(areaThresholdRules[i].range.end).arg(areaThresholdRules[i].threshold));
                }else if(areaThresholdRules[i].threshold < THRESHOLD_MIN)
                {
                    //这里的限制值应该根据前面正确的阈值动态选择吧..手动设置的呀应该只是最极端情况下的限制值?
                    areaThresholdRules[i].threshold = THRESHOLD_MIN;
                    //阈值限制..
                    CPPCODE(qDebug()<<QString("%1->%2 TH:%3 -> <<Threshold Limit>>").arg(areaThresholdRules[i].range.start).arg(areaThresholdRules[i].range.end).arg(areaThresholdRules[i].threshold));
                }else{
                    CPPCODE(qDebug()<<QString("%1->%2 TH:%3").arg(areaThresholdRules[i].range.start).arg(areaThresholdRules[i].range.end).arg(areaThresholdRules[i].threshold));
                }
            }
            return areaThresholdRules[i].threshold;
        }
    }
    return THRESHOLD_MIN;
#endif
}

BYTE areaThresholdMap[IMG_ROW] = {THRESHOLD_MAX};

void ImgProc::CreateAreaThresholdMap(BYTE (*imageRaw)[120][188])
{
    //清空..
    for(int i = 0;i < AREA_THRESHOLD_SIZE;++i)
    {
        areaThresholdRules[i].threshold = 0;
    }
    //这里是上位机方法..方便更改代码.用到小车中需要换代码..
    for(LINE row = IMG_BOTTOM; row > IMG_TOP; --row)
    {
        areaThresholdMap[row] = GetAreaThreshold(imageRaw,row,0);
    }
}

#if 1
void ImgProc::ProcessSimpleCannyV2(BYTE* imgPtr,LINE startRow,LINE endRow,LINE startCol,LINE endCol)
{
    IMGPROC_STRUCT_PTR imgProcDataPtr = &imgProcData;

    //初始化参数..
    memset(grads,0,sizeof(grads));
    memset(&imgProcData,0,sizeof(imgProcData));

    imageStatus.endRow = endRow;

    //计算近处全局灰度 隔行 隔列1 0 1 0 1 0 1
    BYTE (*imageRaw)[120][188] = (uchar (*)[120][188])imgPtr;
    BYTE* rowPtr;

    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(135,206,250,255), 1, Qt::SolidLine)));
    CPPCODE(display->H.value("H").painter->drawLine(0,currentSection,IMG_RIGHT,currentSection));

    //CPPCODE painter颜色选择
    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(135,206,250,100), 1, Qt::SolidLine)));
    CPPCODE(display->H.value("H").painter->drawLine(0,endRow,IMG_RIGHT,endRow));

    LINE row = startRow;
    LINE col = IMG_COL/2 - 1;

    LINE searchStartCol = IMG_COL/2 - 1;

    //建立Map
    areaThresholdRules[4].startRow = endRow - 1;
    CreateAreaThresholdMap(imageRaw);
    imgProcDataPtr->endRow = 0;

    //大津法判断起始边界 减少运算量
    //我们有理由相信近处的边界误差较小
    //这里从倒数25行往下的..往下20行
    BYTE th;//阈值
//FastOSTU((*imageRaw)[IMG_BOTTOM - 25],IMG_COL,20)
    //最基本的算法 还需要改进很多东西才能正式应用到小车系统中去。
    LINE rowTemp;
    for(;row > startRow - 8;--row)
    {
        th = areaThresholdMap[row];

            rowPtr = (*imageRaw)[row];
            //从某一列开始
            for(col = searchStartCol; col <= IMG_RIGHT; col++)
            {
                //右边
    #if 1
                if(rowPtr[col] < th)
                {
                    CPPCODE(display->DrawPoint(col,row));
                    //qDebug()<<row<<col;
                    if(imgProcDataPtr->right.borderType[row] == NoBorder)
                    {
                        imgProcDataPtr->right.borderType[row] = TempBorder;
                        imgProcDataPtr->right.border[row] = col;
                    }
                }
    #endif
            }

            if(imgProcDataPtr->right.borderType[row] == NoBorder)
            {
                imgProcDataPtr->right.border[row] = IMG_RIGHT;
            }

            for(col = searchStartCol; col >= IMG_LEFT; col--)
            {
    #if 1
                //右边
                if(rowPtr[col] < th)
                {
                    CPPCODE(display->DrawPoint(col,row));
                    //qDebug()<<row<<col;
                    if(imgProcDataPtr->left.borderType[row] == NoBorder)
                    {
                        imgProcDataPtr->left.borderType[row] = TempBorder;
                        imgProcDataPtr->left.border[row] = col;
                    }
                }
    #endif
            }
            if(imgProcDataPtr->left.borderType[row] == NoBorder)
            {
                imgProcDataPtr->left.border[row] = IMG_LEFT;
            }
            searchStartCol = imgProcDataPtr->middleLine[row] = (imgProcDataPtr->left.border[row] + imgProcDataPtr->right.border[row])/2;

    }

    //这么在这里就找到了 startRow ~ startRow - 20 范围内的边线(使用的大津法)
    //得到的边线在 borderLeft borderRight 中.

    //如果置信现在搜出来的边线.那么根据现在的边线跟踪搜..?
    //前五个作为比较用..

    PointGradTypeDef g;

    PosTypeDef pLTemp,pRTemp;
    LINE low,high;

    //初始化十字路处理函数..[CrossRoad:两边都丢线的情况]
    CrossRoadFlagTypeDef crossRoadFlag;
    char crossRoadClearFlag = 0;//这里只是一个flag
    crossRoadFlag.dir = 0;
    crossRoadFlag.row = 0;
    crossRoadFlag.limitRow = 0;

    CrossRoadFlagTypeDef crossroadLeftFlag,crossRoadRightFlag;
    crossroadLeftFlag.dir = crossRoadRightFlag.dir = 0;
    crossroadLeftFlag.row = crossroadLeftFlag.limitRow = crossRoadRightFlag.row = crossRoadRightFlag.limitRow = 0;

#define CROSSROAD_FIX 0

    for(row = startRow - 3; row > imageStatus.endRow; --row)
    {
        th = areaThresholdMap[row];
        //右边..
        pLTemp.row = pRTemp.row = row;
        high = LimitH(imgProcDataPtr->right.border[row + 1] + 10,IMG_RIGHT - 1);
        low = LimitL(imgProcDataPtr->right.border[row + 1] - 10,1);

        if(row == 42)
            imgProcDataPtr->endRow = 0;

        for(pRTemp.col = col = high; col > low; --col)
        {
            //搜索当前点的周围情况..
            if( 'Y' == GetOnePointNMS(imageRaw,row,col))
            {
                if(grads[row][col].grad > th * 2 && ((*imageRaw)[row][col - 1] > th * 0.8 || (*imageRaw)[row + 1][col] > th * 0.8) && grads[row][col].gradY < 0)
                {
                    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgba(255,255,255,100)))));
                    CPPCODE(display->DrawPoint(col,row));
                    imgProcDataPtr->right.border[row] = col;
                    imgProcDataPtr->right.borderType[row] = RealBorder;
                    break;
                }else if(grads[row][col].grad > th * 1 )//&& ((*imageRaw)[row][col - 1] > th|| (*imageRaw)[row + 1][col] >  th ) && (grads[row][col].gradY < 0))
                {
                    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgba(0,255,255,100)))));
                    CPPCODE(display->DrawPoint(col,row));
                    if(imgProcDataPtr->right.borderType[row] == WeakBorder)
                    {
                        //断开了.且前面的连通..
                        if(pRTemp.col - col > 1)
                        {
                            //break;
                        }
                    }else{
                        imgProcDataPtr->right.borderType[row] = WeakBorder;
                    }
                    pRTemp.col = col;
                }
            }


        }
        //弱边沿.
        if(imgProcDataPtr->right.borderType[row] == WeakBorder)
        {
            CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgba(255,0,255,100)))));

            imgProcDataPtr->right.border[row] = pRTemp.col;

            if(((*imageRaw)[row][pRTemp.col - 1]) > th)
            {
                CPPCODE(display->DrawPoint(pRTemp.col,row));
            }else{
                imgProcDataPtr->right.borderType[row] = ContinueFind;
            }

        }else if(imgProcDataPtr->right.borderType[row] == NoBorder)
        {
            //判断..
            if(((*imageRaw)[ row ][ (low + high)/2 ]) < th)//黑
            {
                imgProcDataPtr->right.border[row] = low;
                imgProcDataPtr->right.borderType[row] = ContinueFind;
            }else{
                imgProcDataPtr->right.border[row] = (low + high) / 2;
                //imgProcDataPtr->right.borderType[row] = WideBorder;
            }
        }else if(imgProcDataPtr->right.borderType[row] == RealBorder)
        {
            if(((*imageRaw)[row][imgProcDataPtr->right.border[row] - 1]) <= th && ((*imageRaw)[row][imgProcDataPtr->right.border[row] - 2]) <= th && ((*imageRaw)[row][imgProcDataPtr->right.border[row] - 3]) <= th)
            {
                imgProcDataPtr->right.borderType[row] = ContinueFind;
            }
        }




        if(imgProcDataPtr->right.border[row] == 0)
            imgProcDataPtr->right.border[row] = imgProcDataPtr->right.border[row + 1];


        high = LimitH(imgProcDataPtr->left.border[row + 1] + 10,IMG_RIGHT - 1);
        low = LimitL(imgProcDataPtr->left.border[row + 1] - 10,1);
        for(pLTemp.col = col = low; col < high; ++col)
        {
            //搜索当前点的周围情况..
\
            if( 'Y' == GetOnePointNMS(imageRaw,row,col))
            {
                //常规方法是运用非极大值抑制后采用双阈值..再采用连通域连通.

                //双阈值一般H = 2*L
                if(grads[row][col].grad > th * 2 && ((*imageRaw)[row][col + 1] > th * 1 || (*imageRaw)[row + 1][col] > th * 1) && grads[row][col].gradY > 0)
                {
                    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgba(255,255,255,100)))));
                    CPPCODE(display->DrawPoint(col,row));
                    imgProcDataPtr->left.border[row] = col;
                    imgProcDataPtr->left.borderType[row] = RealBorder;
                    break;
                }else if(grads[row][col].grad > th * 1)// && ((*imageRaw)[row][col + 1] > th|| (*imageRaw)[row + 1][col] >  th ) && (grads[row][col].gradY > 0))
                {
                    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgba(0,255,255,100)))));
                    CPPCODE(display->DrawPoint(col,row));
                    if(imgProcDataPtr->left.borderType[row] == WeakBorder)
                    {
                        //断开了.且前面的连通..
                        if(pLTemp.col - col < -1)
                        {
                            //break;
                        }
                    }else{
                        imgProcDataPtr->left.borderType[row] = WeakBorder;
                    }
                    pLTemp.col = col;
                }

            }
            imgProcDataPtr->endRow = 0;
        }
        if(imgProcDataPtr->left.borderType[row] == WeakBorder)
        {
            CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgba(255,0,255,100)))));

            imgProcDataPtr->left.border[row] = pLTemp.col;

            if(((*imageRaw)[row][pLTemp.col + 1]) > th)
            {
                CPPCODE(display->DrawPoint(pLTemp.col,row));
            }else{
                imgProcDataPtr->left.borderType[row] = ContinueFind;
            }

        }else if(imgProcDataPtr->left.borderType[row] == NoBorder)
        {
            //判断..
            if(((*imageRaw)[ row ][ (low + high)/2 ]) <= th)//黑色 小于阈值.. 这个值还是得重新选取啊.不然就翻车了..我擦..
            {
                imgProcDataPtr->left.border[row] = high;
                imgProcDataPtr->left.borderType[row] = ContinueFind;
            }else{
                imgProcDataPtr->left.border[row] = (low + high) / 2;
            }
        }else if(imgProcDataPtr->left.borderType[row] == RealBorder)
        {
            //..这里是什么?
            if(((*imageRaw)[row][imgProcDataPtr->left.border[row] + 1]) <= th && ((*imageRaw)[row][imgProcDataPtr->left.border[row] + 2]) <= th && ((*imageRaw)[row][imgProcDataPtr->left.border[row] + 3]) <= th)
            {
                imgProcDataPtr->left.borderType[row] = ContinueFind;
            }
        }

        if(imgProcDataPtr->left.border[row] == 0)
            imgProcDataPtr->left.border[row] = imgProcDataPtr->left.border[row + 1];

#if 1

        if(ContinueFind == imgProcDataPtr->left.borderType[row] || ContinueFind == imgProcDataPtr->right.borderType[row])
        {

            if(imgProcDataPtr->left.borderType[row] == ContinueFind)
            {
                //Left  ContinueFind

                //从新开始搜索
                for(LINE Xsite = imgProcDataPtr->left.border[row] + 1;Xsite <= imgProcDataPtr->right.border[row] - 1;Xsite++)
                {
                    if( (*imageRaw)[row][Xsite] <= th && (*imageRaw)[row][Xsite + 1] > th )
                    {
                        imgProcDataPtr->left.border[row] = Xsite;
                        imgProcDataPtr->left.borderType[row] = RealBorder;
                        CPPCODE(display->DrawPoint(Xsite,row));

                        break;
                    }else if((*imageRaw)[row][Xsite + 1] > th)
                    {
                        break;
                    }else if(Xsite == imgProcDataPtr->right.border[row] - 1)
                    {
                        imgProcDataPtr->left.border[row] = Xsite;
                        imgProcDataPtr->left.borderType[row] = RealBorder;
                        CPPCODE(display->DrawPoint(Xsite,row));

                        break;
                    }


                }

            }

            if(imgProcDataPtr->right.border[row] - imgProcDataPtr->left.border[row] <= 10)
            {
                imageStatus.endRow = row + 1;
            }

            if(imgProcDataPtr->right.borderType[row] == ContinueFind){
                //Right ContinueFind
                CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(255,140,0,200))));
                for(LINE Xsite = imgProcDataPtr->right.border[row] - 1;Xsite >= imgProcDataPtr->left.border[row] + 1;Xsite--)
                {
                    if( (*imageRaw)[row][Xsite] <= th && (*imageRaw)[row][Xsite - 1] > th)
                    {
                        imgProcDataPtr->right.border[row] = Xsite;
                        CPPCODE(display->DrawPoint(Xsite,row));
                        imgProcDataPtr->right.borderType[row] = RealBorder;
                    }else if((*imageRaw)[row][Xsite - 1] > th)
                    {
                        CPPCODE(display->DrawPoint(Xsite,row));
                        break;
                    }else if(Xsite == imgProcDataPtr->left.border[row] + 1)
                    {
                        imgProcDataPtr->right.border[row] = Xsite;
                        imgProcDataPtr->right.borderType[row] = RealBorder;
                        CPPCODE(display->DrawPoint(Xsite,row));
                        break;
                    }
                }


            }

        }


#endif



#if 1        //处理折点..
        if(row <= startRow - 8)
        {


            //Clear CrossRoad Buf
            if(crossroadLeftFlag.limitRow > row)
                crossroadLeftFlag.dir = 0;
            if(crossRoadRightFlag.limitRow > row)
                crossRoadRightFlag.dir = 0;

            if(imgProcDataPtr->left.borderType[row] != NoBorder)
            {
                if(row == 50)
                    imgProcDataPtr->endRow = 0;//临时断点

                if(imgProcDataPtr->left.border[row] - imgProcDataPtr->left.border[row + 4] <= -8\
                        && imgProcDataPtr->left.border[row + 4] - imgProcDataPtr->left.border[row + 8] >= 3)
                {
                    //这个循环下如果没找到是不是可以加一个flag呢..如果另外一边找到了这种情况再回头来清flag
                    //为什么需要这么搞呢?感觉是怕拐弯的时候也给补线了..
                    crossRoadClearFlag = 'F';

                    if(crossroadLeftFlag.dir == 0)
                    {
                        //
                        crossroadLeftFlag.dir = 'L';
                        crossroadLeftFlag.row = row;
                        crossroadLeftFlag.limitRow = row - 5;
                    }

                    for (int16_t yTemp = row; yTemp <= row + 5; ++yTemp)
                    {
                        if (imgProcDataPtr->right.borderType[row] != RealBorder)
                        {
                            CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(Qt::blue))));
                            crossRoadClearFlag = 'T';
                            for (yTemp = (row + 4); yTemp >= row; --yTemp)
                            {
                                //消除左边的折点
                                imgProcDataPtr->left.border[yTemp] = 2 * imgProcDataPtr->left.border[yTemp + 1] - imgProcDataPtr->left.border[yTemp + 2];
                                //折线上点的状态改变
                                imgProcDataPtr->left.borderType[yTemp] = NoBorder;
                                CPPCODE(display->DrawPoint(imgProcDataPtr->left.border[yTemp],yTemp));
                            }
                            //改变状态
                            imgProcDataPtr->whiteCnt += 4;

                            //判断是否有..然后执行
                            if(crossRoadFlag.dir == 'R' && row > crossRoadFlag.limitRow)
                            {
                                for(yTemp = crossRoadFlag.row + 4;yTemp >= row;--yTemp)
                                {
                                    //消除左边的折点
                                    imgProcDataPtr->right.border[yTemp] = 2 * imgProcDataPtr->right.border[yTemp + 1] - imgProcDataPtr->right.border[yTemp + 2];
                                    //折线上点的状态改变
                                    imgProcDataPtr->right.borderType[yTemp] = NoBorder;
                                    CPPCODE(display->DrawPoint(imgProcDataPtr->right.border[yTemp],yTemp));
                                }
                            }


                            break;//break break
                        }
                    }

                    //在所有判断完之后加上当前的信息..
#if(CROSSROAD_FIX == 1)
                    crossRoadFlag.dir = 'L';
#endif
                    if(crossRoadClearFlag == 'F')
                    {
                        crossRoadFlag.row = row;
                        crossRoadFlag.limitRow = row - 10;
                    }else{
                        crossRoadFlag.row = crossRoadFlag.limitRow = row;
                    }


                }



            }


            if(imgProcDataPtr->right.borderType[row] != NoBorder)
            {
                if(imgProcDataPtr->right.border[row] - imgProcDataPtr->right.border[row + 4] >= 8\
                        && imgProcDataPtr->right.border[row + 4] - imgProcDataPtr->right.border[row + 8] <= 3)
                {
                    crossRoadClearFlag = 'F';

                    if(crossRoadRightFlag.dir == 0)
                    {
                        //
                        crossRoadRightFlag.dir = 'R';
                        crossRoadRightFlag.row = row;
                        crossRoadRightFlag.limitRow = row - 5;
                    }

                    for (int16_t yTemp = row; yTemp <= row + 5; ++yTemp)
                    {
                        if (imgProcDataPtr->left.borderType[row] != RealBorder)
                        {
                            CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(Qt::blue))));

                            for (yTemp = (row + 4); yTemp >= row; --yTemp)
                            {
                                //消除左边的折点
                                imgProcDataPtr->right.border[yTemp] = 2 * imgProcDataPtr->right.border[yTemp + 1] - imgProcDataPtr->right.border[yTemp + 2];
                                //折线上点的状态改变
                                imgProcDataPtr->right.borderType[yTemp] = NoBorder;
                                CPPCODE(display->DrawPoint(imgProcDataPtr->right.border[yTemp],yTemp));
                            }
                            //改变状态
                            imgProcDataPtr->whiteCnt += 4;

                            //判断是否有..然后执行
                            if(crossRoadFlag.dir == 'L' && row > crossRoadFlag.limitRow)
                            {
                                for(yTemp = crossRoadFlag.row + 4;yTemp >= row;--yTemp)
                                {
                                    //消除左边的折点
                                    imgProcDataPtr->left.border[yTemp] = 2 * imgProcDataPtr->left.border[yTemp + 1] - imgProcDataPtr->left.border[yTemp + 2];
                                    //折线上点的状态改变
                                    imgProcDataPtr->left.borderType[yTemp] = NoBorder;
                                    CPPCODE(display->DrawPoint(imgProcDataPtr->left.border[yTemp],yTemp));
                                }
                            }

                            break;
                        }
                    }

                    //在所有判断完之后加上当前的信息..
#if(CROSSROAD_FIX == 1)
                    crossRoadFlag.dir = 'R';
#endif
                    if(crossRoadClearFlag == 'F')
                    {
                        crossRoadFlag.row = row;
                        crossRoadFlag.limitRow = row - 10;
                    }else{
                        crossRoadFlag.row = crossRoadFlag.limitRow = row;
                    }

                }
            }

            //CrossRoadJudge
            if(crossroadLeftFlag.dir == 'L' && crossRoadRightFlag.dir == 'R')
            {
                CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(135,206,0,100), 1, Qt::SolidLine)));
                CPPCODE(display->H.value("H").painter->drawLine(0+30,row,IMG_RIGHT-30,row));
                //进入 crossRoadMode
                for (int16_t yTemp = (row + 4); yTemp >= row; --yTemp)
                {
                    //消除左边的折点
                    imgProcDataPtr->right.border[yTemp] = 2 * imgProcDataPtr->right.border[yTemp + 1] - imgProcDataPtr->right.border[yTemp + 2];
                    //折线上点的状态改变
                    imgProcDataPtr->right.borderType[yTemp] = NoBorder;
                    CPPCODE(display->DrawPoint(imgProcDataPtr->right.border[yTemp],yTemp));
                }
            }

        }
#endif




        searchStartCol = (imgProcDataPtr->left.border[row] + imgProcDataPtr->right.border[row])/2;

    }

    for(LINE Ysite = startRow;Ysite > imageStatus.endRow;Ysite--)
    {
        CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(240,128,128,200))));

        if(imgProcDataPtr->left.borderType[Ysite] == NoBorder)
        {
            CPPCODE(display->DrawPoint(IMG_LEFT,Ysite));
        }

        if(imgProcDataPtr->right.borderType[Ysite] == NoBorder)
        {
            CPPCODE(display->DrawPoint(IMG_RIGHT,Ysite));
        }
    }




    /////////////////////////Curve Fitter
    /////////////////////////Curve Fitter
    /////////////////////////Curve Fitter
    /////////////////////////Curve Fitter
    /////////////////////////Curve Fitter
    imgProcDataPtr->endRow = imageStatus.endRow;
    if(imageStatus.endRow <= 80)
    {
        for(row = imageStatus.endRow; row < imageStatus.endRow + 6; ++row)
        {



            if (imgProcDataPtr->left.borderType[row] == NoBorder && imgProcDataPtr->left.borderType[row + 1] == NoBorder
                && imgProcDataPtr->right.borderType[row] != NoBorder && imgProcDataPtr->right.borderType[row + 1] != NoBorder
                )
            {
                while (row >= imgProcDataPtr->endRow + 1)
                {

                    --row;

                    if (imgProcDataPtr->right.borderType[row] == NoBorder) break;
                    if (imgProcDataPtr->left.borderType[row] == RealBorder && (imgProcDataPtr->pathWidth[row] <= 20 || (row <= imgProcDataPtr->endRow + 5)))
                    {

                        imgProcDataPtr->left.border[row] = imgProcDataPtr->left.border[row + 2];
                        imgProcDataPtr->left.borderType[row] = NoBorder;
                        imgProcDataPtr->right.border[row] = 2 * imgProcDataPtr->right.border[row + 1] - imgProcDataPtr->right.border[row + 2];

                        Limit(imgProcDataPtr->left.border[row], IMG_LEFT, IMG_RIGHT);
                        Limit(imgProcDataPtr->right.border[row], IMG_LEFT, IMG_RIGHT);

                        imgProcDataPtr->pathWidth[row] = imgProcDataPtr->right.border[row] - imgProcDataPtr->left.border[row];
                        imgProcDataPtr->middleLine[row] = (imgProcDataPtr->left.border[row] + imgProcDataPtr->right.border[row]) / 2;

                        if (imgProcDataPtr->pathWidth[row] <= 0)
                        {
                            imgProcDataPtr->endRow = row + 1;
                            break;
                        }

                    }
                }

                break;

            }
            else if (imgProcDataPtr->right.borderType[row] == NoBorder && imgProcDataPtr->right.borderType[row + 1] == NoBorder
                && imgProcDataPtr->left.borderType[row] == RealBorder && imgProcDataPtr->left.borderType[row + 1] == RealBorder)
            {
                while (row >= imgProcDataPtr->endRow + 1)
                {

                    --row;

                    if (imgProcDataPtr->left.borderType[row] != RealBorder) break;
                    if (imgProcDataPtr->right.borderType[row] == RealBorder && (imgProcDataPtr->pathWidth[row] <= 20 || (row <= imgProcDataPtr->endRow + 5)))
                    {
                        imgProcDataPtr->right.border[row] = imgProcDataPtr->right.border[row + 2];
                        imgProcDataPtr->right.borderType[row] = NoBorder;
                        imgProcDataPtr->left.border[row] = 2 * imgProcDataPtr->left.border[row + 1] - imgProcDataPtr->left.border[row + 2];

                        Limit(imgProcDataPtr->left.border[row], IMG_LEFT, IMG_RIGHT);
                        Limit(imgProcDataPtr->right.border[row], IMG_LEFT, IMG_RIGHT);


                        imgProcDataPtr->pathWidth[row] = imgProcDataPtr->right.border[row] - imgProcDataPtr->left.border[row];
                        imgProcDataPtr->middleLine[row] = (imgProcDataPtr->left.border[row] + imgProcDataPtr->right.border[row]) / 2;

                        if (imgProcDataPtr->pathWidth[row] <= 0)
                        {
                            imgProcDataPtr->endRow = row + 1;
                            break;
                        }

                    }
                }

                break;
            }

        }
    }



    //尝试补线..补线程序..
    int       TFSite = startRow, FTSite = startRow,ytemp;
    float     DetR = 0, DetL = 0;
    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(255,255,255,200))));
    for(LINE Ysite = startRow - 5;Ysite >= imageStatus.endRow;Ysite--)
    {
        if(imgProcDataPtr->left.borderType[Ysite] == NoBorder)
        {
            if(imgProcDataPtr->left.border[Ysite + 1] >= IMG_RIGHT - 20)
            {
                imageStatus.endRow = Ysite + 1;
            }
            while(Ysite >= imageStatus.endRow + 2)
            {
                Ysite--;
                if(imgProcDataPtr->left.borderType[Ysite] == RealBorder && imgProcDataPtr->left.borderType[Ysite - 1] == RealBorder && imgProcDataPtr->left.borderType[Ysite - 2] == RealBorder)
                {
                    FTSite = Ysite - 2;
                    break;
                }
            }

            DetL = ((float)(imgProcDataPtr->left.border[FTSite] - imgProcDataPtr->left.border[TFSite]) / ((float)FTSite - TFSite));
            for(ytemp = TFSite;ytemp >= FTSite;ytemp--)
            {
                imgProcDataPtr->left.border[ytemp] = (LINE)(DetL * ((float)(ytemp - TFSite))) + imgProcDataPtr->left.border[TFSite];
                CPPCODE(display->DrawPoint(imgProcDataPtr->left.border[ytemp],ytemp));
            }

        }else{
            TFSite = Ysite + 2;
        }
    }

    TFSite = startRow, FTSite = startRow;

    for(LINE Ysite = startRow - 5;Ysite >= imageStatus.endRow;Ysite--)
    {
        if(imgProcDataPtr->right.borderType[Ysite] == NoBorder)
        {
            if(imgProcDataPtr->right.border[Ysite + 1] <= IMG_LEFT + 20)
            {
                imageStatus.endRow = Ysite + 1;
            }
            while(Ysite >= imageStatus.endRow + 2)
            {
                Ysite--;
                if(imgProcDataPtr->right.borderType[Ysite] == RealBorder && imgProcDataPtr->right.borderType[Ysite - 1] == RealBorder && imgProcDataPtr->right.borderType[Ysite - 2] == RealBorder)
                {
                    FTSite = Ysite - 2;
                    break;
                }
            }

            DetR = ((float)(imgProcDataPtr->right.border[FTSite] - imgProcDataPtr->right.border[TFSite]) / ((float)FTSite - TFSite));
            for(ytemp = TFSite;ytemp >= FTSite;ytemp--)
            {
                imgProcDataPtr->right.border[ytemp] = (LINE)(DetR * ((float)(ytemp - TFSite))) + imgProcDataPtr->right.border[TFSite];
                CPPCODE(display->DrawPoint(imgProcDataPtr->right.border[ytemp],ytemp));
            }

        }else{
            TFSite = Ysite + 2;
        }
    }



    CPPCODE(display->H.value("H").painter->setPen(QPen(QColor(qRgba(255,255,0,100)))));

    for(LINE Ysite = startRow;Ysite > imageStatus.endRow;Ysite--)
    {
        imgProcDataPtr->middleLine[Ysite] = (imgProcDataPtr->left.border[Ysite] + imgProcDataPtr->right.border[Ysite])/2;
        CPPCODE(display->DrawPoint(imgProcDataPtr->middleLine[Ysite],Ysite));
    }

}

void ImgProc::ProcessImage(BYTE *imgPtr, LINE startRow, LINE endRow, LINE startCol, LINE endCol)
{

}
#endif
