#include "BioXASSIS3820ScalerView.h"
#include "beamline/BioXAS/BioXASSIS3820Scaler.h"
#include "ui/CLS/CLSSIS3820ScalerControlsView.h"
#include "ui/CLS/CLSSIS3820ScalerDarkCurrentWidget.h"
#include "ui/BioXAS/BioXASSIS3820ScalerModesView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"

BioXASSIS3820ScalerView::BioXASSIS3820ScalerView(CLSSIS3820Scaler *scaler, bool optionsVisible, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	scaler_ = scaler;

	// Create UI elements.

	CLSSIS3820ScalerControlsView *controlsView = new CLSSIS3820ScalerControlsView(scaler_);

	controlsBox_ = new QGroupBox();
	controlsBox_->setTitle("Controls");

	QCheckBox *biasEnabledVisible = new QCheckBox("Bias voltage enabled");
	QCheckBox *biasVisible = new QCheckBox("Bias voltage");
	QCheckBox *darkCurrentVisible = new QCheckBox("Dark current");
	QCheckBox *modesVisible = new QCheckBox("Modes");

	optionsBox_ = new QGroupBox();
	optionsBox_->setTitle("View Options");

	BioXASSIS3820ScalerModesView *modesView = new BioXASSIS3820ScalerModesView(qobject_cast<BioXASSIS3820Scaler*>(scaler_));

	modesBox_ = new QGroupBox();
	modesBox_->setTitle("Modes");

	BioXASSIS3820ScalerChannelsView *channelsView = new BioXASSIS3820ScalerChannelsView(scaler_, false, false, false);

	channelsBox_ = new QGroupBox();
	channelsBox_->setTitle("Channels");

	CLSSIS3820ScalerDarkCurrentWidget *darkCurrentWidget = new CLSSIS3820ScalerDarkCurrentWidget(scaler_);

	darkCurrentBox_ = new QGroupBox();
	darkCurrentBox_->setTitle("Dark current");

	// Create and set layouts.

	QHBoxLayout *controlsLayout = new QHBoxLayout();
	controlsLayout->addStretch();
	controlsLayout->addWidget(controlsView);
	controlsLayout->addStretch();

	controlsBox_->setLayout(controlsLayout);

	QHBoxLayout *optionsLayout = new QHBoxLayout();
	optionsLayout->addStretch();
	optionsLayout->addWidget(biasEnabledVisible);
	optionsLayout->addWidget(biasVisible);
	optionsLayout->addWidget(darkCurrentVisible);
	optionsLayout->addWidget(modesVisible);
	optionsLayout->addStretch();

	optionsBox_->setLayout(optionsLayout);

	QHBoxLayout *modesLayout = new QHBoxLayout();
	modesLayout->addStretch();
	modesLayout->addWidget(modesView);
	modesLayout->addStretch();

	modesBox_->setLayout(modesLayout);

	QVBoxLayout *channelsLayout = new QVBoxLayout();
	channelsLayout->addWidget(channelsView);

	channelsBox_->setLayout(channelsLayout);

	QHBoxLayout *darkCurrentLayout = new QHBoxLayout();
	darkCurrentLayout->addStretch();
	darkCurrentLayout->addWidget(darkCurrentWidget);
	darkCurrentLayout->addStretch();

	darkCurrentBox_->setLayout(darkCurrentLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(controlsBox_);
	layout->addWidget(optionsBox_);
	layout->addWidget(modesBox_);
	layout->addWidget(channelsBox_);
	layout->addWidget(darkCurrentBox_);

	setLayout(layout);

	// Initial settings.

	biasEnabledVisible->setChecked(false);
	biasVisible->setChecked(false);
	darkCurrentVisible->setChecked(false);
	modesVisible->setChecked(false);

	optionsBox_->hide();
	modesBox_->hide();
	channelsBox_->hide();
	darkCurrentBox_->hide();

	// Make connections.

	connect( modesVisible, SIGNAL(clicked(bool)), this, SLOT(setModeBoxVisible(bool)) );
	connect( biasEnabledVisible, SIGNAL(clicked(bool)), channelsView, SLOT(setBiasEnabledEditorVisible(bool)) );
	connect( biasVisible, SIGNAL(clicked(bool)), channelsView, SLOT(setBiasEditorVisible(bool)) );
	connect( darkCurrentVisible, SIGNAL(clicked(bool)), channelsView, SLOT(setDarkCurrentVisible(bool)) );
	connect( darkCurrentVisible, SIGNAL(clicked(bool)), this, SLOT(setDarkCurrentVisible(bool)) );

	if (scaler_)
		connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(setChannelViewsVisible(bool)) );

	connect( channelsView, SIGNAL(biasEnabledEditorVisibleChanged(bool)), biasEnabledVisible, SLOT(setChecked(bool)) );
	connect( channelsView, SIGNAL(biasEditorVisibleChanged(bool)), biasVisible, SLOT(setChecked(bool)) );

	// Current settings.

	biasEnabledVisible->setChecked(channelsView->biasEnabledEditorVisible());
	biasVisible->setChecked(channelsView->biasEditorVisible());
	darkCurrentVisible->setChecked(channelsView->darkCurrentVisible());

	setOptionsVisible(optionsVisible);

	setChannelViewsVisible(true);
}

BioXASSIS3820ScalerView::~BioXASSIS3820ScalerView()
{

}

void BioXASSIS3820ScalerView::setOptionsVisible(bool isVisible)
{
	optionsBox_->setVisible(isVisible);
}

void BioXASSIS3820ScalerView::setModeBoxVisible(bool isVisible)
{
	modesBox_->setVisible(isVisible);
}

void BioXASSIS3820ScalerView::setChannelViewsVisible(bool isVisible)
{
	if (scaler_ && scaler_->isConnected() && isVisible)
		channelsBox_->setVisible(true);
	else
		channelsBox_->setVisible(false);
}

void BioXASSIS3820ScalerView::setDarkCurrentVisible(bool isVisible)
{
	if (scaler_ && isVisible)
		darkCurrentBox_->setVisible(true);
	else
		darkCurrentBox_->setVisible(false);
}
