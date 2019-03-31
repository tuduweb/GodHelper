#ifndef IMGSHOWCOMPONENT_H
#define IMGSHOWCOMPONENT_H

#include <QTWidgets>
#include "Core/OpenGLWidget.h"

#include "ImgProc/ImgProcCore.h"


class ImgShowComponent : public OpenGLWidget
{
    Q_OBJECT

public:
    explicit ImgShowComponent(QWidget *parent = 0);



    QLabel *imgLabel;
    QLabel *textLabel;

    ImgProcCore* imgProc;
    QThread imgProcThread;//图像线程

    ~ImgShowComponent();

public slots:
    void updateImgWidget(QPixmap packedPixmap);//更新窗口的槽
    void updateImgWidgetPtr(QPixmap* packedPixmap);//更新窗口的槽
    void updateImage(QImage imagePtr);
signals:
    void updateSurface();
    void dropSignal(QImage image);//拖入信号
    void dropSignals(QList<QUrl> imgUrlList);
    void SaveImages();

protected:
    void paintEvent(QPaintEvent *e);
    void dragEnterEvent(QDragEnterEvent *event);
    //void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mouseMoveEvent(QMouseEvent * event);

private:
    QPixmap *pixmap;//显示的图片



};
#endif // IMGSHOWCOMPNENT_H
