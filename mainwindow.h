#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>


extern QThread dataCenterThread;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QThread dataCenterThread;
    QThread imgProcThread;//图像线程


    //PCONCMD consoleCMDPtr;




private:
    Ui::MainWindow *ui;

/*
    ImgShowCompnent *imgShow;

    DataPanelComponent *dataPanel;

    DataSourceComponent *dataSource;

    SenderComponent *sender;

    DataCenter *dataCenter;

    ImgProc *imgProc;

    WaveFormComponent *waveForm;

    InfoPanelComponent *infoPanel;
*/
private slots:
    void myTest(bool);

    void CMDPackageHandler(QByteArray CMDByteArray);

    void AddMsg(uint mtype,QString* msg);


};

#endif // MAINWINDOW_H
