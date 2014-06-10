#include "CLSSIS3820ScalerChannelViewWithDarkCurrent.h"

#include <QDebug>

CLSSIS3820ScalerChannelViewWithDarkCurrent::CLSSIS3820ScalerChannelViewWithDarkCurrent(CLSSIS3820ScalerChannel *channel, QWidget *parent) :
    CLSSIS3820ScalerChannelView(channel, parent)
{
    darkCurrentValueLabel_ = new QLabel();
    setDarkCurrentValueLabel(channel_->detector()->darkCurrentMeasurementValue());

    correctedMeasurementLabel_ = new QLabel();
    setCorrectedMeasurementLabel(0);

    content_ = new QVBoxLayout(this);
    content_->addWidget(darkCurrentValueLabel_);
    content_->addWidget(correctedMeasurementLabel_);

    if (channel_->detector()->canDoDarkCurrentCorrection()) {

        connect( channel_->detector(), SIGNAL(newDarkCurrentMeasurementValueReady(double)), this, SLOT(onNewDarkCurrentMeasurementValue(double)) );
        connect( channel_->detector(), SIGNAL(requiresNewDarkCurrentMeasurement(bool)), this, SLOT(onNewDarkCurrentMeasurementState(bool)) );
        connect( channel_->detector(), SIGNAL(newValuesAvailable()), this, SLOT(onDetectorNewValuesAvailable()) );

        setDarkCurrentViewMode(Show);

    } else {
        setDarkCurrentViewMode(Hide);
    }
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
        channelLayout_->removeItem(content_);
        emit darkCurrentViewModeChanged(darkCurrentViewMode_);

    } else if (darkCurrentViewMode_ == Show) {
        channelLayout_->addLayout(content_);
        emit darkCurrentViewModeChanged(darkCurrentViewMode_);

    } else {
        qDebug() << "Unknown dark current view mode encountered. Setting view mode to 'Hide'.";
        setDarkCurrentViewMode(Hide);
    }
}



void CLSSIS3820ScalerChannelViewWithDarkCurrent::onNewDarkCurrentMeasurementValue(double newValue)
{
    qDebug() << "CLSSIS3820ScalerChannelView receives new dark current measurement value : " << newValue;
    setDarkCurrentValueLabel(newValue);
}



void CLSSIS3820ScalerChannelViewWithDarkCurrent::onNewDarkCurrentMeasurementState(bool measurementUpToDate)
{
    qDebug() << "CLSSIS3820ScalerChannelView receives new dark current required status : " << measurementUpToDate;

    if (measurementUpToDate)
        darkCurrentValueLabel_->setStyleSheet("color: blue;");

    else
        darkCurrentValueLabel_->setStyleSheet("color: red;");
}



void CLSSIS3820ScalerChannelViewWithDarkCurrent::onDetectorNewValuesAvailable()
{
    // we arrive at a dark current corrected value by dividing the number of counts aquired by the measurement dwell time, and subtract the dark current.
    double *newValue = 0;
    channel_->detector()->data(newValue);

    double dwellTime = channel_->detector()->acquisitionTime();

    // the latest value of the detector's dark current. this is normalized by its acquisition time already (see AMDetector::setAsDarkCurrentMeasurementValue()).
    double darkCurrent = channel_->detector()->darkCurrentMeasurementValue();

    double correctedMeasurement_ = *newValue / dwellTime - darkCurrent;

    qDebug() << "CLSSIS3820ScalerChannelView has new DC corrected value : " << correctedMeasurement_;

    setCorrectedMeasurementLabel(correctedMeasurement_);
}



void CLSSIS3820ScalerChannelViewWithDarkCurrent::setDarkCurrentValueLabel(double displayValue)
{
    darkCurrentValueLabel_->setText(QString("Dark current value : %1").arg(displayValue, 0, 'f', 2));
}



void CLSSIS3820ScalerChannelViewWithDarkCurrent::setCorrectedMeasurementLabel(double displayValue)
{
    correctedMeasurementLabel_->setText(QString("Corrected measurement : %1").arg(displayValue, 0, 'f', 2));
}




