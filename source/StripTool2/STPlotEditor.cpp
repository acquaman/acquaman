#include "STPlotEditor.h"

STPlotEditor::STPlotEditor(STPlotWidget *plot, QWidget *parent) : STEditor(parent)
{
    plot_ = plot;

    QGridLayout *mainLayout = new QGridLayout(this);
\
    QLabel *timeLabel = new QLabel(this);
    timeLabel->setText("Time :");
    timeEntry_ = new QLineEdit(this);
    mainLayout->addWidget(timeLabel, 0, 0);
    mainLayout->addWidget(timeEntry_, 0, 1);

    QLabel *unitsLabel = new QLabel("Units :", this);
    timeUnits_ = new QComboBox();
    mainLayout->addWidget(unitsLabel, 1, 0);
    mainLayout->addWidget(timeUnits_, 1, 1);

    setLayout(mainLayout);
}

STPlotEditor::~STPlotEditor()
{

}

void STPlotEditor::applyChanges()
{

}

void STPlotEditor::setTimeEntry(int timeValue)
{
    timeEntry_->setText(QString::number(timeValue));
}

//void STPlotEditor::setTimeUnits(STPlot::TimeUnits newUnits)
//{
//    Q_UNUSED(newUnits)
//}
