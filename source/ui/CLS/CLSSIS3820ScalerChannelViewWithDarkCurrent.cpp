#include "CLSSIS3820ScalerChannelViewWithDarkCurrent.h"

#include <QDebug>

CLSSIS3820ScalerChannelViewWithDarkCurrent::CLSSIS3820ScalerChannelViewWithDarkCurrent(CLSSIS3820ScalerChannel *channel, QWidget *parent) :
    CLSSIS3820ScalerChannelView(channel, parent)
{
    QLabel *darkCurrentLabel = new QLabel("Dark current value : ");
    darkCurrentValue_ = new QLabel(" ");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(darkCurrentLabel);
    layout->addWidget(darkCurrentValue_);

    darkCurrentContent_ = new QGroupBox();
    darkCurrentContent_->setLayout(layout);
    darkCurrentContent_->setFlat(true);

    setDarkCurrentViewMode(Hide);

    connect( channel_, SIGNAL(newDarkCurrentMeasurementValue(double)), this, SLOT(onNewDarkCurrentMeasurementValue(double)) );
    connect( channel_, SIGNAL(newDarkCurrentMeasurementState(CLSSIS3820Scaler::DarkCurrentCorrectionState)), this, SLOT(onNewDarkCurrentMeasurementState(CLSSIS3820Scaler::DarkCurrentCorrectionState)) );

    channelLayout_->addWidget(darkCurrentContent_);
}



CLSSIS3820ScalerChannelViewWithDarkCurrent::~CLSSIS3820ScalerChannelViewWithDarkCurrent()
{

}



CLSSIS3820ScalerChannelViewWithDarkCurrent::DarkCurrentViewMode CLSSIS3820ScalerChannelViewWithDarkCurrent::darkCurrentViewMode()
{
    return darkCurrentViewMode_;
}



void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDarkCurrentViewMode(DarkCurrentViewMode newMode)
{
    darkCurrentViewMode_ = newMode;

    if (darkCurrentViewMode_ == Hide) {
        darkCurrentContent_->hide();
        emit darkCurrentViewModeChanged(darkCurrentViewMode_);

    } else if (darkCurrentViewMode_ == Show) {
        darkCurrentContent_->show();
        emit darkCurrentViewModeChanged(darkCurrentViewMode_);

    } else {
        qDebug() << "Unknown dark current view mode encountered. Setting view mode to 'Hide'.";
        setDarkCurrentViewMode(Hide);
    }
}



void CLSSIS3820ScalerChannelViewWithDarkCurrent::onNewDarkCurrentMeasurementValue(double newValue)
{
    qDebug() << "CLSSIS3820ScalerChannelView receives new dark current measurement value : " << newValue;
    darkCurrentValue_->setText(QString("%1").arg(newValue, 0, 'f', 2));
}



void CLSSIS3820ScalerChannelViewWithDarkCurrent::onNewDarkCurrentMeasurementState(bool measurementUpToDate)
{
    qDebug() << "CLSSIS3820ScalerChannelView new dark current required status : " << measurementUpToDate;

    if (measurementUpToDate)
        darkCurrentValue_->setStyleSheet("color: blue;");

    else
        darkCurrentValue_->setStyleSheet("color: red;");
}
