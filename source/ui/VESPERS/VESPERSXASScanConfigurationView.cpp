#include "VESPERSXASScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/AMTopFrame.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QToolButton>

VESPERSXASScanConfigurationView::VESPERSXASScanConfigurationView(VESPERSXASScanConfiguration *config, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS XAS Configuration");

	// Regions setup
	regionsView_ = new AMXASRegionsView(config_->regions());
	regionsView_->setBeamlineEnergy(VESPERSBeamline::vespers()->energyRelative());

	regionsLineView_ = new AMRegionsLineView(config_->regions());

	// The fluorescence detector setup
	QButtonGroup *fluorescenceButtonGroup = new QButtonGroup;
	QRadioButton *tempButton;
	QVBoxLayout *fluorescenceDetectorLayout = new QVBoxLayout;

	tempButton = new QRadioButton("None");
	fluorescenceButtonGroup->addButton(tempButton, 0);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Single Element Vortex");
	fluorescenceButtonGroup->addButton(tempButton, 1);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton = new QRadioButton("Four Element Vortex");
	fluorescenceButtonGroup->addButton(tempButton, 2);
	fluorescenceDetectorLayout->addWidget(tempButton);
	tempButton->setChecked(true);
	connect(fluorescenceButtonGroup, SIGNAL(buttonClicked(int)), config_, SLOT(setFluorescenceDetectorChoice(int)));

	QGroupBox *fluorescenceDetectorGroupBox = new QGroupBox("Fluorescence Detector");
	fluorescenceDetectorGroupBox->setLayout(fluorescenceDetectorLayout);

	// Ion chamber selection
	ItGroup_ = new QButtonGroup;

	I0Group_ = new QButtonGroup;

	QButtonGroup *ItI0ButtonGroup = new QButtonGroup;
	QToolButton *ItI0temp = new QToolButton;
	ItI0temp->setText("It");
	ItI0temp->setCheckable(true);
	ItI0ButtonGroup->addButton(ItI0temp, 0);
	ItI0temp = new QToolButton;
	ItI0temp->setText("I0");
	ItI0temp->setCheckable(true);
	ItI0ButtonGroup->addButton(ItI0temp, 1);
	ItI0temp->setChecked(true);
	connect(ItI0ButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onItI0Toggled(int)));

	// Scan name selection
	scanName_ = new QLineEdit;
	scanName_->setText("XAS-Scan");
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(regionsLineView_, 0, 0, 1, 4, Qt::AlignCenter);
	contentsLayout->addWidget(regionsView_, 1, 0, 1, 3);
	contentsLayout->addWidget(fluorescenceDetectorGroupBox, 1, 3, 1, 1);
	contentsLayout->addLayout(scanNameLayout, 2, 0, Qt::AlignLeft);

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(contentsLayout);
	configViewLayout->addStretch();

	setLayout(configViewLayout);
}
