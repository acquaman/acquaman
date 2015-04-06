#include "BioXASSIS3820ScalerView.h"
#include "ui/CLS/CLSSIS3820ScalerControlsView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"
#include "ui/CLS/CLSSIS3820ScalerDarkCurrentWidget.h"

BioXASSIS3820ScalerView::BioXASSIS3820ScalerView(CLSSIS3820Scaler *scaler, bool optionsShown, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	scaler_ = scaler;

	// Create UI elements.

	CLSSIS3820ScalerControlsView *controlsView = new CLSSIS3820ScalerControlsView(scaler_);

	controlsBox_ = new QGroupBox();
	controlsBox_->setTitle("Controls");

	QCheckBox *biasEnabledShown = new QCheckBox("Bias voltage enabled");
	QCheckBox *biasShown = new QCheckBox("Bias voltage");
	QCheckBox *darkCurrentShown = new QCheckBox("Dark current");

	optionsBox_ = new QGroupBox();
	optionsBox_->setTitle("Options");

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
	optionsLayout->addWidget(biasEnabledShown);
	optionsLayout->addWidget(biasShown);
	optionsLayout->addWidget(darkCurrentShown);
	optionsLayout->addStretch();

	optionsBox_->setLayout(optionsLayout);

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
	layout->addWidget(channelsBox_);
	layout->addWidget(darkCurrentBox_);

	setLayout(layout);

	// Initial settings.

	biasEnabledShown->setChecked(false);
	biasShown->setChecked(false);
	darkCurrentShown->setChecked(false);

	optionsBox_->hide();
	channelsBox_->hide();
	darkCurrentBox_->hide();

	// Make connections.

	connect( biasEnabledShown, SIGNAL(clicked(bool)), channelsView, SLOT(setBiasEnabledEditorShown(bool)) );
	connect( biasShown, SIGNAL(clicked(bool)), channelsView, SLOT(setBiasEditorShown(bool)) );
	connect( darkCurrentShown, SIGNAL(clicked(bool)), channelsView, SLOT(setDarkCurrentShown(bool)) );
	connect( darkCurrentShown, SIGNAL(clicked(bool)), this, SLOT(showDarkCurrentWidget(bool)) );

	if (scaler_)
		connect( scaler_, SIGNAL(connectedChanged(bool)), this, SLOT(showChannelViews(bool)) );

	connect( channelsView, SIGNAL(biasEnabledEditorShownChanged(bool)), biasEnabledShown, SLOT(setChecked(bool)) );
	connect( channelsView, SIGNAL(biasEditorShownChanged(bool)), biasShown, SLOT(setChecked(bool)) );

	// Current settings.

	biasEnabledShown->setChecked(channelsView->biasEnabledEditorShown());
	biasShown->setChecked(channelsView->biasEditorShown());
	darkCurrentShown->setChecked(channelsView->darkCurrentShown());

	showOptions(optionsShown);

	showChannelViews(true);
}

BioXASSIS3820ScalerView::~BioXASSIS3820ScalerView()
{

}

void BioXASSIS3820ScalerView::showOptions(bool shown)
{
	optionsBox_->setShown(shown);
}

void BioXASSIS3820ScalerView::showChannelViews(bool shown)
{
	if (scaler_ && scaler_->isConnected() && shown)
		channelsBox_->setShown(true);
	else
		channelsBox_->setShown(false);
}

void BioXASSIS3820ScalerView::showDarkCurrentWidget(bool shown)
{
	if (scaler_ && shown)
		darkCurrentBox_->setShown(true);
	else
		darkCurrentBox_->setShown(false);
}
