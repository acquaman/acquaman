//#include "CLSSIS3820ScalerViewWithDarkCurrent.h"
//#include <QDebug>

//CLSSIS3820ScalerViewWithDarkCurrent::CLSSIS3820ScalerViewWithDarkCurrent(CLSSIS3820Scaler *scaler, QWidget *parent) :
//    CLSSIS3820ScalerView(scaler, parent)
//{
//    QLabel *darkCurrentLabel = new QLabel("Dark current value : ");
//    darkCurrentValue_ = new QLabel(" ");

//    QLayout *layout = new QHBoxLayout();
//    layout->addWidget(darkCurrentLabel);
//    layout->addWidget(darkCurrentValue_);

//    QGroupBox *darkCurrentContent_ = new QGroupBox(this);
//    darkCurrentContent_->setLayout(layout);
//    darkCurrentContent_->setFlat(true);

//    channelLayout_->addWidget(darkCurrentContent_);
//    setDarkCurrentViewMode(Hide);

//    connect( scaler_, SIGNAL(newDarkCurrentValue(double)), this, SLOT(onNewScalerDarkCurrentValue(double)) );
//    connect( scaler_, SIGNAL(darkCurrentCorrectionStateChanged(CLSSIS3820Scaler::DarkCurrentCorrectionState)), this, SLOT(onNewScalerDarkCurrentStatus(CLSSIS3820Scaler::DarkCurrentCorrectionState)) );
//}



//CLSSIS3820ScalerViewWithDarkCurrent::~CLSSIS3820ScalerViewWithDarkCurrent()
//{

//}



//CLSSIS3820ScalerViewWithDarkCurrent::DarkCurrentViewMode CLSSIS3820ScalerViewWithDarkCurrent::darkCurrentViewMode()
//{
//    return viewMode_;
//}



//void CLSSIS3820ScalerViewWithDarkCurrent::setDarkCurrentViewMode(DarkCurrentViewMode newMode)
//{
//    viewMode_ = newMode;

//    if (viewMode_ == Hide) {
//        darkCurrentContent_->hide();
//        emit viewModeChanged(viewMode_);

//    } else if (viewMode_ == Show) {
//        darkCurrentContent_->show();
//        emit viewModeChanged(viewMode_);

//    } else {
//        qDebug() << "Unknown view mode encountered in CLSSIS3820ScalerViewWithDarkCurrent. Setting view mode to 'Hide'.";
//        setDarkCurrentViewMode(Hide);
//    }

//}



//void CLSSIS3820ScalerViewWithDarkCurrent::onNewScalerDarkCurrentValue(double newMeasurement)
//{
//    qDebug() << "Scaler reports new dark current measurement value : " << newMeasurement;
//    darkCurrentValue_->setText(QString("%f").arg(newMeasurement, 0, 'g', 3));
//}



//void CLSSIS3820ScalerViewWithDarkCurrent::onNewScalerDarkCurrentStatus(CLSSIS3820Scaler::DarkCurrentCorrectionState newStatus)
//{
//    qDebug() << "Scaler reports new dark current status : " << newStatus;

//    if (newStatus == CLSSIS3820Scaler::SUCCEEDED)
//        darkCurrentValue_->setStyleSheet("QLabel { color: blue }");

//    else
//        darkCurrentValue_->setStyleSheet("QLabel { color: red }");
//}
