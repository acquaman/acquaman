#include "CLSAMDSScalerView.h"

#include "beamline/CLS/CLSAMDSScaler.h"

// CLSAMDSScalerChannelView
//////////////////////////////////////////////////////////////

CLSAMDSScalerChannelView::CLSAMDSScalerChannelView(CLSAMDSScalerChannel *channel, QWidget *parent)
	: QWidget(parent)
{
	channel_ = channel;
	viewMode_ = Counts;

	enableBox_ = new QCheckBox;

	if(channel_->isConnected() && channel_->isEnabled())
		enableBox_->setChecked(true);

	connect(channel_, SIGNAL(enabledChanged(bool)), enableBox_, SLOT(setChecked(bool)));
	connect(enableBox_, SIGNAL(toggled(bool)), channel_, SLOT(setEnabled(bool)));

	channelName_ = new QLabel(channel_->customChannelName());
	channelName_->setFixedWidth(80);

	amplifierView_ = 0;

	connect(channel_, SIGNAL(currentAmplifierAttached()), this, SLOT(onNewCurrentAmplifierAttached()));

	if (channel_->currentAmplifier()){

		amplifierView_ = channel_->currentAmplifier()->createView();
		connect( amplifierView_, SIGNAL(viewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)) );

	}

	scalerOutput_ = new QToolButton;
	scalerOutput_->setMaximumWidth(100);
	scalerOutput_->setAutoRaise(true);


	connect(channel_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged()));
	connect(scalerOutput_, SIGNAL(clicked()), this, SLOT(onScalerOutputClicked()));

	statusLabel_ = new QLabel;
	statusLabel_->setPixmap(QIcon(":/22x22/greenLEDOff.png").pixmap(22));
	connect(channel_, SIGNAL(voltageRangeChanged(AMRange)), this, SLOT(updateStatusLabel()));

	if(channel_->isConnected())
		onReadingChanged();

	channelLayout_ = new QHBoxLayout;
	channelLayout_->addWidget(enableBox_, 0, Qt::AlignLeft);
	channelLayout_->addWidget(channelName_, 0, Qt::AlignCenter);

	if (amplifierView_)
		channelLayout_->addWidget(amplifierView_, 0, Qt::AlignCenter);

	channelLayout_->addWidget(scalerOutput_, 0, Qt::AlignCenter);
	channelLayout_->addWidget(statusLabel_, 0, Qt::AlignRight);
	channelLayout_->setContentsMargins(1, 1, 1, 1);

	setLayout(channelLayout_);

	statusLabel_->setVisible(channel_->voltageRange().isValid());
}

CLSAMDSScalerChannelView::~CLSAMDSScalerChannelView(){}

bool CLSAMDSScalerChannelView::hasAmplifierView() const
{
	return amplifierView_ != 0;
}

void CLSAMDSScalerChannelView::onReadingChanged()
{
	if (viewMode_ == Counts)
		scalerOutput_->setText(QString("%1 counts").arg(channel_->reading()));

	else if (viewMode_ == Voltage)
		scalerOutput_->setText(QString("%1 V").arg(channel_->voltage(), 0, 'f', 2));

	updateStatusLabel();
}

void CLSAMDSScalerChannelView::updateStatusLabel()
{
	if (channel_->withinLinearRange()){

		statusLabel_->setPixmap(QIcon(":/22x22/greenLEDOn.png").pixmap(22));
		statusLabel_->setToolTip("");
	}

	else if (channel_->voltageTooLow()){

		statusLabel_->setPixmap(QIcon(":/22x22/redLEDOn.png").pixmap(22));
		statusLabel_->setToolTip(QString("The voltage is too low!\nIncrease the sensitivity until the voltage is higher than %1 V.").arg(channel_->minimumVoltage()));
	}

	else if (channel_->voltageTooHigh()){

		statusLabel_->setPixmap(QIcon(":/22x22/redLEDOn.png").pixmap(22));
		statusLabel_->setToolTip(QString("The voltage is too high!\nDecrease the sensitivity until the voltage is lower than %1 V.").arg(channel_->maximumVoltage()));
	}
}

void CLSAMDSScalerChannelView::onScalerOutputClicked()
{
	if (viewMode_ == Counts){

		viewMode_ = Voltage;
		disconnect(channel_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged()));
		connect(channel_, SIGNAL(voltageChanged(double)), this, SLOT(onReadingChanged()));
	}

	else if (viewMode_ == Voltage){

		viewMode_ = Counts;
		connect(channel_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged()));
		disconnect(channel_, SIGNAL(voltageChanged(double)), this, SLOT(onReadingChanged()));
	}

	emit outputViewModeChanged(viewMode_);
	onReadingChanged();
}

void CLSAMDSScalerChannelView::setAmplifierViewMode(AMCurrentAmplifierView::ViewMode newMode)
{
	if (channel_->currentAmplifier())
		amplifierView_->setViewMode(newMode);
}

void CLSAMDSScalerChannelView::setOutputViewMode(CLSAMDSScalerChannelView::OutputViewMode mode)
{
	if (viewMode_ != mode){

		viewMode_ = mode;
		onReadingChanged();
		emit outputViewModeChanged(viewMode_);
	}
}

void CLSAMDSScalerChannelView::setEnableCheckBoxVisibility(bool visible)
{
	enableBox_->setVisible(visible);
}

void CLSAMDSScalerChannelView::setCustomNameVisibility(bool visible)
{
	channelName_->setVisible(visible);
}

void CLSAMDSScalerChannelView::setSR570Visibility(bool visible)
{
	if (amplifierView_)
		amplifierView_->setVisible(visible);
}

void CLSAMDSScalerChannelView::setOutputVisibility(bool visible)
{
	scalerOutput_->setVisible(visible);
}

void CLSAMDSScalerChannelView::setStatusLabelVisibility(bool visible)
{
	statusLabel_->setVisible(visible);
}

void CLSAMDSScalerChannelView::onNewCurrentAmplifierAttached()
{
	// If one already exists, lets get rid of it before doing anything else.
	if (amplifierView_)
		delete channelLayout_->takeAt(channelLayout_->indexOf(amplifierView_));

	amplifierView_ = channel_->currentAmplifier()->createView();
	channelLayout_->insertWidget(2, amplifierView_, 0, Qt::AlignCenter);
}

void CLSAMDSScalerChannelView::setAmplifierViewPrecision(int newPrecision)
{
	if(amplifierView_)
		amplifierView_->setPrecision(newPrecision);
}

void CLSAMDSScalerChannelView::setAmplifierViewFormat(char newFormat)
{
	if(amplifierView_)
		amplifierView_->setFormat(newFormat);
}

// CLSAMDSScalerView
///////////////////////////////////////////////

CLSAMDSScalerView::CLSAMDSScalerView(CLSAMDSScaler *scaler, QWidget *parent) :
	QWidget(parent)
{
	scaler_ = scaler;

	// Build the top scaler controls.
	controlsView_ = new CLSAMDSScalerControlsView(scaler_);

	// Build the channel views.
	channelLayout_ = new QVBoxLayout;

	mainVL_ = new QVBoxLayout();
	mainVL_->addWidget(controlsView_);
	mainVL_->addLayout(channelLayout_);

	setLayout(mainVL_);

	CLSAMDSScalerChannelView *channelView = 0;
	int channelCount = scaler_->channels().count();

	for (int i = 0; i < channelCount; i++) {
		channelView = new CLSAMDSScalerChannelView(scaler_->channelAt(i));
		channelViews_ << channelView;

		connect(channelView, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SLOT(onAmplifierViewChanged(AMCurrentAmplifierView::ViewMode)));
		connect(channelView, SIGNAL(outputViewModeChanged(CLSAMDSScalerChannelView::OutputViewMode)), this, SLOT(onOutputViewModeChanged(CLSAMDSScalerChannelView::OutputViewMode)));

		channelLayout_->addWidget(channelView);
		channelView->setVisible(!scaler_->channelAt(i)->customChannelName().isEmpty());
	}
}

CLSAMDSScalerView::~CLSAMDSScalerView(){}

void CLSAMDSScalerView::onAmplifierViewChanged(AMCurrentAmplifierView::ViewMode mode)
{
	foreach (CLSAMDSScalerChannelView *channel, channelViews_){

		channel->blockSignals(true);
		channel->setAmplifierViewMode(mode);
		channel->blockSignals(false);
	}
}

void CLSAMDSScalerView::onOutputViewModeChanged(CLSAMDSScalerChannelView::OutputViewMode mode)
{
	foreach (CLSAMDSScalerChannelView *channel, channelViews_){

		channel->blockSignals(true);
		channel->setOutputViewMode(mode);
		channel->blockSignals(false);
	}
}

void CLSAMDSScalerView::setAmplifierViewPrecision(int newPrecision)
{
	foreach (CLSAMDSScalerChannelView *view, channelViews_)
	{
		if(view->hasAmplifierView())
			view->setAmplifierViewPrecision(newPrecision);
	}
}

void CLSAMDSScalerView::setAmplifierViewFormat(char newFormat)
{
	foreach (CLSAMDSScalerChannelView *view, channelViews_)
	{
		if(view->hasAmplifierView())
			view->setAmplifierViewFormat(newFormat);
	}
}


// CLSAMDSScalerChannelView
//////////////////////////////////////////////////////////////

CLSAMDSScalerControlsView::CLSAMDSScalerControlsView(CLSAMDSScaler *scaler, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	scaler_ = 0;

	// Create UI elements.

	modeChoice_ = new QComboBox;
	modeChoice_->addItems(QStringList() << "Single Shot" << "Continuous");
	modeChoice_->setCurrentIndex(0);

	startButton_ = new QPushButton(QIcon(":/22x22/media-playback-start.png"), "Start");
	startButton_->setMaximumHeight(25);

	stopButton_ = new QPushButton(QIcon(":/22x22/media-playback-stop.png"), "Stop");
	stopButton_->setMaximumHeight(25);

	status_ = new QLabel;
	status_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(25));

	time_ = new QDoubleSpinBox;
	time_->setRange(0.2, 100);
	time_->setValue(1000);
	time_->setSuffix(" s");
	time_->setFixedWidth(100);
	time_->setAlignment(Qt::AlignCenter);

	// Create and set layouts.

	QHBoxLayout *startAndStopButtons = new QHBoxLayout;
	startAndStopButtons->addWidget(startButton_);
	startAndStopButtons->addWidget(stopButton_);

	QVBoxLayout *statusAndModeLayout = new QVBoxLayout;
	statusAndModeLayout->addWidget(status_, 0, Qt::AlignCenter);
	statusAndModeLayout->addLayout(startAndStopButtons);
	statusAndModeLayout->addWidget(modeChoice_, 0, Qt::AlignCenter);

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"), 0, Qt::AlignRight);
	timeLayout->addWidget(time_);

	QVBoxLayout *spinBoxLayout = new QVBoxLayout;
	spinBoxLayout->addLayout(timeLayout);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addLayout(statusAndModeLayout);
	layout->addLayout(spinBoxLayout);

	setLayout(layout);

	// Make connections.

	connect(startButton_, SIGNAL(clicked()), this, SLOT(startScanning()));
	connect(stopButton_, SIGNAL(clicked()), this, SLOT(stopScanning()));
	connect(modeChoice_, SIGNAL(currentIndexChanged(int)), this, SLOT(setDwellMode()));
	connect(time_, SIGNAL(editingFinished()), this, SLOT(setDwellTime()));

	// Current settings.

	setScaler(scaler);
}

CLSAMDSScalerControlsView::~CLSAMDSScalerControlsView()
{

}

void CLSAMDSScalerControlsView::setScaler(CLSAMDSScaler *newScaler)
{
	if (scaler_ != newScaler) {

		if (scaler_) {
			disconnect( scaler_, 0, this, 0 );
		}

		scaler_ = newScaler;

		if (scaler_) {
			connect(scaler_, SIGNAL(dwellModeChanged(double)), this, SLOT(onDwellModeChanged()));
			connect(scaler_, SIGNAL(dwellTimeChanged(double)), this, SLOT(onDwellTimeChanged()));
			connect(scaler_, SIGNAL(dwellStateChanged(double)), this, SLOT(onDwellStateChanged()));
			connect(scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected(bool)));
		}

		if(scaler_->isConnected())
			onScalerConnected(true);
	}
}

void CLSAMDSScalerControlsView::startScanning()
{
	if (scaler_)
		scaler_->setStarted(true);
}

void CLSAMDSScalerControlsView::stopScanning()
{
	if (scaler_)
		scaler_->setStarted(false);
}

void CLSAMDSScalerControlsView::setDwellMode()
{
	if (scaler_) {
		int index = modeChoice_->currentIndex();

		if (index == 1 && (fabs(scaler_->dwellMode()-0.0) < 0.1) )
			scaler_->setDwellMode(1.0);

		else if (index == 0 && (fabs(scaler_->dwellMode()-1.0) < 0.1) )
			scaler_->setDwellMode(0.0);
	}
}

void CLSAMDSScalerControlsView::setDwellTime()
{
	if (scaler_) {
		double secondsDwell = time_->value();

		if (secondsDwell != scaler_->dwellTime())
			scaler_->setDwellTime(secondsDwell);
	}
}

void CLSAMDSScalerControlsView::onDwellModeChanged()
{
	if (scaler_) {
		scaler_->blockSignals(true);
		modeChoice_->setCurrentIndex(scaler_->dwellMode());
		scaler_->blockSignals(false);
	}
}

void CLSAMDSScalerControlsView::onDwellTimeChanged()
{
	if (scaler_) {
		scaler_->blockSignals(true);
		time_->setValue(scaler_->dwellTime());
		scaler_->blockSignals(false);
	}
}

void CLSAMDSScalerControlsView::onDwellStateChanged()
{
	if (scaler_ && (fabs(scaler_->dwellState() - 1.0) < 0.1)) {

		startButton_->setEnabled(false);
		stopButton_->setEnabled(true);
		status_->setPixmap(QIcon(":/32x32/greenLEDOn.png").pixmap(25));

	} else if (scaler_ && (fabs(scaler_->dwellState() - 0.0) < 0.1)) {

		startButton_->setEnabled(true);
		stopButton_->setEnabled(false);
		status_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(25));

	} else {

		startButton_->setEnabled(false);
		stopButton_->setEnabled(false);
		status_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(25));
	}
}

void CLSAMDSScalerControlsView::onScalerConnected(bool connected)
{
	Q_UNUSED(connected)
	onDwellModeChanged();
	onDwellTimeChanged();
	onDwellStateChanged();
}
