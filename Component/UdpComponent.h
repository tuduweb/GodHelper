#ifndef UDPCOMPONENT_H
#define UDPCOMPONENT_H

#include <QObject>
#include <QSerialPort>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QWidget>


class UdpComponent: public QWidget
{
    Q_OBJECT

public:

    UdpComponent(QWidget *parent = 0);
    ~UdpComponent();

    void UdpSendData(QByteArray byteArray);

    QHostAddress currentClientAddress;

    //DataCenter *dataCenter;
    QUdpSocket  *udpSocket;//udp

    bool BindInterface(bool checked);

    //绑定信号到dataCenter
    bool BindDataCenter();



    void InitUI();

signals:
    void newDataSignal(QString sKey,QByteArray byteArray);
    void warning(QString str);
    void error(QString str);
    void newMassage(QString str);


public slots:
    //UDP相关
    //void BindInterface(bool checked);
    //void udpUnbind();

    //void DSSlot(bool flag);
private:
    quint16 currentRecPort;

    QString bindLocalIP;
    quint16 bindLocalPort;
private slots:
    void receiveUdpData();
    void udpStateChanged(QAbstractSocket::SocketState socketState);
    void udpErrorOccur(QAbstractSocket::SocketError socketError);

signals:
    void BindSuccess(QString key);

};

#endif // UDPCOMPONENT_H
