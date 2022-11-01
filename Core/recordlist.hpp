#ifndef RECORDLIST_HPP
#define RECORDLIST_HPP

#include <QListView>
#include <QAbstractListModel>

#include "recordcenter.hpp"

class RecordModel: public QAbstractListModel{

public:
    RecordModel(RecordCenter *center): p_recordCenter(center) {
        connect(center, &RecordCenter::recordLoadered, this, [&](const QString&) {
            //this->
            //this->beginResetModel();
            //this->endResetModel();
        });
    }
    int rowCount(const QModelIndex &parent) const override {
        //https://doc.qt.io/qt-5/qabstractitemmodel.html#rowCount
        //Note: When implementing a table based model, rowCount() should return 0 when the parent is valid.
        return parent.isValid() ? 0 : p_recordCenter->m_recordBuddle.size();
    };
    QVariant data(const QModelIndex &index, int role) const override {
        if (role == Qt::DisplayRole) {
            return p_recordCenter->m_recordBuddle[index.row()].recordName;
        }else if(role == Qt::UserRole) {
            return p_recordCenter->m_recordBuddle[index.row()].record;
        }

        return QVariant();
    }

//    virtual QHash<int, QByteArray> roleNames() const override {
//        //
//    }

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override {
        if (!index.isValid())
            return QAbstractListModel::flags(index) | Qt::ItemIsDropEnabled;


        return QAbstractListModel::flags(index) |
               Qt::ItemIsDragEnabled;
        //Qt::ItemIsEditable |
    }


    RecordCenter* p_recordCenter;
};

class RecordList: public QListView
{
    Q_OBJECT

public:
    RecordList(RecordCenter *center, QWidget* parent = nullptr): QListView(parent), p_recordCenter(center), m_model(new RecordModel(center)) {
        setModel(m_model);

        connect(center, &RecordCenter::recordLoadered, this, [&](const QString& _path) {
            //qDebug() << _path;
            //this->setModel(m_model);
        });
    }

    RecordCenter* p_recordCenter;


protected slots:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override {
        QListView::currentChanged(current, previous);

        if(current.isValid()) {
            emit imageChanged(p_recordCenter->m_recordBuddle[current.row()].record);
        }

    }

signals:
    void imageChanged(const QImage& image);

protected:
    RecordModel *m_model;
};

#endif // RECORDLIST_HPP
