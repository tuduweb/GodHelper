#include "imgprocdisplay.h"
#include "Bin/header.h"

ImgProcDisplay::ImgProcDisplay(QObject *parent) : QObject(parent)
{

    //添加通道..
    AddDrawingChannel("H");
    AddDrawingChannel("2");


}

void ImgProcDisplay::PainterInit(void)
{
    //初始化绘制..
}

void ImgProcDisplay::PainterStart(void)
{
    //开始一副新图的绘制? 清空..
    QHashIterator<QString, PainterDef> i(H);
    while(i.hasNext()) {
        i.next();
        i.value().cover->fill(Qt::transparent);
        i.value().painter->begin(i.value().cover);
    }

    //unsigned int t = H.value("1").index;
    H.value("H").painter->setPen(QPen(Qt::yellow, 1, Qt::SolidLine));
    for(int i = 0;i < IMG_ROW;i++)
    {
        //H.value("H").painter->drawPoint(i,i);
    }

}

void ImgProcDisplay::PainterEnd(void)
{
    QHashIterator<QString, PainterDef> i(H);
    while(i.hasNext()) {
        i.next();
        i.value().painter->end();
    }
}



//绘制通道的添加...
bool ImgProcDisplay::AddDrawingChannel(const QString name)
{
    PainterDef def;
    def.cover = new QImage(IMG_COL,IMG_ROW,QImage::Format_ARGB32);
    //根据画布大小.或者显示大小进行设置..比如
    //比如道路图像数据80*60 可以把图像数据放大.再绘图(在自己封的函数里面进行坐标变换).这样得到的效果比较细腻.


    def.index = 0;//通道的层次关系.比较小的层次在前...
    def.painter = new QPainter;//绘图板
    //新增绘制通道 绘制通道..
    H.insert(name,def);
    return true;
}

void ImgProcDisplay::DrawPoint(const int x,const int y)
{
    H.value("H").painter->drawPoint(x,y);
}

void ImgProcDisplay::DrawPoints(const QPolygon &points)
{
    H.value("H").painter->drawPoints(points);
}
