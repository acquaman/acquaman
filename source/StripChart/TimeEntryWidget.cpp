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
}



TimeEntryWidget::~TimeEntryWidget()
{
}



int TimeEntryWidget::timeAmount() const
{
    return timeAmount_->value();
}



TimeEntryWidget::TimeUnits TimeEntryWidget::timeUnits() const
{
    TimeEntryWidget::TimeUnits units;

    switch (timeUnits_->currentIndex()) {
    case (0) :
        units = TimeEntryWidget::Seconds;
        break;
    case (1) :
        units = TimeEntryWidget::Minutes;
        break;

    case (2) :
        units = TimeEntryWidget::Hours;
        break;

    default:
        qDebug() << "TimeEntryWidget::timeUnits() : unknown units encountered!! Setting units to seconds.";
        units = TimeEntryWidget::Seconds;
        timeUnits_->setCurrentIndex(TimeEntryWidget::Seconds);
        break;
    }

    return units;
}



void TimeEntryWidget::setTimeAmount(int amount)
{
    timeAmount_->setValue(amount);
}



void TimeEntryWidget::setTimeUnits(TimeEntryWidget::TimeUnits newUnits)
{   
    timeUnits_->setCurrentIndex(newUnits);

    switch (newUnits) {
    case (TimeEntryWidget::Seconds) :
        timeAmount_->setMaximum(secondsMax_);
        emit timeUnitsChanged(newUnits);
        break;

    case (TimeEntryWidget::Minutes) :
        timeAmount_->setMaximum(minutesMax_);
        emit timeUnitsChanged(newUnits);
        break;

    case (TimeEntryWidget::Hours) :
        timeAmount_->setMaximum(hoursMax_);
        emit timeUnitsChanged(newUnits);
        break;

    default:
        qDebug() << "TimeEntryWidget::setTimeUnits(...) : unknown units encountered!! Setting units to seconds.";
        setTimeAmount(10);
        setTimeUnits(TimeEntryWidget::Seconds);
        break;
    }
}



void TimeEntryWidget::onTimeUnitsChanged(int newUnitsIndex)
{
    switch (newUnitsIndex) {
    case (0) :
        emit timeUnitsChanged(TimeEntryWidget::Seconds);
        break;
    case (1) :
        emit timeUnitsChanged(TimeEntryWidget::Minutes);
        break;
    case (2) :
        emit timeUnitsChanged(TimeEntryWidget::Hours);
        break;

    default:
        qDebug() << "TimeEntryWidget::onTimeUnitsChanged(int) : unknown units encountered!! Setting time to seconds.";
        setTimeAmount(10);
        setTimeUnits(TimeEntryWidget::Seconds);
        break;
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
    connect( timeUnits_, SIGNAL(currentIndexChanged(int)), this, SLOT(onTimeUnitsChanged(int)) );
}



void TimeEntryWidget::defaultSettings()
{
    timeAmount_->setMinimum(1);
    timeAmount_->setValue(10);

    timeUnits_->addItem("sec");
    timeUnits_->addItem("min");
    timeUnits_->addItem("hr");

    setTimeUnits(TimeEntryWidget::Seconds);

    QHBoxLayout *timeLayout = new QHBoxLayout();
//    timeLayout->addWidget(timeLabel_);
    timeLayout->addWidget(timeAmount_);
    timeLayout->addWidget(timeUnits_);

    setLayout(timeLayout);
}