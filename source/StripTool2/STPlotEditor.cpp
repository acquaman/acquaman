#include "STPlotEditor.h"

STPlotEditor::STPlotEditor(QWidget *parent) : QWidget(parent)
{
    timeEntry_ = new QLineEdit();
    timeUnits_ = new QComboBox();

    QHBoxLayout *timeLayout = new QHBoxLayout(this);
    timeLayout->addWidget(timeEntry_);
    timeLayout->addWidget(timeUnits_);

    okButton_ = new QPushButton("Ok");
    connect( okButton_, SIGNAL(clicked()), this, SIGNAL(editComplete()) );

    cancelButton_ = new QPushButton("Cancel");
    connect( cancelButton_, SIGNAL(clicked()), this, SIGNAL(editCancelled()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(okButton_);
    buttonLayout->addWidget(cancelButton_);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(timeLayout);
    layout->addLayout(buttonLayout);

    setLayout(layout);
}

STPlotEditor::~STPlotEditor()
{

}

QLineEdit* STPlotEditor::timeEntry() const
{
    return timeEntry_;
}

QComboBox* STPlotEditor::timeUnits() const
{
    return timeUnits_;
}

void STPlotEditor::setTimeEntry(int timeValue)
{
    timeEntry_->setText(QString::number(timeValue));
}

void STPlotEditor::setTimeUnits()
{

}
