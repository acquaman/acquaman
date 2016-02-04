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
	statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(25));

	dwellTimeBox_ = new QSpinBox;
	dwellTimeBox_->setRange(0, 1000000);
	dwellTimeBox_->setValue(1000);
	dwellTimeBox_->setSuffix(" ms");
	dwellTimeBox_->setFixedWidth(100);
	dwellTimeBox_->setAlignment(Qt::AlignCenter);

	scansPerBuffer_ = new QSpinBox;
	scansPerBuffer_->setRange(0, 10000);
	scansPerBuffer_->setValue(1);
	scansPerBuffer_->setFixedWidth(100);
	scansPerBuffer_->setAlignment(Qt::AlignCenter);

	totalScans_ = new QSpinBox;
	totalScans_->setRange(0, 10000);
	totalScans_->setValue(1);
	totalScans_->setFixedWidth(100);
	totalScans_->setAlignment(Qt::AlignCenter);

	// Create and set layouts.

	QHBoxLayout *startAndStopButtons = new QHBoxLayout;
	startAndStopButtons->addWidget(startButton_);
	startAndStopButtons->addWidget(stopButton_);

	QVBoxLayout *statusAndModeLayout = new QVBoxLayout;
	statusAndModeLayout->addWidget(statusLabel_, 0, Qt::AlignCenter);
	statusAndModeLayout->addLayout(startAndStopButtons);
	statusAndModeLayout->addWidget(acquisitionModeBox_, 0, Qt::AlignCenter);

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"), 0, Qt::AlignRight);
	timeLayout->addWidget(dwellTimeBox_);

	QHBoxLayout *scansPerBufferLayout = new QHBoxLayout;
	scansPerBufferLayout->addWidget(new QLabel("Scans per Buffer:"), 0, Qt::AlignRight);
	scansPerBufferLayout->addWidget(scansPerBuffer_);

	QHBoxLayout *totalScansLayout = new QHBoxLayout;
	totalScansLayout->addWidget(new QLabel("Total Scans"), 0, Qt::AlignRight);
	totalScansLayout->addWidget(totalScans_);

	QVBoxLayout *spinBoxLayout = new QVBoxLayout;
	spinBoxLayout->addLayout(timeLayout);
	spinBoxLayout->addLayout(scansPerBufferLayout);
	spinBoxLayout->addLayout(totalScansLayout);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addLayout(statusAndModeLayout);
	layout->addLayout(spinBoxLayout);

	setLayout(layout);

	// Make connections.

	connect(startButton_, SIGNAL(clicked()), this, SLOT(startScanning()));
	connect(stopButton_, SIGNAL(clicked()), this, SLOT(stopScanning()));
	connect(acquisitionModeBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(setContinuous()));
	connect(dwellTimeBox_, SIGNAL(editingFinished()), this, SLOT(setDwellTime()));
	connect(scansPerBuffer_, SIGNAL(editingFinished()), this, SLOT(setScansPerBuffer()));
	connect(totalScans_, SIGNAL(editingFinished()), this, SLOT(setTotalScans()));

	// Current settings.

	setScaler(scaler);
}

CLSSIS3820ScalerControlsView::~CLSSIS3820ScalerControlsView()
{

}

void CLSSIS3820ScalerControlsView::refresh()
{
	updateStartStopButtons();
	updateStatusLabel();
	updateAcquisitionModeBox();
	onDwellTimeChanged();
	onScansPerBufferChanged();
	onTotalScansChanged();
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
			connect( scaler_, SIGNAL(scanningChanged(bool)), this, SLOT(updateStartStopButtons()) );
			connect( scaler_, SIGNAL(scanningChanged(bool)), this, SLOT(updateStatusLabel()) );
			connect( scaler_, SIGNAL(continuousChanged(bool)), this, SLOT(updateAcquisitionModeBox()));
			connect( scaler_, SIGNAL(dwellTimeChanged(double)), this, SLOT(onDwellTimeChanged()));
			connect( scaler_, SIGNAL(scansPerBufferChanged(int)), this, SLOT(onScansPerBufferChanged()));
			connect( scaler_, SIGNAL(totalScansChanged(int)), this, SLOT(onTotalScansChanged()));
		}

		refresh();
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
	if (scaler_) {
		int index = acquisitionModeBox_->currentIndex();

		if (index == 1 && !scaler_->isContinuous())
			scaler_->setContinuous(true);

		else if (index == 0 && scaler_->isContinuous())
			scaler_->setContinuous(false);
	}
}

void CLSSIS3820ScalerControlsView::setDwellTime()
{
	if (scaler_) {
		double secondsDwell = (double)dwellTimeBox_->value() / MILLISECONDS_PER_SECOND;

		if (secondsDwell != scaler_->dwellTime())
			scaler_->setDwellTime(secondsDwell);
	}
}

void CLSSIS3820ScalerControlsView::setScansPerBuffer()
{
	if (scaler_) {
		int newValue = scansPerBuffer_->value();

		if (newValue != scaler_->scansPerBuffer())
			scaler_->setScansPerBuffer(newValue);
	}
}

void CLSSIS3820ScalerControlsView::setTotalScans()
{
	if (scaler_) {
		int newValue = totalScans_->value();

		if (newValue != scaler_->totalScans())
			scaler_->setTotalScans(newValue);
	}
}

void CLSSIS3820ScalerControlsView::updateStartStopButtons()
{
	bool startEnabled = false;
	bool stopEnabled = false;

	if (scaler_ && scaler_->isConnected()) {
		if (scaler_->isScanning())
			stopEnabled = true;
		else
			startEnabled = true;
	}

	startButton_->setEnabled(startEnabled);
	stopButton_->setEnabled(stopEnabled);
}

void CLSSIS3820ScalerControlsView::updateStatusLabel()
{
	QIcon icon = QIcon(":/32x32/greenLEDOff.png");

	if (scaler_ && scaler_->isConnected()) {
		if (scaler_->isScanning())
			icon = QIcon(":/32x32/greenLEDOn.png");
	}

	statusLabel_->setPixmap(icon.pixmap(25));
}

void CLSSIS3820ScalerControlsView::updateAcquisitionModeBox()
{
	acquisitionModeBox_->clear();
	acquisitionModeBox_->setEnabled(false);

	if (scaler_ && scaler_->isConnected()) {
		acquisitionModeBox_->blockSignals(true);
		acquisitionModeBox_->setEnabled(true);
		acquisitionModeBox_->addItem("Single shot", CLSSIS3820Scaler::SingleShot);
		acquisitionModeBox_->addItem("Continuous", CLSSIS3820Scaler::Continuous);
		acquisitionModeBox_->setCurrentIndex(acquisitionModeBox_->findData(scaler_->isContinuous() ? CLSSIS3820Scaler::Continuous : CLSSIS3820Scaler::SingleShot));
		acquisitionModeBox_->blockSignals(false);
	}
}

void CLSSIS3820ScalerControlsView::onDwellTimeChanged()
{
	if (scaler_) {
		scaler_->blockSignals(true);
		double seconds = scaler_->dwellTime();
		dwellTimeBox_->setValue(int(seconds * MILLISECONDS_PER_SECOND));
		scaler_->blockSignals(false);
	}
}

void CLSSIS3820ScalerControlsView::onScansPerBufferChanged()
{
	if (scaler_) {
		scaler_->blockSignals(true);
		scansPerBuffer_->setValue(scaler_->scansPerBuffer());
		scaler_->blockSignals(false);
	}
}

void CLSSIS3820ScalerControlsView::onTotalScansChanged()
{
	if (scaler_) {
		scaler_->blockSignals(true);
		totalScans_->setValue(scaler_->totalScans());
		scaler_->blockSignals(false);
	}
}
