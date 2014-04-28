#include "CLSDarkCurrentWidget.h"

CLSDarkCurrentWidget::CLSDarkCurrentWidget(double dwellSeconds, QWidget *parent) :
    QWidget(parent)
{
    dwellTime_ = dwellSeconds;

    QLabel *dwellTimeLabel = new QLabel("Dwell time (s): ");
    dwellTimeEntry_ = new QLineEdit(QString::number(dwellTime_));
    connect( dwellTimeEntry_, SIGNAL(textChanged(QString)), this, SLOT(onDwellTimeEntryChanged(QString)) );

    QHBoxLayout *dwellLayout = new QHBoxLayout();
    dwellLayout->addWidget(dwellTimeLabel);
    dwellLayout->addWidget(dwellTimeEntry_);

    darkCurrentButton_ = new QPushButton("Dark Current Measurement");
    darkCurrentButton_->setEnabled(true);
    connect( darkCurrentButton_, SIGNAL(clicked()), this, SLOT(onDarkCurrentButtonClicked()) );

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(dwellLayout);
    layout->addWidget(darkCurrentButton_);

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



void CLSDarkCurrentWidget::onDwellTimeEntryChanged(const QString &entryText)
{
    double entryNumber = entryText.toDouble();
    dwellTime_ = entryNumber;
}



void CLSDarkCurrentWidget::onDarkCurrentButtonClicked()
{
    if (dwellTime_ <= 0) {
        qDebug() << "CLSDarkCurrentWidget :: the dwellTime_ is less than or equal to zero. Cannot complete dark current measurement.";
        return;
    }

    if (dwellTime_ != dwellTimeEntry_->text().toDouble()) {
        qDebug() << "CLSDarkCurrentWidget :: the dwellTime_ does not match what's entered in dwellTimeEntry_. Cannot complete dark current measurement.";
        return;
    }

    emit darkCurrentButtonClicked(dwellTime_);
}
