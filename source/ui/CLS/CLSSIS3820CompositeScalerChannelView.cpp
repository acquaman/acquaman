#include "CLSSIS3820CompositeScalerChannelView.h"

#include <QAction>
#include <QMenu>

 CLSSIS3820CompositeScalerChannelView::~CLSSIS3820CompositeScalerChannelView(){}
CLSSIS3820CompositeScalerChannelView::CLSSIS3820CompositeScalerChannelView(CLSSIS3820ScalerChannel *channel1, CLSSIS3820ScalerChannel *channel2, QWidget *parent)
	: QWidget(parent)
{
	channel1_ = channel1;
	channel2_ = channel2;
	viewMode_ = Counts;
	compositeName_ = "";
	showCompositeView_ = true;

	enableBox_ = new QCheckBox;

	if(channel1_->isConnected() && channel1_->isEnabled())
		enableBox_->setChecked(true);

	connect(channel1_, SIGNAL(enabledChanged(bool)), enableBox_, SLOT(setChecked(bool)));
	connect(enableBox_, SIGNAL(toggled(bool)), channel1_, SLOT(setEnabled(bool)));

	channelName_ = new QLabel(channel1_->customChannelName());

	sr570View_ = 0;
	connect(channel1_, SIGNAL(sr570Attached()), this, SLOT(onNewSR570Attached()));

	if (channel1_->sr570() && channel2_->sr570()){

		sr570View_ = new CLSSR570CompositeView(channel1_->sr570(), channel2_->sr570());
		connect(sr570View_, SIGNAL(viewModeChanged(CLSSR570CompositeView::ViewMode)), this, SIGNAL(sr570ViewModeChanged(CLSSR570CompositeView::ViewMode)));
	}

	scalerOutput_ = new QToolButton;
	scalerOutput_->setAutoRaise(true);

	connect(channel1_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged()));
	connect(scalerOutput_, SIGNAL(clicked()), this, SLOT(onScalerOutputClicked()));

	statusLabel_ = new QLabel;
	statusLabel_->setPixmap(QIcon(":/OFF.png").pixmap(22));
	statusLabel_->setVisible(channel1_->voltageRange().isValid());
	connect(channel1_, SIGNAL(voltageRangeChanged(AMRange)), this, SLOT(updateStatusLabel()));

	if(channel1_->isConnected())
		onReadingChanged();

	channelLayout_ = new QHBoxLayout;
	channelLayout_->addWidget(enableBox_, 0, Qt::AlignLeft);
	channelLayout_->addWidget(channelName_, 0, Qt::AlignCenter);

	if (sr570View_)
		channelLayout_->addWidget(sr570View_, 0, Qt::AlignCenter);

	channelLayout_->addWidget(scalerOutput_, 0, Qt::AlignCenter);
	channelLayout_->addWidget(statusLabel_, 0, Qt::AlignRight);

	setLayout(channelLayout_);
	setVisible(!channel1_->customChannelName().isEmpty());

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
}

void CLSSIS3820CompositeScalerChannelView::onReadingChanged()
{
	if (viewMode_ == Counts && showCompositeView_)
		scalerOutput_->setText(QString("%1 counts").arg(channel1_->reading()+channel2_->reading()));

	else if (viewMode_ == Counts && !showCompositeView_)
		scalerOutput_->setText(QString("%1 counts\n%2 counts").arg(channel1_->reading()).arg(channel2_->reading()));

	else if (viewMode_ == Voltage)
		scalerOutput_->setText(QString("%1 V\n%2 V").arg(channel1_->voltage(), 0, 'f', 2).arg(channel2_->voltage(), 0, 'f', 2));

	updateStatusLabel();
}

void CLSSIS3820CompositeScalerChannelView::updateStatusLabel()
{
	if (channel1_->withinLinearRange()){

		statusLabel_->setPixmap(QIcon(":/ON.png").pixmap(22));
		statusLabel_->setToolTip("");
	}

	else if (channel1_->voltageTooLow()){

		statusLabel_->setPixmap(QIcon(":/RED.png").pixmap(22));
		statusLabel_->setToolTip(QString("The voltage is too low!\nIncrease the sensitivity until the voltage is higher than %1 V.").arg(channel1_->minimumVoltage()));
	}

	else if (channel1_->voltageTooHigh()){

		statusLabel_->setPixmap(QIcon(":/RED.png").pixmap(22));
		statusLabel_->setToolTip(QString("The voltage is too high!\nDecrease the sensitivity until the voltage is lower than %1 V.").arg(channel1_->maximumVoltage()));
	}
}

void CLSSIS3820CompositeScalerChannelView::onScalerOutputClicked()
{
	if (viewMode_ == Counts){

		viewMode_ = Voltage;
		disconnect(channel1_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged()));
		connect(channel1_, SIGNAL(voltageChanged(double)), this, SLOT(onReadingChanged()));
	}

	else if (viewMode_ == Voltage){

		viewMode_ = Counts;
		connect(channel1_, SIGNAL(readingChanged(int)), this, SLOT(onReadingChanged()));
		disconnect(channel1_, SIGNAL(voltageChanged(double)), this, SLOT(onReadingChanged()));
	}

	emit outputViewModeChanged(viewMode_);
	onReadingChanged();
}

void CLSSIS3820CompositeScalerChannelView::setSR570ViewMode(CLSSR570CompositeView::ViewMode mode)
{
	if (sr570View_)
		sr570View_->setViewMode(mode);
}

void CLSSIS3820CompositeScalerChannelView::setOutputViewMode(CLSSIS3820CompositeScalerChannelView::OutputViewMode mode)
{
	if (viewMode_ != mode){

		viewMode_ = mode;
		onReadingChanged();
		emit outputViewModeChanged(viewMode_);
	}
}

void CLSSIS3820CompositeScalerChannelView::setEnableCheckBoxVisibility(bool visible)
{
	enableBox_->setVisible(visible);
}

void CLSSIS3820CompositeScalerChannelView::setCustomNameVisibility(bool visible)
{
	channelName_->setVisible(visible);
}

void CLSSIS3820CompositeScalerChannelView::setSR570Visibility(bool visible)
{
	if (sr570View_)
		sr570View_->setVisible(visible);
}

void CLSSIS3820CompositeScalerChannelView::setOutputVisibility(bool visible)
{
	scalerOutput_->setVisible(visible);
}

void CLSSIS3820CompositeScalerChannelView::setStatusLabelVisibility(bool visible)
{
	statusLabel_->setVisible(visible);
}

void CLSSIS3820CompositeScalerChannelView::onNewSR570Attached()
{
	// If one already exists, lets get rid of it before doing anything else.
	if (sr570View_)
		delete channelLayout_->takeAt(channelLayout_->indexOf(sr570View_));

	if (channel1_->sr570() && channel2_->sr570()){

		sr570View_ = new CLSSR570CompositeView(channel1_->sr570(), channel2_->sr570());
		channelLayout_->insertWidget(2, sr570View_, 0, Qt::AlignCenter);
	}
}

void CLSSIS3820CompositeScalerChannelView::updateCompositeNameLabel()
{
	if (showCompositeView_)
		channelName_->setText(compositeName_);

	else
		channelName_->setText(QString("%1\n%2").arg(channel1_->customChannelName()).arg(channel2_->customChannelName()));
}

void CLSSIS3820CompositeScalerChannelView::setCustomCompositeName(const QString &name)
{
	compositeName_ = name;
	updateCompositeNameLabel();
}

void CLSSIS3820CompositeScalerChannelView::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup(this);

	QAction *temp = popup.addAction(showCompositeView_ ? "Show Individual View" : "Show Combined View");

	temp = popup.addAction("Basic View");
	temp->setDisabled(sr570View_->viewMode() == CLSSR570CompositeView::Basic);

	temp = popup.addAction("Advanced View");
	temp->setDisabled(sr570View_->viewMode() == CLSSR570CompositeView::Advanced);

	temp = popup.exec(mapToGlobal(pos));

	// If a valid action was selected.
	if (temp){

		if (temp->text() == "Show Individual View" || temp->text() == "Show Combined View"){

			showCompositeView_ = !showCompositeView_;
			updateCompositeNameLabel();
		}

		else if (temp->text() == "Basic View")
			setSR570ViewMode(CLSSR570CompositeView::Basic);

		else if (temp->text() == "Advanced View")
			setSR570ViewMode(CLSSR570CompositeView::Advanced);
	}
}
