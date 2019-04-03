#ifndef CHECKBUTTON_H
#define CHECKBUTTON_H

#include <QWidget>
#include <QCheckBox>

class CheckButton : public QCheckBox
{
    Q_OBJECT
public:
    explicit CheckButton(QString text,QWidget *parent = nullptr);

signals:

public slots:
};

#endif // CHECKBUTTON_H
