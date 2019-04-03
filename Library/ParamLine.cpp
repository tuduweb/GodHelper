#include "ParamLine.h"
#include <QLineEdit>
#include <QLabel>
#include <QLayout>
#include <QSpinBox>

ParamLine::ParamLine(QWidget *parent) : QWidget(parent)
{
    QLabel* label = new QLabel(this);
    QLineEdit* lineEdit = new QLineEdit(this);

    label->setText("Pitch.P");
    label->setMinimumWidth(100);
    label->setMaximumWidth(100);
    label->setAlignment(Qt::AlignCenter);
    lineEdit->setText("11.1");

    QDoubleSpinBox* spinBox = new QDoubleSpinBox(this);
    spinBox->setValue(11.1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(spinBox);


}
