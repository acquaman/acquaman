#include "XRFFreeRunView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"
#include "ui/AMTopFrame.h"
#include "util/VESPERS/GeneralUtilities.h"
#include "util/VESPERS/XRFElement.h"
#include "ui/VESPERS/XRFPeriodicTableView.h"
#include "ui/VESPERS/VESPERSXRFElementView.h"
#include "ui/VESPERS/VESPERSXRFScanConfigurationView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPushButton>

XRFFreeRunView::XRFFreeRunView(XRFFreeRun *xrfFreeRun, QWidget *parent)
	: QWidget(parent)
{
	xrfFreeRun_ = xrfFreeRun;
	detector_ = xrfFreeRun_->detector();
	xrfTable_ = xrfFreeRun_->table();

	AMTopFrame *topFrame = new AMTopFrame(QString("XRF Free Run - %1").arg(detector_->name()));
	topFrame->setIcon(QIcon(":/utilities-system-monitor.png"));

	connect(detector_, SIGNAL(statusChanged()), this, SLOT(onStatusChanged()));

	view_ = new XRFDetailedDetectorView(detector_);
	view_->setMinimumEnergy(xrfTable_->minimumEnergy());
	view_->setMaximumEnergy(xrfTable_->maximumEnergy());
	connect(detector_, SIGNAL(detectorConnected(bool)), this, SLOT(setEnabled(bool)));
	connect(xrfTable_, SIGNAL(currentElementChanged(XRFElement*)), view_, SLOT(showEmissionLines(XRFElement*)));
	connect(xrfTable_, SIGNAL(currentElementChanged(XRFElement*)), view_, SLOT(highlightMarkers(XRFElement*)));
	connect(xrfTable_, SIGNAL(minimumEnergyChanged(double)), view_, SLOT(setMinimumEnergy(double)));
	connect(xrfTable_, SIGNAL(maximumEnergyChanged(double)), view_, SLOT(setMaximumEnergy(double)));
	connect(xrfTable_, SIGNAL(removedAllRegionsOfInterest()), view_, SLOT(removeAllRegionsOfInterestMarkers()));

	XRFPeriodicTableView *tableView = new XRFPeriodicTableView(xrfTable_);
	QPalette palette = tableView->palette();
	palette.setColor(QPalette::Window, QColor(79, 148, 205));
	tableView->setPalette(palette);
	tableView->setAutoFillBackground(true);

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

	// Scan config view.
	VESPERSXRFScanConfigurationView *scanConfigView = new VESPERSXRFScanConfigurationView(xrfFreeRun_->configuration());

	QFont font(this->font());
	font.setBold(true);

	QLabel *startLabel = new QLabel("Start & Stop");
	startLabel->setFont(font);
	startLabel->setMinimumWidth(100);

	QHBoxLayout *startAndStopLayout = new QHBoxLayout;
	startAndStopLayout->addWidget(start_);
	startAndStopLayout->addWidget(stop);

	QVBoxLayout *controlLayout = new QVBoxLayout;
	controlLayout->addSpacing(20);
	controlLayout->addWidget(startLabel);
	controlLayout->addLayout(startAndStopLayout);
	controlLayout->addWidget(scanConfigView);
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
