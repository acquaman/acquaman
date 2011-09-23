/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "XRFFreeRunView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"
#include "ui/AMTopFrame.h"
#include "util/VESPERS/GeneralUtilities.h"
#include "util/VESPERS/XRFElement.h"
#include "ui/VESPERS/XRFPeriodicTableView.h"
#include "ui/VESPERS/VESPERSXRFElementView.h"
#include "ui/util/AMPeriodicTableDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPushButton>

XRFFreeRunView::XRFFreeRunView(XRFFreeRun *xrfFreeRun, AMWorkflowManagerView *workflow, QWidget *parent)
	: QWidget(parent)
{
	xrfFreeRun_ = xrfFreeRun;
	workflow_ = workflow;
	detector_ = xrfFreeRun_->detector();
	xrfTable_ = xrfFreeRun_->table();

	AMTopFrame *topFrame = new AMTopFrame(QString("XRF Free Run - %1").arg(detector_->name()));
	topFrame->setIcon(QIcon(":/utilities-system-monitor.png"));

	connect(detector_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));

	// Build the detector view.
	view_ = new XRFDetailedDetectorView(detector_);
	view_->setMinimumEnergy(xrfTable_->minimumEnergy());
	view_->setMaximumEnergy(xrfTable_->maximumEnergy());
	view_->setROIMarkersHighlighted(true);
	view_->setEmissionLinesVisible(true);
	view_->setPileUpPeaksVisible(false);
	view_->setCombinationPileUpPeaksVisible(false);
	connect(detector_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
	connect(xrfTable_, SIGNAL(currentElementChanged(XRFElement*)), view_, SLOT(setCurrentElement(XRFElement*)));
	connect(xrfTable_, SIGNAL(minimumEnergyChanged(double)), view_, SLOT(setMinimumEnergy(double)));
	connect(xrfTable_, SIGNAL(maximumEnergyChanged(double)), view_, SLOT(setMaximumEnergy(double)));
	connect(xrfTable_, SIGNAL(removedAllRegionsOfInterest()), view_, SLOT(removeAllRegionsOfInterestMarkers()));

	// The periodic table view.
	XRFPeriodicTableView *tableView = new XRFPeriodicTableView(xrfTable_);
	QPalette palette = tableView->palette();
	palette.setColor(QPalette::Window, QColor(79, 148, 205));
	tableView->setPalette(palette);
	tableView->setAutoFillBackground(true);

	// The element view.
	VESPERSXRFElementView *elView = new VESPERSXRFElementView(xrfTable_->currentElement());
	palette = elView->palette();
	palette.setColor(QPalette::Window, QColor(110, 139, 61));
	elView->setPalette(palette);
	elView->setAutoFillBackground(true);
	elView->setMinimumEnergy(xrfTable_->minimumEnergy());
	elView->setMaximumEnergy(xrfTable_->maximumEnergy());
	connect(xrfTable_, SIGNAL(currentElementChanged(XRFElement*)), elView, SLOT(setElement(XRFElement*)));
	connect(xrfTable_, SIGNAL(minimumEnergyChanged(double)), elView, SLOT(setMinimumEnergy(double)));
	connect(xrfTable_, SIGNAL(maximumEnergyChanged(double)), elView, SLOT(setMaximumEnergy(double)));
	connect(elView, SIGNAL(addLine(QString)), xrfTable_, SLOT(addLineToList(QString)));
	connect(elView, SIGNAL(removeLine(QString)), xrfTable_, SLOT(removeLineFromList(QString)));

	QPushButton *sortButton = new QPushButton(QIcon(":/ArrowCCW.png"), "Sort");
	connect(sortButton, SIGNAL(clicked()), view_, SLOT(sortRegionsOfInterest()));

	// Customize the regions of interest.
	customize_ = new CustomizeRegionsOfInterest(detector_->roiList(), this);
	QPushButton *configureButton = new QPushButton(QIcon(":/configure.png"), "Edit ROIs");

	QScrollArea *scroll = new QScrollArea;
	scroll->setWidget(customize_);
	scroll->setMinimumWidth(480);

	connect(configureButton, SIGNAL(clicked()), scroll, SLOT(show()));

	// Control options for the detector.
	start_ = new QToolButton;
	start_->setIcon(QIcon(":/play_button_green.png"));
	connect(start_, SIGNAL(clicked()), this, SLOT(onStartClicked()));

	QToolButton *stop = new QToolButton;
	stop->setIcon(QIcon(":/red-stop-button.png"));
	connect(stop, SIGNAL(clicked()), this, SLOT(onStopClicked()));

	integrationTime_ = new QDoubleSpinBox;
	integrationTime_->setSuffix(" s");
	integrationTime_->setSingleStep(0.1);
	integrationTime_->setMaximum(1000.00);
	integrationTime_->setAlignment(Qt::AlignCenter);
	connect(integrationTime_, SIGNAL(editingFinished()), this, SLOT(onIntegrationTimeUpdate()));
	connect(detector_, SIGNAL(integrationTimeChanged(double)), integrationTime_, SLOT(setValue(double)));

	QPushButton *customizeSettings = new QPushButton(QIcon(":/hammer.png"), "Settings");
	customizeSettings->setCheckable(true);
	connect(customizeSettings, SIGNAL(toggled(bool)), this, SLOT(onAdvancedSettingsChanged(bool)));

	minEnergy_ = new QDoubleSpinBox;
	minEnergy_->hide();
	minEnergy_->setSuffix(" keV");
	minEnergy_->setSingleStep(0.01);
	minEnergy_->setMinimum(0.0);
	minEnergy_->setMaximum(30.00);
	minEnergy_->setValue(xrfTable_->minimumEnergy()/1000);
	minEnergy_->setAlignment(Qt::AlignCenter);
	connect(minEnergy_, SIGNAL(editingFinished()), this, SLOT(onMinimumEnergyUpdate()));

	maxEnergy_ = new QDoubleSpinBox;
	maxEnergy_->hide();
	maxEnergy_->setSuffix(" keV");
	maxEnergy_->setSingleStep(0.01);
	maxEnergy_->setMaximum(30.00);
	maxEnergy_->setAlignment(Qt::AlignCenter);
	connect(maxEnergy_, SIGNAL(editingFinished()), this, SLOT(onMaximumEnergyUpdate()));
	connect(detector_, SIGNAL(maximumEnergyChanged(double)), maxEnergy_, SLOT(setValue(double)));
	connect(detector_, SIGNAL(maximumEnergyChanged(double)), this, SLOT(onMaximumEnergyChanged(double)));

	peakingTime_ = new QDoubleSpinBox;
	peakingTime_->hide();
	peakingTime_->setSuffix(QString::fromUtf8(" μs"));
	peakingTime_->setSingleStep(0.01);
	peakingTime_->setMaximum(100);
	peakingTime_->setAlignment(Qt::AlignCenter);
	connect(peakingTime_, SIGNAL(editingFinished()), this, SLOT(onPeakingTimeUpdate()));
	connect(detector_, SIGNAL(peakingTimeChanged(double)), peakingTime_, SLOT(setValue(double)));

	// Set up GUI elements to enable/disable showing pile up peaks.
	QCheckBox *selfPileUpBox = new QCheckBox("Self");
	connect(selfPileUpBox, SIGNAL(toggled(bool)), this, SLOT(onShowSelfPileUpPeaks(bool)));

	selfPileUpLabel_ = new QLabel("Using: ");
	selfPileUpLabel_->hide();
	connect(xrfTable_, SIGNAL(currentElementChanged(XRFElement*)), this, SLOT(onCurrentElementChanged(XRFElement*)));

	combinationPeakCheckBox_ = new QCheckBox("Combination");
	combinationPeakCheckBox_->hide();
	connect(combinationPeakCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onShowCombinationPileUpPeaks(bool)));

	combinationPileUpLabel_ = new QLabel("Using: ");
	combinationPileUpLabel_->hide();

	combinationPileUpChoiceButton_ = new QToolButton;
	combinationPileUpChoiceButton_->setText("Ca");
	combinationPileUpChoiceButton_->hide();
	connect(combinationPileUpChoiceButton_, SIGNAL(clicked()), this, SLOT(getCombinationElement()));

	view_->setSecondaryElement(xrfTable_->elementBySymbol("Ca"));

	QHBoxLayout *combinationLayout = new QHBoxLayout;
	combinationLayout->addSpacing(20);
	combinationLayout->addWidget(combinationPileUpLabel_);
	combinationLayout->addWidget(combinationPileUpChoiceButton_);

	QVBoxLayout *pileUpLayout = new QVBoxLayout;
	pileUpLayout->addWidget(selfPileUpBox, 0, Qt::AlignLeft);
	pileUpLayout->addWidget(selfPileUpLabel_, 0, Qt::AlignCenter);
	pileUpLayout->addWidget(combinationPeakCheckBox_, 0, Qt::AlignLeft);
	pileUpLayout->addLayout(combinationLayout);

	QGroupBox *pileUpGroupBox = new QGroupBox("Pile Up Peaks");
	pileUpGroupBox->setLayout(pileUpLayout);
	pileUpGroupBox->setFixedWidth(120);
	pileUpGroupBox->setFlat(true);

	// General layout management.
	QFont font(this->font());
	font.setBold(true);

	QLabel *startLabel = new QLabel("Start & Stop");
	startLabel->setFont(font);
	startLabel->setMinimumWidth(100);
	QLabel *timeLabel = new QLabel("Real Time");
	timeLabel->setFont(font);
	minEnergyLabel_ = new QLabel("Min. Energy");
	minEnergyLabel_->hide();
	minEnergyLabel_->setFont(font);
	maxEnergyLabel_ = new QLabel("Max. Energy");
	maxEnergyLabel_->hide();
	maxEnergyLabel_->setFont(font);
	peakingTimeLabel_ = new QLabel("Peaking Time");
	peakingTimeLabel_->hide();
	peakingTimeLabel_->setFont(font);

	QHBoxLayout *startAndStopLayout = new QHBoxLayout;
	startAndStopLayout->addWidget(start_);
	startAndStopLayout->addWidget(stop);

	QVBoxLayout *controlLayout = new QVBoxLayout;
	controlLayout->addSpacing(20);
	controlLayout->addWidget(startLabel);
	controlLayout->addLayout(startAndStopLayout);controlLayout->addWidget(timeLabel);
	controlLayout->addWidget(integrationTime_);
	controlLayout->addWidget(customizeSettings);
	controlLayout->addWidget(minEnergyLabel_);
	controlLayout->addWidget(minEnergy_);
	controlLayout->addWidget(maxEnergyLabel_);
	controlLayout->addWidget(maxEnergy_);
	controlLayout->addWidget(peakingTimeLabel_);
	controlLayout->addWidget(peakingTime_);
	controlLayout->addWidget(pileUpGroupBox);
	controlLayout->addStretch();
	controlLayout->addWidget(sortButton);
	controlLayout->addWidget(configureButton);

	QHBoxLayout *selectionLayout = new QHBoxLayout;
	selectionLayout->addStretch();
	selectionLayout->addWidget(tableView);
	selectionLayout->addWidget(elView);
	selectionLayout->addStretch();

	QVBoxLayout *viewAndSelectionLayout = new QVBoxLayout;
	viewAndSelectionLayout->addWidget(view_);
	viewAndSelectionLayout->addLayout(selectionLayout);

	QHBoxLayout *plotControlLayout = new QHBoxLayout;
	plotControlLayout->addLayout(viewAndSelectionLayout);
	plotControlLayout->addLayout(controlLayout);

	QVBoxLayout *masterLayout = new QVBoxLayout;
	masterLayout->addWidget(topFrame);
	masterLayout->addLayout(plotControlLayout);

	setLayout(masterLayout);
}

void XRFFreeRunView::onStopClicked()
{
	VESPERSXRFScanController *current = qobject_cast<VESPERSXRFScanController *>(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController());

	if (current)
		current->finish();
}

void XRFFreeRunView::onAdvancedSettingsChanged(bool advanced)
{
	minEnergyLabel_->setVisible(advanced);
	minEnergy_->setVisible(advanced);
	maxEnergyLabel_->setVisible(advanced);
	maxEnergy_->setVisible(advanced);
	peakingTimeLabel_->setVisible(advanced);
	peakingTime_->setVisible(advanced);
}

void XRFFreeRunView::onShowSelfPileUpPeaks(bool showPeaks)
{
	view_->setPileUpPeaksVisible(showPeaks);
	selfPileUpLabel_->setVisible(showPeaks);
	combinationPeakCheckBox_->setVisible(showPeaks);
	combinationPeakCheckBox_->setChecked(false);
	onShowCombinationPileUpPeaks(false);
}

void XRFFreeRunView::onShowCombinationPileUpPeaks(bool showPeaks)
{
	view_->setCombinationPileUpPeaksVisible(showPeaks);
	combinationPileUpLabel_->setVisible(showPeaks);
	combinationPileUpChoiceButton_->setVisible(showPeaks);
}

void XRFFreeRunView::getCombinationElement()
{
	AMElement *el = AMPeriodicTableDialog::getElement(this);

	if (el){

		combinationPileUpChoiceButton_->setText(el->symbol());
		view_->setSecondaryElement(xrfTable_->elementByAtomicNumber(el->atomicNumber()));
	}
}
