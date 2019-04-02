#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>
#include <QtGui>
#include "ImgProc/ImgProcCore.h"

#include "Core/FileHandle.h"



typedef enum
{
    COMM_DATA_CAM_UNKNOWN = 0,//未知
    COMM_DATA_CAM_GRAYSCALE8,//相机 8位灰度
    COMM_DATA_CAM_GRAYSCALE1,//相机 1位灰度
    COMM_DATA_SYS_SPEED,//系统运行速率 (函数实际频率)
    COMM_DATA_CAR_RUNNING,//小车运行数据..
    COMM_DATA_CAR_SENSOR,//传感器数据
    COMM_DATA_CAR_PWM,//小车PWM数据
    COMM_DATA_UPPER_PID,//更新PID数据..双向设定..
    COMM_DATA_SETTING,//对车进行设置(各种开关)
    COMM_DATA_OTHER//根据表中的设定自行匹配
}COMM_DATATYPE_e;

//数据解析 兼容匿名上位机..
typedef enum
{
    COMM_STATUS_IDLE = (uint8_t)0,
    COMM_STATUS_HEAD = 0xAA,//0xAA
    COMM_STATUS_SADDR,//0x66 0x77 四轮,直立
    COMM_STATUS_DADDR = 0xBF,//0xBF bins UpperMonitor
    COMM_STATUS_TYPE,//Type enum typeDef
    COMM_STATUS_LEN,//Data Length
    COMM_STATUS_DATA,//Data main
    COMM_STATUS_CHECK,//CheckNum
    COMM_STATUS_END
}COMM_PARSE_STATUS_e;

typedef struct{
    COMM_PARSE_STATUS_e status;
    COMM_DATATYPE_e type;
    uint16_t packageSize;//数据长度..
    uint16_t dataSize;
    uint16_t toFillSize;//填充大小
    uchar* dataPtr;
}COMM_DATA_TypeDef;

extern COMM_DATA_TypeDef commData;



typedef struct ImgProcListNode{
    QThread *thread;
    ImgProcCore* core;
}imgProcListItemDef;

class DataCenter : public QObject
{
    Q_OBJECT
public:
    explicit DataCenter(QObject *parent = nullptr);

    void ConnectSon();

    QVector<QRgb> vcolorTable;

    quint32 sizeCount;
    uint lastTimeStamp;


    ImgProcCore* imgProcCore;
    QThread imgProcThread;//图像线程

    QThread fileHandleThread;//文件处理线程..

    ~DataCenter();


    void ProcessPackage(COMM_DATA_TypeDef* packagePtr);


protected:
    void timerEvent(QTimerEvent * timeEvent);

    QVector<imgProcListItemDef> imgProcCoreList;

    //图像处理线程相关..
    void OpenImgProcThread();//新增一条图像处理线程..
    void FinishImgProcThread();//关闭一条图像处理线程..
    void FinishAllImgProcThread();//关闭所有图像处理线程..


signals:
    void AddMsgSignal(uint type,QString* msg);

    void myTest(QString sKey,QByteArray byteData);
    void updateImage(QImage image);
    void updateImage2(QImage image);
    void ProcessImage(QImage image);
    void updateReceiveCount(uint16 receiveCount);
    void ProcessImageCharArray(uchar* image);

public slots:
    void DataInterface(QString sKey,QByteArray byteData);
    //void dataInterface(QImage picDataPtr);
    void DataInterface(QImage picData);
    //void dataInterface(QByteArray byteData);
    void ImgUrlInterface(QList<QUrl> imgUrlList);

    //增加数据源
    /// TCP不同Cilent属于不同的数据源.
    /// UDP不同的Sender属于不同的数据源
    /// 不同串口号属于不同的数据源
    /// SD卡数据属于特殊的数据源..启用特殊部件进行处理
    //void addDataSource(int type,int key);

};

#endif // DATACENTER_H
