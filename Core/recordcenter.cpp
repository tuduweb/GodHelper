#include "recordcenter.hpp"

QQueue<QString> m_loadQueue;
QMutex m_queue_mutex;
bool ready = false;
std::condition_variable cv;


void RecordLoader::run() {
    //QString _loadPath = "D:/monitorPics/data/test--1747883617.bmp";
    //QImage image(_loadPath);

    //m_buddle.append(image);

    while(1) {

        while(1) {
            //

            m_queue_mutex.lock();
            if(m_loadQueue.isEmpty()) {
                m_queue_mutex.unlock();
                break;
            }
            QString _loadPath = m_loadQueue.front();

            if(_loadPath.isNull() == false) {
                m_loadQueue.pop_front();
                m_queue_mutex.unlock();

                QImage image(_loadPath);

                if(image.isNull() == false) {
                    RecordItem item{image, _loadPath};
                    m_recordBuddle.append(item);
                    emit recordLoadered(item.recordName);
                }
                qDebug() << _loadPath << image.size();
            }else{
                m_queue_mutex.unlock();
            }

            QThread::msleep(100);

            //当为空 跳出
        }

        QThread::sleep(1);
    }
}

#include <QDir>

RecordCenter::RecordCenter() {

    QString loadDirPath = "D:/monitorPics/data/";
    QDir picsDir(loadDirPath);
    if(picsDir.exists() == false) {
        qDebug() << loadDirPath << " not exist";
    }

    int fileCount = picsDir.count();

    for(int i = 0; i < fileCount; ++i) {
        const auto& fileName = picsDir[i];

        if(false == fileName.endsWith(".bmp")) {
            continue;
        }

        m_loadQueue.push_back(loadDirPath + fileName);
    }


    m_loadQueue.push_back("D:/monitorPics/data/test--1747883617.bmp");

    for(int i = 0; i < 4; ++i) {
        auto item = new RecordLoader(m_recordBuddle);

        //signals
        connect(item, &RecordLoader::recordLoadered, this, &RecordCenter::recordLoadered);

        loaders.push_back(item);
    }

    for(const auto& loader: loaders) {
        loader->start();
    }

}
