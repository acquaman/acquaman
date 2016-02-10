#include "CLSSIS3820ScalerControlsView.h"

CLSSIS3820ScalerControlsView::CLSSIS3820ScalerControlsView(CLSSIS3820Scaler *scaler, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	scaler_ = 0;

	// Create UI elements.

	acquisitionModeBox_ = new QComboBox;

	startButton_ = new QPushButton(QIcon(":/22x22/media-playback-start.png"), "Start");
	startButton_->setMaximumHeight(25);

	stopButton_ = new QPushButton(QIcon(":/22x22/media-playback-stop.png"), "Stop");
	stopButton_->setMaximumHeight(25);

	statusLabel_ = new QLabel;

	dwellTimeBox_ = new QSpinBox;
	dwellTimeBox_->setFixedWidth(100);
	dwellTimeBox_->setAlignment(Qt::AlignCenter);

	scansPerBufferBox_ = new QSpinBox;
	scansPerBufferBox_->setFixedWidth(100);
	scansPerBufferBox_->setAlignment(Qt::AlignCenter);

	totalScansBox_ = new QSpinBox;
	totalScansBox_->setFixedWidth(100);
	totalScansBox_->setAlignment(Qt::AlignCenter);

	// Create and set layouts.

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(startButton_);
	buttonsLayout->addWidget(stopButton_);

	QVBoxLayout *statusLayout = new QVBoxLayout;
	statusLayout->addWidget(statusLabel_, 0, Qt::AlignCenter);
	statusLayout->addLayout(buttonsLayout);
	statusLayout->addWidget(acquisitionModeBox_, 0, Qt::AlignCenter);

	QGridLayout *controlsLayout = new QGridLayout();
	controlsLayout->addWidget(new QLabel("Dwell Time: "), 0, 0, 1, 1, Qt::AlignRight);
	controlsLayout->addWidget(dwellTimeBox_, 0, 1);
	controlsLayout->addWidget(new QLabel("Scans per Buffer: "), 1, 0, 1, 1, Qt::AlignRight);
	controlsLayout->addWidget(scansPerBufferBox_, 1, 1);
	controlsLayout->addWidget(new QLabel("Total Scans: "), 2, 0, 1, 1, Qt::AlignRight);
	controlsLayout->addWidget(totalScansBox_, 2, 1);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addLayout(statusLayout);
	layout->addLayout(controlsLayout);

	setLayout(layout);

	// Make connections.

	connect(startButton_, SIGNAL(clicked()), this, SLOT(startScanning()));
	connect(stopButton_, SIGNAL(clicked()), this, SLOT(stopScanning()));
	connect(acquisitionModeBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(setContinuous()));
	connect(dwellTimeBox_, SIGNAL(editingFinished()), this, SLOT(setDwellTime()));
	connect(scansPerBufferBox_, SIGNAL(editingFinished()), this, SLOT(setScansPerBuffer()));
	connect(totalScansBox_, SIGNAL(editingFinished()), this, SLOT(setTotalScans()));

	// Current settings.

	setScaler(scaler);

	refresh();
}

CLSSIS3820ScalerControlsView::~CLSSIS3820ScalerControlsView()
{

}

void CLSSIS3820ScalerControlsView::refresh()
{
	updateStartButton();
	updateStopButton();
	updateStatusLabel();
	updateAcquisitionModeBox();
	updateDwellTimeBox();
	updateScansPerBufferBox();
	updateTotalScansBox();
}

void CLSSIS3820ScalerControlsView::setScaler(CLSSIS3820Scaler *newScaler)
{
	if (scaler_ != newScaler) {

		if (scaler_) {
			disconnect( scaler_, 0, this, 0 );
		}

		scaler_ = newScaler;

		if (scaler_) {
			connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(refresh()) );
			connect( scaler_, SIGNAL(scanningChanged(bool)), this, SLOT(onScalerScanningStateChanged()) );
			connect( scaler_, SIGNAL(continuousChanged(bool)), this, SLOT(onScalerContinuousStateChanged()) );
			connect( scaler_, SIGNAL(dwellTimeChanged(double)), this, SLOT(updateDwellTimeBox()) );
			connect( scaler_, SIGNAL(scansPerBufferChanged(int)), this, SLOT(updateScansPerBufferBox()) );
			connect( scaler_, SIGNAL(totalScansChanged(int)), this, SLOT(updateTotalScansBox()) );
		}

		refresh();

		emit scalerChanged(scaler_);
	}
}

void CLSSIS3820ScalerControlsView::startScanning()
{
	if (scaler_)
		scaler_->setScanning(true);
}

void CLSSIS3820ScalerControlsView::stopScanning()
{
	if (scaler_)
		scaler_->setScanning(false);
}

void CLSSIS3820ScalerControlsView::setContinuous()
{
	if (scaler_ && scaler_->isConnected()) {
		bool modeOK = false;
		int mode = acquisitionModeBox_->itemData(acquisitionModeBox_->currentIndex()).toInt(&modeOK);

		if (modeOK) {
			if (mode == CLSSIS3820Scaler::Continuous)
				scaler_->setContinuous(true);
			else if (mode == CLSSIS3820Scaler::SingleShot)
				scaler_->setContinuous(false);
		}
	}
}

void CLSSIS3820ScalerControlsView::setDwellTime()
{
	if (scaler_ && scaler_->isConnected()) {
		double dwellTime = dwellTimeBox_->value() / 1000.0; // The dwell time box displays time in ms, the scaler keeps time in s.
		scaler_->setDwellTime(dwellTime);
	}
}

void CLSSIS3820ScalerControlsView::setScansPerBuffer()
{
	if (scaler_ && scaler_->isConnected()) {
		scaler_->setScansPerBuffer(scansPerBufferBox_->value());
	}
}

void CLSSIS3820ScalerControlsView::setTotalScans()
{
	if (scaler_ && scaler_->isConnected()) {
		scaler_->setTotalScans(totalScansBox_->value());
	}
}

void CLSSIS3820ScalerControlsView::updateStartButton()
{
	startButton_->setEnabled(false);

	if (scaler_ && scaler_->isConnected()) {
		if (!scaler_->isScanning())
			startButton_->setEnabled(true);
	}
}

void CLSSIS3820ScalerControlsView::updateStopButton()
{
	stopButton_->setEnabled(false);

	if (scaler_ && scaler_->isConnected()) {
		if (scaler_->isScanning() && !scaler_->isContinuous()) // The stop button shouldn't be enabled when the scaler is in continuous mode.
			stopButton_->setEnabled(true);
	}
}

void CLSSIS3820ScalerControlsView::updateStatusLabel()
{
	statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(25));

	if (scaler_ && scaler_->isConnected()) {
		if (scaler_->isScanning())
			statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOn.png").pixmap(25));
	}
}

void CLSSIS3820ScalerControlsView::updateAcquisitionModeBox()
{
	acquisitionModeBox_->clear();
	acquisitionModeBox_->setEnabled(false);

	if (scaler_ && scaler_->isConnected()) {
		acquisitionModeBox_->setEnabled(true);

		acquisitionModeBox_->blockSignals(true);
		acquisitionModeBox_->addItem("Single shot", CLSSIS3820Scaler::SingleShot);
		acquisitionModeBox_->addItem("Continuous", CLSSIS3820Scaler::Continuous);
		acquisitionModeBox_->setCurrentIndex(acquisitionModeBox_->findData(scaler_->isContinuous() ? CLSSIS3820Scaler::Continuous : CLSSIS3820Scaler::SingleShot));
		acquisitionModeBox_->blockSignals(false);
	}
}

void CLSSIS3820ScalerControlsView::updateDwellTimeBox()
{
	dwellTimeBox_->clear();
	dwellTimeBox_->setEnabled(false);

	if (scaler_ && scaler_->isConnected()) {

		if (!scaler_->isContinuous())
			dwellTimeBox_->setEnabled(true);

		dwellTimeBox_->blockSignals(true);
		dwellTimeBox_->setRange(0, 1000000);
		dwellTimeBox_->setSuffix(" ms");
		dwellTimeBox_->setValue(int(scaler_->dwellTime() * 1000)); // The box displays time in ms, scaler dwell is in s.
		dwellTimeBox_->blockSignals(false);
	}
}

void CLSSIS3820ScalerControlsView::updateScansPerBufferBox()
{
	scansPerBufferBox_->clear();
	scansPerBufferBox_->setEnabled(false);

	if (scaler_ && scaler_->isConnected()) {

		if (!scaler_->isContinuous())
			scansPerBufferBox_->setEnabled(true);

		scansPerBufferBox_->blockSignals(true);
		scansPerBufferBox_->setRange(1, 10000); // The scaler PV will not accept values lower than 1.
		scansPerBufferBox_->setValue(scaler_->scansPerBuffer());
		scansPerBufferBox_->blockSignals(false);
	}
}

void CLSSIS3820ScalerControlsView::updateTotalScansBox()
{
	totalScansBox_->clear();
	totalScansBox_->setEnabled(false);

	if (scaler_ && scaler_->isConnected()) {

		if (!scaler_->isContinuous())
			totalScansBox_->setEnabled(true);

		totalScansBox_->blockSignals(true);
		totalScansBox_->setRange(0, 10000);
		totalScansBox_->setValue(scaler_->totalScans());
		totalScansBox_->blockSignals(false);
	}
}

void CLSSIS3820ScalerControlsView::onScalerScanningStateChanged()
{
	updateStatusLabel();
	updateStartButton();
	updateStopButton();
}

void CLSSIS3820ScalerControlsView::onScalerContinuousStateChanged()
{
	updateAcquisitionModeBox();

	updateDwellTimeBox();
	updateScansPerBufferBox();
	updateTotalScansBox();
}
