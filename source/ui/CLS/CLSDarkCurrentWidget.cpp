#include "CLSDarkCurrentWidget.h"

CLSDarkCurrentWidget::CLSDarkCurrentWidget(double dwellSeconds, QWidget *parent) :
    QWidget(parent)
{
    dwellTime_ = dwellSeconds;

    QLabel *widgetTitle = new QLabel("Dark Current Correction");

    QLabel *dwellTimeLabel = new QLabel("Dwell time (s): ");
    dwellTimeEntry_ = new QLineEdit(QString::number(dwellTime_));

    darkCurrentButton_ = new QPushButton("Collect");
    darkCurrentButton_->setEnabled(true);
    connect( darkCurrentButton_, SIGNAL(clicked()), this, SLOT(onDarkCurrentButtonClicked()) );

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(widgetTitle, 0, 0);
    layout->addWidget(dwellTimeLabel, 1, 0);
    layout->addWidget(dwellTimeEntry_, 1, 1);
    layout->addWidget(darkCurrentButton_, 2, 1);

    setLayout(layout);
}



CLSDarkCurrentWidget::~CLSDarkCurrentWidget()
{
}



void CLSDarkCurrentWidget::disableDarkCurrentButton()
{
    darkCurrentButton_->setEnabled(false);
}



void CLSDarkCurrentWidget::enableDarkCurrentButton()
{
    darkCurrentButton_->setEnabled(true);
}



void CLSDarkCurrentWidget::setDarkCurrentButtonEnabled(bool isEnabled)
{
    darkCurrentButton_->setEnabled(isEnabled);
}



void CLSDarkCurrentWidget::onDarkCurrentButtonClicked()
{
    double dwellEntered = dwellTimeEntry_->text().toDouble();

    if (dwellEntered <= 0) {
        return;
    }

    emit darkCurrentButtonClicked(dwellTime_ = dwellEntered);
}
