#include "CheckButton.h"

CheckButton::CheckButton(QString text,QWidget *parent) : QCheckBox(parent)
{
    setText(text);
    setCursor(QCursor(Qt::PointingHandCursor));
    setStyleSheet("background-color:#ccc;font:bold 14px Times, serif;padding:5px;");

    connect(this,&CheckButton::stateChanged,[=] (bool checked) {
        if(checked)
        {
            setStyleSheet("background-color:#777;font:bold 14px Times, serif;padding:5px;");
        }else{
            setStyleSheet("background-color:#ccc;font:bold 14px Times, serif;padding:5px;");
        }

    });
}
