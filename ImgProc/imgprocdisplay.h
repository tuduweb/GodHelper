#ifndef IMGPROCDISPLAY_H
#define IMGPROCDISPLAY_H

#include <QObject>
#include <QImage>
#include <QPainter>

typedef struct _cover_image{
    QImage *base;
    QImage *ring;
    QImage *extend;
    QImage *fitter;
}CoverImageTypeDef;

typedef struct _cover_painter
{
    QPainter base;
    QPainter ring;
    QPainter extend;
    QPainter fitter;
}CoverPainterTypeDef;

//定义一个画布层
typedef struct _bin_painter{
    unsigned int index;
    QImage *cover;
    QPainter *painter;
}PainterDef;

typedef struct _text_debugger{
    QString str;
}TextDebuggerDef;


class ImgProcDisplay : public QObject
{
    Q_OBJECT
public:
    QVector<PainterDef> C;
    QList<TextDebuggerDef> T;
    QHash<QString,PainterDef> H;
    explicit ImgProcDisplay(QObject *parent = nullptr);


    //Cover Image Draw
    void PainterInit(void);//Painter初始化..
    void PainterStart(void);
    void PainterEnd(void);

    bool AddDrawingChannel(QString name);//新增一个绘制通道..
    bool AddDrawingPen(QString name);//新增一种画笔.. 暂时放着 有时间再弄..

    void SetDrawChannel(QString name);//设置绘制区域
    void SetDrawPen(QString name);
    //在制定的绘制区域绘制点
    void DrawPoint(const int x,const int y);
    void DrawPoints(const QPolygon &points);

    //突出显示关键点..
    void HintPoint(const int x,const int y);
    void HintPoints(const QPolygon &points);

    //绘制直线..等

    //隐藏区域..给与透明度处理
    void HiddenFarArea(const int row);
    void HiddenNearArea(const int row);



    //DebugCenter..不应该放在这..因为debugger是外部的东西..可以不耦合在一起!.但是通道上必须有啊.
signals:

public slots:
};

#endif // IMGPROCDISPLAY_H
