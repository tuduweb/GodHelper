#pragma once

#include <QVector>
#include <QThread>
#include <QImage>
#include <QMutex>
#include <QQueue>

#include <condition_variable>


#include <QDebug>

struct RecordItem{
    QString recordName;
    QImage record;

    RecordItem(const QImage& _record, const QString& name) {
        recordName = name;
        record = _record;
    }
};

class RecordLoader: public QThread{
    Q_OBJECT
public:
    //RecordLoader(QVector<QImage>& _buddle): m_buddle(_buddle) {}
    RecordLoader(QVector<RecordItem>& _recordBuddle): m_recordBuddle(_recordBuddle) {}
protected:
    void run() override;

    //QThread* thread;

    //QVector<QImage>& m_buddle;
    QVector<RecordItem>& m_recordBuddle;

signals:
    void recordLoadered(const QString& imageFileUri);
};

class RecordModel;
class RecordList;

class RecordCenter: public QObject
{
    Q_OBJECT

    friend RecordModel;
    friend RecordList;

public:
    RecordCenter();


protected:
    //QVector<QImage> m_recordBuddle;
    QVector<RecordItem> m_recordBuddle;

    QVector<RecordLoader*> loaders;

signals:
    void recordLoadered(const QString& imageFileUri);
};

