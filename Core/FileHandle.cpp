#include "FileHandle.h"
#include <QQueue>
#include <QDate>
#include <QDir>

static QString str("%1/%2/%3");

static QQueue<QString> queue;

FileHandle* fileHandle;


FileHandle::FileHandle(QObject *parent) : QObject(parent)
{
    //初始化命名规则..
    //基本命名规则..

}


void FileHandle::SaveImage(QImage image)
{
    int timeT = QDateTime::currentDateTime().toMSecsSinceEpoch();//将当前时间转为时间戳
    //image.save("P:/s.bmp");
}

bool FileHandle::mkDir(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists())
    {
      return true;
    }
    else
    {
       bool ok = dir.mkpath(fullPath);//创建多级目录
       return ok;
    }
}
