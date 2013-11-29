#include "TimeEntryWidget.h"

TimeEntryWidget::TimeEntryWidget(QWidget *parent) :
    QWidget(parent)
{
    timeLabel_ = new QLabel("Time :");

    timeAmount_ = new QSpinBox();
    connect( timeAmount_, SIGNAL(valueChanged(int)), this, SIGNAL(timeAmountChanged(int)) );

    timeUnits_ = new QComboBox();
    connect( timeUnits_, SIGNAL(currentIndexChanged(QString)), this, SIGNAL(timeUnitsChanged(QString)) );
    connect( timeUnits_, SIGNAL(activated(QString)), this, SIGNAL(timeUnitsChanged(QString)) );
    connect( this, SIGNAL(setUnits(int)), timeUnits_, SLOT(setCurrentIndex(int)) );

    connect( this, SIGNAL(timeUnitsChanged(QString)), this, SLOT(toTestSignal(QString)) );

    QHBoxLayout *timeLayout = new QHBoxLayout();
    timeLayout->addWidget(timeLabel_);
    timeLayout->addWidget(timeAmount_);
    timeLayout->addWidget(timeUnits_);

    setLayout(timeLayout);
}



TimeEntryWidget::~TimeEntryWidget()
{
}



void TimeEntryWidget::initialize()
{
    timeAmount_->setValue(10);

    timeUnits_->addItem("seconds");
    timeUnits_->addItem("minutes");
    timeUnits_->addItem("hours");

    emit setUnits(0);
}



void TimeEntryWidget::toTestSignal(const QString &signalText)
{
    qDebug() << "Time entry widget emitted signal with this text:" << signalText;
}
