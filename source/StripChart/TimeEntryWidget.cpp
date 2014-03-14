#include "TimeEntryWidget.h"

TimeEntryWidget::TimeEntryWidget(QWidget *parent) :
    QWidget(parent)
{
    timeLabel_ = 0;
    timeAmount_ = 0;
    timeUnits_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();

    QHBoxLayout *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(timeLabel_);
    timeLayout->addWidget(timeAmount_);
    timeLayout->addWidget(timeUnits_);

    setLayout(timeLayout);
}



TimeEntryWidget::~TimeEntryWidget()
{
}



int TimeEntryWidget::timeAmount() const
{
    return timeAmount_->value();
}



QString TimeEntryWidget::timeUnits() const
{
    return timeUnits_->currentText();
}



void TimeEntryWidget::setTimeAmount(int amount)
{
    timeAmount_->setValue(amount);
}



void TimeEntryWidget::setTimeUnits(const QString &units)
{
    if (units == "sec") {
        timeUnits_->setCurrentIndex(TimeEntryWidget::Seconds);
        timeAmount_->setMaximum(secondsMax_);

    } else if (units == "min") {
        timeUnits_->setCurrentIndex(TimeEntryWidget::Minutes);
        timeAmount_->setMaximum(minutesMax_);

    } else if (units == "hr") {
        timeUnits_->setCurrentIndex(TimeEntryWidget::Hours);
        timeAmount_->setMaximum(hoursMax_);

    } else {
        qDebug() << "TimeEntryWidget :: did not recognize " << units << " as valid time unit. No change made.";
    }
}



void TimeEntryWidget::buildComponents()
{
    secondsMax_ = 60;
    minutesMax_ = 60;
    hoursMax_ = 24;

    timeLabel_ = new QLabel("Time :");
    timeAmount_ = new QSpinBox();
    timeUnits_ = new QComboBox();
}



void TimeEntryWidget::makeConnections()
{
    connect( timeAmount_, SIGNAL(valueChanged(int)), this, SIGNAL(timeAmountChanged(int)) );
    connect( timeUnits_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(timeUnitsChanged(int)) );
    connect( timeUnits_, SIGNAL(activated(int)), this, SIGNAL(timeUnitsChanged(int)) );
}



void TimeEntryWidget::defaultSettings()
{
    timeAmount_->setMinimum(1);
    timeAmount_->setValue(10);

    timeUnits_->addItem("sec");
    timeUnits_->addItem("min");
    timeUnits_->addItem("hr");
    timeUnits_->setCurrentIndex(TimeEntryWidget::Seconds);
}
