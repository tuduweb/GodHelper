#include "FileHandle.h"
#include <QQueue>
static QString str("%1/%2/%3");

static QQueue<QString> queue;

FileHandle::FileHandle(QObject *parent) : QObject(parent)
{
    //初始化命名规则..
    //基本命名规则..

}
