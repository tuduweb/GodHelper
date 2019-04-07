#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include <QObject>
#include <QImage>

//子线程处理文件

struct FILE_HANDLE_QUEUE
{
    uchar* source;//源地址
};



class FileHandle : public QObject
{
    Q_OBJECT
public:
    explicit FileHandle(QObject *parent = nullptr);

    void SaveOriginalImage();//储存原图..
    void SaveProcessedImage();//储存处理后的图像..
    void ReadSDCard();//读取sd卡..
    void SaveImage(QImage image);

    bool isAutoSave = false;
    QString saveDir = QString("P:/smartcar/monitor/2019/{name}");

    bool mkDir(QString fullPath);

signals:

public slots:
};


extern FileHandle* fileHandle;

#endif // FILEHANDLE_H
