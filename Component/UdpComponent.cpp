#include "UdpComponent.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QtWidgets>
#include <QStringList>

#include "Bin/header.h"

//注意字母大小写,缩放等问题


#include <Core/datacenter.h>

//记录dataCenter的指针,且不可修改.只在初始化的时候赋值
extern DataCenter* dataCenterPtr;

//////////////////////////////////////////////
/// \brief UdpComponent::UdpComponent
/// \param parent
/// 这里需要实现的是接收平台 是类似于主机的东西 方便多个设备来通信
/// 所以需要实现从多个Udp接收
UdpComponent::UdpComponent(QWidget *parent)
{
    this->setParent(parent);//这句需要使用吗..需要查看一下c++的手册.有关于初始化部分

    //注意野指针,在这里直接定义局部指针 并且在内存中开辟了一些东西,有必要加上this来让父页面来管理这些东西..
    QVBoxLayout *mainLayout = new QVBoxLayout(this);//The QVBoxLayout class lines up widgets vertically.
    //mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    ///////////////////////页面布局//////////////////////////
    /// 本机IP地址 QLineEdit (readOnly)
    /// 绑定端口 QTextLabel + QLineEdit (or List) (Number Range Limitted)
    /// 数据源绑定 Class + List or Vector 似乎没办法绑定..只能在解析这里下功夫了.

    //创建文本行
    QLineEdit* localAddrLineEdit = new QLineEdit(this);
    //QListWidget* localAddrListWidget = new QListWidget(this);
    QComboBox *localHostAddrComboBox = new QComboBox(this);
    //localHostAddrComboBox->setEditable(true);

    /*
    QStringList localHostAddrStringList;
    localHostAddrStringList << "Biao" << "Biao Huang" << "Mac" << "MacBook" << "MacBook Pro" << "Mac Pro";
    QCompleter* completer = new QCompleter(localHostAddrStringList,this);
            completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
            //设置下拉提示popup的样式
           completer->popup()->setStyleSheet("    background-color: #294662;\
                                            color: #ffffff;\
                                            border: 1px solid #375C80;\
                                            border-radius: 5px;\
                                            padding: 0px 0px 0px 0px;\
                                            min-width: 17px;\
                                            font: 14px \"Arial\";");

    localHostAddrComboBox->setCompleter(completer);*/



    //找本地的IP地址.智能车为低端 所以需要匹配智能车所使用的IPV4
    //有可能本机有多个网卡 那么这时候需要选择.. 这里为了多电脑使用 后期必须要更改..
    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();//获取本地全部IP地址 包括ipv4和ipv6 // waring:Invalid parameter passed to C runtime function.
    // 获取第一个本主机的IPv4地址..
    for (int i = 0,nListSize = ipAddressesList.size(); i < nListSize; ++i)
    {
           if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
               if (strIpAddress.isEmpty())
               {
                   strIpAddress = ipAddressesList.at(i).toString();
                   //break;
               }
               localHostAddrComboBox->addItem(strIpAddress);
               //把以下内容要改到系统日志中..
               qDebug()<<"["<<i<<"]"<<ipAddressesList.at(i).toString();

           }
    }
    // 如果没有找到，则以本地IP地址为IP
    if (strIpAddress.isEmpty())
    {
        strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
        localHostAddrComboBox->addItem(strIpAddress);
    }
    localAddrLineEdit->setHidden(true);
    connect(localHostAddrComboBox,
            QOverload<const QString &>::of(&QComboBox::activated),
            [=](QString bindIP){
        bindLocalIP = bindIP;
        localAddrLineEdit->setText(bindLocalIP);
    });
    bindLocalIP = strIpAddress;



    //QLabel* testLabel = new QLabel(strIpAddress,this);
    //testLabel->setStyleSheet("background-color:#999999");
    //mainLayout->addWidget(testLabel);

    QHBoxLayout* mainItemLayout = new QHBoxLayout();

    QLabel* localAddrText = new QLabel("地址",this);
    localAddrText->setAlignment(Qt::AlignCenter);
    mainItemLayout->addWidget(localAddrText,0);
    mainItemLayout->addWidget(localHostAddrComboBox,3);

    QLineEdit* localPortLineEdit = new QLineEdit(this);
    //限制条件..绑定更改..
    connect(localPortLineEdit,&QLineEdit::textChanged,[=](QString bindLocalPortString){
        //todo:checkPortvalid
        bindLocalPort = bindLocalPortString.toUShort();
    });
    localPortLineEdit->setText("5556");

    mainItemLayout->addWidget(localPortLineEdit,1);


    mainLayout->addLayout(mainItemLayout);


    this->setLayout(mainLayout);

    //QPushButton* listenButton = new QPushButton("Listen");
    //listenButton->setCheckable(true);
    //listenButton->setChecked(false);

    /*
    [] 不截取任何变量
    [&} 截取外部作用域中所有变量，并作为引用在函数体中使用
    [=] 截取外部作用域中所有变量，并拷贝一份在函数体中使用
    [=, &foo]   截取外部作用域中所有变量，并拷贝一份在函数体中使用，但是对foo变量使用引用
    [bar]   截取bar变量并且拷贝一份在函数体重使用，同时不截取其他变量
    [this]            截取当前类中的this指针。如果已经使用了&或者=就默认添加此选项。
    */

    //按钮是需要打开和关闭udp通信的.如果同一封装数据格式..那么就…
    /*connect(listenButton,&QPushButton::toggled,[=](bool checked){
        qDebug()<<checked;
        listenButton->setText("Close");
    });*/



    udpSocket   = new QUdpSocket(this);

    connect(udpSocket,&QUdpSocket::readyRead,this,&UdpComponent::receiveUdpData);
    connect(udpSocket,&QUdpSocket::stateChanged,this,&UdpComponent::udpStateChanged);
    connect(udpSocket,static_cast<void (QUdpSocket::*)(QAbstractSocket::SocketError socketError)>
            (&QUdpSocket::error),this,&UdpComponent::udpErrorOccur);


    //自绑定
    connect(this,&UdpComponent::newDataSignal,dataCenterPtr,QOverload<QString,QByteArray>::of(&DataCenter::DataInterface));

}
UdpComponent::~UdpComponent()
{
    //
}

bool UdpComponent::BindInterface(bool checked)
{
    //QHostAddress
    if(checked)
    {
        qDebug()<<bindLocalIP<<bindLocalPort;
        //this->udpSocket->bind(bindLocalPort,QUdpSocket::ShareAddress);
        this->udpSocket->bind(QHostAddress(bindLocalIP),bindLocalPort,QUdpSocket::ShareAddress);
        emit BindSuccess(bindLocalIP+"/"+QString(bindLocalPort));
    }else{
        //emit Close[REMOVE]
        this->udpSocket->close();
    }
    //this->udpSocket->bind(port,QUdpSocket::ShareAddress);
    return true;
}






//UDP数据接收
void UdpComponent::receiveUdpData()
{
    //QHostAddress clientAddress;
    //quint16 recPort = 0;
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;//QByteArray makes a deep copy of the string data.
        //定义一个数组 接收
        datagram.resize(static_cast<int>(udpSocket->pendingDatagramSize()));//qint64 convert to int(32)根据可读数据来设置空间大小
        //Receives a datagram no larger than maxSize bytes and stores it in data.
        //The sender's host address and port is
        //              stored in *address and *port (unless the pointers are 0).
        //              If maxSize is too small, the rest of the datagram will be lost.

        //所以需要根据Sender的 addr. 和 port 来判别
        udpSocket->readDatagram(datagram.data(),datagram.size(),&currentClientAddress,&currentRecPort);//保存到当前地址

        //sKey
        QString current = QHostAddress(currentClientAddress.toIPv4Address()).toString().append("/").append(QString::number(currentRecPort));

        //发送信号 DataCenter将会接收
        emit newDataSignal(current,datagram);

        qDebug()<<current<<":invalid data size:"<<datagram.count();


        /*
        int ret = strData.compare("GetIPAddr");//这一段是判断命令的 就是返回ip地址!所以抄写的时候一定要注意这些关系
        if(0 == ret)
        {
            //qDebug()<<strData;
        }*/

        //udpSocket->writeDatagram(datagram,currentClientAddress,currentRecPort);//把数据直接发送回去

        //qDebug()<<strData;
    }
}


void UdpComponent::udpStateChanged(QAbstractSocket::SocketState socketState)
{
    switch(socketState)
    {
     case QAbstractSocket::UnconnectedState:
        //emit newMassage("Tcp网络未连接");
        break;
    case QAbstractSocket::HostLookupState:
        //emit newMassage("寻找服务主机中...");
        break;
    case QAbstractSocket::ConnectingState:
        //emit newMassage("找到服务主机");
        //emit newMassage("Tcp网络连接中...");
        break;
    case QAbstractSocket::ConnectedState:
        //emit newMassage("Tcp网络已连接！");
        break;
    case QAbstractSocket::ClosingState:
        //emit newMassage("Tcp网络准备关闭中...");
        break;
    default:
        break;
    }
}
void UdpComponent::udpErrorOccur(QAbstractSocket::SocketError socketError)
{
   emit error(udpSocket->errorString());
}

void UdpComponent::UdpSendData(QByteArray byteArray)
{
    udpSocket->writeDatagram(byteArray,currentClientAddress,currentRecPort);
}

