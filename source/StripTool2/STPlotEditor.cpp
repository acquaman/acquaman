#include "STPlotEditor.h"

STPlotEditor::STPlotEditor(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *timeLabel = new QLabel(this);
    timeLabel->setText("Time :");
    timeEntry_ = new QLineEdit(this);

    QLabel *unitsLabel = new QLabel("Units :", this);
    timeUnits_ = new QComboBox();

    QGridLayout *timeLayout = new QGridLayout(this);
    timeLayout->addWidget(timeLabel, 0, 0);
    timeLayout->addWidget(timeEntry_, 0, 1);
    timeLayout->addWidget(unitsLabel, 1, 0);
    timeLayout->addWidget(timeUnits_, 1, 1);

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

void STPlotEditor::setTimeEntry(int timeValue)
{
    timeEntry_->setText(QString::number(timeValue));
}

void STPlotEditor::setTimeUnits(STPlot::TimeUnits newUnits)
{
    Q_UNUSED(newUnits)
}
