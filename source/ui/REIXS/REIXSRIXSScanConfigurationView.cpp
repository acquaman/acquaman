/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "REIXSRIXSScanConfigurationView.h"

#include <QFormLayout>
#include <QBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QIcon>
#include <QUrl>

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotRectangle.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotColorMap.h"
#include "MPlot/MPlotPoint.h"
#include "ui/dataman/AMScanView.h"
#include "ui/AMTopFrame.h"


#include "dataman/datasource/AMDataSourceImageData.h"
#include "ui/dataman/AMChooseScanDialog.h"
#include "ui/dataman/AMSamplePre2013Editor.h"
#include "dataman/AMScan.h"


#include "source/actions3/AMActionRunner3.h"
#include "source/ui/actions3/AMActionRunnerQueueView3.h"
#include "source/ui/actions3/AMWorkflowView3.h"




REIXSRIXSScanConfigurationView::REIXSRIXSScanConfigurationView(REIXSXESScanConfiguration* config, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	if(config) {
		configuration_ = config;
		ownsConfiguration_ = false;
	}
	else {
		configuration_ = new REIXSXESScanConfiguration(this);
		ownsConfiguration_ = true;
	}

	chooseScanDialog_ = 0;
	/*
	image_ = new MPlotSeriesBasic();
	image_->setColorMap(*colorMap_);
	*/

	currentScan_ = 0;

	queueView_ = new AMActionRunnerQueueView3(AMActionRunner3::workflow());

	detectorOptions = new QGroupBox("Initial Setup");
	stopScanOptions = new QGroupBox("Stop scan when...");
	nameOptions = new QGroupBox("Scan meta-data");
	XASEnergySelect = new QGroupBox("Select Scan Energy");

	energyBox_ = new QDoubleSpinBox();
	energyBox_->setDecimals(2);
	energyBox_->setEnabled(true);
	energyBox_->setRange(92,2000);
	energyBox_->setValue(configuration_->energy());
	energyBox_->setSingleStep(0.1);

	energyBoxLabel_ = new QLabel("RIXS Excitation Energy:");

	slitWidthBox_= new QDoubleSpinBox();
	slitWidthBox_->setRange(5,500);
	slitWidthBox_->setValue(configuration_->slitWidth());
	slitWidthBox_->setSingleStep(5);
	slitWidthBox_->setDecimals(0);
	slitWidthBox_->setEnabled(configuration_->applySlitWidth());
	applySlitWidthBox_ = new QCheckBox("Slit Width");
	applySlitWidthBox_->setChecked(configuration_->applySlitWidth());


	polarizationBox_ = new QComboBox();
	polarizationBox_->addItem("Circular Left");
	polarizationBox_->addItem("Circular Right");
	polarizationBox_->addItem("Linear Horizontal");
	polarizationBox_->addItem("Linear Vertical +");
	polarizationBox_->addItem("Linear Vertical -");
	polarizationBox_->addItem("Linear Inclined");
	polarizationBox_->setCurrentIndex(configuration_->polarization());
	polarizationBox_->setEnabled(configuration_->applyPolarization());
	applyPolarizationBox_ = new QCheckBox("Polarization");
	applyPolarizationBox_->setChecked(configuration_->applyPolarization());


	polarizationAngleBox_ = new QDoubleSpinBox();
	polarizationAngleBox_->setDecimals(0);
	polarizationAngleBox_->setRange(-180,180);
	polarizationAngleBox_->setEnabled((configuration_->applyPolarization() && (configuration_->polarization() == 5)));
	polarizationAngleBox_->setValue(configuration_->polarizationAngle());
	polarizationAngleBox_->setSingleStep(10);

	polarizationAngleLabel_ = new QLabel();
	polarizationAngleLabel_->setText("      Angle");

	doNotClearExistingCountsOption_ = new QCheckBox("Do not clear previous detector data");

	maximumTotalCounts_ = new QDoubleSpinBox();
	maximumTimeEdit_ = new QTimeEdit();

	numberEdit_ = new QSpinBox();
	numberEdit_->setRange(0, 10000);
	numberEdit_->setEnabled(false);
	nameEdit_ = new QLineEdit();
	sampleSelector_ = new AMSamplePre2013Editor(AMDatabase::database("user"));
	autoNamingCheckBox_ = new QCheckBox("from last sample move");

	maximumTotalCounts_->setDecimals(0);
	maximumTotalCounts_->setRange(10, 1000000000);
	maximumTimeEdit_->setDisplayFormat("hh:mm:ss");

	XASScanName_ = new QLabel("Select XAS Scan...");

	/*
	addEnergy_ =  new QPushButton();
	addEnergy_->setText("Add Energy");
	QIcon down;
	down.addFile(QString::fromUtf8(":/22x22/go-down-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
	addEnergy_->setIcon(down);
	queueScan_ = new QPushButton();
	queueScan_->setText("Add This Energy");
	*/

	loadXASData_ = new QPushButton();
	loadXASData_->setText("Load XAS Scan");


	// Add scan view (plots)
	scanView_ = new AMScanView();
	scanView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	scanView_->changeViewMode(AMScanView::Tabs);

	// share the scan set model with the AMScanView
	scanSetModel_ = scanView_->model();

	/*
	plot_ = new MPlot();
	plotWidget_ = new MPlotWidget();
	plotWidget_->setPlot(plot_);

	plot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	plot_->axisRight()->setTicks(0);
	plot_->axisBottom()->setTicks(4);
	plot_->axisLeft()->showGrid(false);
	plot_->axisBottom()->showAxisName(false);
	plot_->axisLeft()->showAxisName(false);
	plot_->axisScaleBottom()->setPadding(0);
	plot_->axisScaleLeft()->setPadding(0);
	image_ = 0;
	*/

	QGridLayout* RIXSView = new QGridLayout();


	QGridLayout* initialSetupLayout = new QGridLayout();
	initialSetupLayout->addWidget(applySlitWidthBox_,1,0);
	initialSetupLayout->addWidget(slitWidthBox_,1,1);
	initialSetupLayout->addWidget(applyPolarizationBox_,2,0);
	initialSetupLayout->addWidget(polarizationBox_,2,1);
	initialSetupLayout->addWidget(polarizationAngleLabel_,3,0);
	initialSetupLayout->addWidget(polarizationAngleBox_,3,1);
	initialSetupLayout->addWidget(doNotClearExistingCountsOption_,4,0,1,2,0);
	detectorOptions->setLayout(initialSetupLayout);

	QFormLayout* stopScanWhenLayout = new QFormLayout();
	stopScanWhenLayout->addRow("Total Counts Reaches", maximumTotalCounts_);
	stopScanWhenLayout->addRow("After this much time", maximumTimeEdit_);
	stopScanOptions->setLayout(stopScanWhenLayout);

	QFormLayout* scanMetaDataLayout = new QFormLayout();
	scanMetaDataLayout->addRow("Scan base name", nameEdit_);
	/*
	scanMetaDataLayout->addRow("Number", numberEdit_);
	*/
	scanMetaDataLayout->addRow("Sample", sampleSelector_);
	scanMetaDataLayout->addRow("Set automatically", autoNamingCheckBox_);
	nameOptions->setLayout(scanMetaDataLayout);

	QGridLayout* XASScanViewLayout = new QGridLayout();
	XASScanViewLayout->addWidget(scanView_,0,0,1,5);
	XASScanViewLayout->addWidget(energyBoxLabel_,1,0);
	XASScanViewLayout->addWidget(energyBox_,1,1);
	/*
	XASScanViewLayout->addWidget(queueScan_,1,2);
	*/
	XASScanViewLayout->addWidget(loadXASData_,1,4);
	XASEnergySelect->setLayout(XASScanViewLayout);

	RIXSView->addWidget(detectorOptions, 0,0);
	RIXSView->addWidget(stopScanOptions,1,0);
	RIXSView->addWidget(nameOptions,2,0);
	RIXSView->addWidget(XASEnergySelect,0,1,3,1);
	RIXSView->addWidget(queueView_, 3,0,1,2);

	QVBoxLayout* outerVLayout = new QVBoxLayout();
	outerVLayout->setContentsMargins(0,0,0,0);
	outerVLayout->setSpacing(0);
	outerVLayout->addWidget(new AMTopFrame("Setup RIXS Scans", QIcon(":/utilities-system-monitor.png")));
	outerVLayout->addLayout(RIXSView);

	setLayout(outerVLayout);

	currentCalibrationId_ = -1;

	maximumTotalCounts_->setValue(configuration_->maximumTotalCounts());
	maximumTimeEdit_->setTime(QTime().addSecs(int(configuration_->maximumDurationSeconds())));
	doNotClearExistingCountsOption_->setChecked(configuration_->doNotClearExistingCounts());
	nameEdit_->setText(configuration_->userScanName());
	numberEdit_->setValue(configuration_->scanNumber());
	sampleSelector_->setCurrentSampleFromId(configuration_->sampleId());
	autoNamingCheckBox_->setChecked(configuration_->namedAutomatically());
	onAutoNamingCheckboxClicked(configuration_->namedAutomatically());

	configuration_->setApplyEnergy(true);

	connect(loadXASData_, SIGNAL(clicked()), this, SLOT(onloadXASDataClicked()));
	/*
	connect(queueScan_, SIGNAL(clicked()), this, SLOT(onQueueScanClicked()));
	*/
	connect(applySlitWidthBox_, SIGNAL(toggled(bool)),slitWidthBox_,SLOT(setEnabled(bool)));
	connect(applySlitWidthBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setApplySlitWidth(bool)));
	connect(slitWidthBox_, SIGNAL(valueChanged(double)), configuration_, SLOT(setSlitWidth(double)));

	connect(energyBox_, SIGNAL(valueChanged(double)), configuration_, SLOT(setEnergy(double)));

	connect(applyPolarizationBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setApplyPolarization(bool)));
	connect(polarizationBox_, SIGNAL(currentIndexChanged(int)), configuration_, SLOT(setPolarization(int)));
	connect(polarizationAngleBox_, SIGNAL(valueChanged(double)), configuration_, SLOT(setPolarizationAngle(double)));
	connect(applyPolarizationBox_, SIGNAL(clicked(bool)), polarizationBox_, SLOT(setEnabled(bool)));
	connect(applyPolarizationBox_, SIGNAL(clicked(bool)), this, SLOT(reviewPolarizationAngleBoxEnabled()));
	connect(polarizationBox_, SIGNAL(activated(int)), this, SLOT(reviewPolarizationAngleBoxEnabled()));



	connect(doNotClearExistingCountsOption_, SIGNAL(toggled(bool)), configuration_, SLOT(setDoNotClearExistingCounts(bool)));

	connect(maximumTotalCounts_, SIGNAL(valueChanged(double)), configuration_, SLOT(setMaximumTotalCounts(double)));
	connect(maximumTimeEdit_, SIGNAL(timeChanged(QTime)), this, SLOT(onMaximumTimeEditChanged(QTime)));

	connect(autoNamingCheckBox_, SIGNAL(clicked(bool)), this, SLOT(onAutoNamingCheckboxClicked(bool)));

	connect(nameEdit_, SIGNAL(textEdited(QString)), configuration_, SLOT(setUserScanName(QString)));
	connect(numberEdit_, SIGNAL(valueChanged(int)), configuration_, SLOT(setScanNumber(int)));
	connect(sampleSelector_, SIGNAL(currentSampleChanged(int)), configuration_, SLOT(setSampleId(int)));
	connect(autoNamingCheckBox_, SIGNAL(clicked(bool)), configuration_, SLOT(setNamedAutomatically(bool)));

	connect(scanView_, SIGNAL(dataPositionChanged(QPointF)), this, SLOT(onDataPositionToolChanged(QPointF)));
	connect(energyBox_, SIGNAL(valueChanged(double)), this, SLOT(onEnergySpinBoxChanged(double)));
}

REIXSRIXSScanConfigurationView::~REIXSRIXSScanConfigurationView()
{
}


void REIXSRIXSScanConfigurationView::onloadXASDataClicked()
{
	if(!chooseScanDialog_) {
		chooseScanDialog_ = new AMChooseScanDialog(AMDatabase::database("user"), "Open an existing scan", "Choose existing XAS scan to open.", this);
		connect(chooseScanDialog_, SIGNAL(accepted()), this, SLOT(onloadXASDataChosen()));
	}
	chooseScanDialog_->show();
}

/*
void REIXSRIXSScanConfigurationView::onQueueScanClicked()
{
	AMScanConfiguration *config = configuration()->createCopy();

	AMBeamlineScanAction* action = new AMBeamlineScanAction(config);
	AMActionRunner3::workflow()->addActionToQueue();

}
*/


void REIXSRIXSScanConfigurationView::onloadXASDataChosen()
{
	dropScanURLs(chooseScanDialog_->getSelectedScans());
}


bool REIXSRIXSScanConfigurationView::dropScanURLs(const QList<QUrl>& urls) {

	if(	!urls.count() )
		return false;

	bool accepted = false;

	foreach(QUrl url, urls) {

		bool isScanning;
		QString scanName;
		AMScan* scan = AMScan::createFromDatabaseUrl(url, false, &isScanning, &scanName);

		// See if this scan is acquiring, and refuse to create a new instance if so.
		/// \todo With the new AMScan memory management model, we could actually open the existing AMScan* instance in multiple editors if desired... But propagation of changes under editing might be a problem, all editors currently assuming they are the only ones modifying the scan.
		if(isScanning) {
			QMessageBox stillScanningEnquiry;
			stillScanningEnquiry.setWindowTitle("This scan is still acquiring.");
			stillScanningEnquiry.setText(QString("The scan '%1' is currently still acquiring data, so you can't open multiple copies of it yet.")
										 .arg(scanName));
			stillScanningEnquiry.setIcon(QMessageBox::Information);
			stillScanningEnquiry.addButton(QMessageBox::Ok);
			stillScanningEnquiry.exec();
			continue;
		}

		if(!scan)
			continue;
		if(scan->type().compare("AMXASScan"))  //.compare() returns 0 if same
			continue;

		// success!
		addScan(scan);
		accepted = true;
	}

	return accepted;
}

void REIXSRIXSScanConfigurationView::addScan(AMScan* newScan) {
	scanView_->setPlotCursorVisibility(false);

	currentScan_ = scanSetModel_->scanAt(0);
	scanSetModel_->removeScan(currentScan_);
	scanSetModel_->addScan(newScan);
	currentScan_ = scanSetModel_->scanAt(0);
	/*
	ui_.scanListView->setCurrentIndex(scanSetModel_->indexForScan(scanSetModel_->indexOf(newScan)));
	*/

	if(scanSetModel_->exclusiveDataSourceName().isEmpty()) {
		QVector<int> nonHiddenDataSourceIndexes = newScan->nonHiddenDataSourceIndexes();
		if(!nonHiddenDataSourceIndexes.isEmpty())
			scanSetModel_->setExclusiveDataSourceByName(newScan->dataSourceAt(nonHiddenDataSourceIndexes.first())->name());
	}
	scanView_->setScanBarsVisible(true);
	energyBox_->setValue(currentScan_->dataSourceAt(0)->axisValue(0,currentScan_->dataSourceAt(0)->size(0)/2));
	scanView_->setPlotCursorCoordinates(currentScan_->dataSourceAt(0)->axisValue(0,currentScan_->dataSourceAt(0)->size(0)/2));
	scanView_->setPlotCursorVisibility(true);

	/*
	QList<AMDataSource*> raw1DDataSources;
	if(scan->rawDataSources()->at(i)->rank() == 1)
		raw1DDataSources << currentScan_->rawDataSources()->at(0);
	*/

	QList<AMDataSource*> raw1DDataSources;
	for(int i=0; i<currentScan_->rawDataSources()->count(); i++)
		if(currentScan_->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << currentScan_->rawDataSources()->at(i);

	emit scanAdded(this, newScan);
	refreshWindowTitle();

	/*
	if(currentScan_->sampleId())
	{
		if(autoNamingCheckBox_->isChecked()) autoNamingCheckBox_->click();
		sampleSelector_->setCurrentSample(currentScan_->sampleId());
		numberEdit_ = 0;
		nameEdit_->setText(QString("RIXS %1").arg(currentScan_->name()).replace("XAS", ""));
	}
	*/
}

void REIXSRIXSScanConfigurationView::refreshWindowTitle() {

	int numScans = scanSetModel_->scanCount();

	if(numScans == 0) {
		XASEnergySelect->setTitle("Select XAS Scan:");
		/*
		setWindowTitle("Scan Editor");
		*/
		return;
	}

	QString windowTitle = scanSetModel_->scanAt(0)->fullName();
	windowTitle.prepend("Select RIXS Scan Energy from ");
	windowTitle.append(":");
	/*
	setWindowTitle(windowTitle);
	*/
	XASEnergySelect->setTitle(windowTitle);
}



/*
// Overloaded to enable drag-dropping scans (when Drag Action = Qt::CopyAction and mime-type = "text/uri-list" with the proper format.)
void REIXSRIXSScanConfigurationView::dragEnterEvent(QDragEnterEvent *event) {
	if(	event->possibleActions() & Qt::CopyAction
			&& event->mimeData()->hasUrls()
			&& event->mimeData()->urls().count() > 0
			&& event->mimeData()->urls().at(0).scheme() == "amd"
			) {
		event->accept();
		event->acceptProposedAction();
	}
}
*/

void REIXSRIXSScanConfigurationView::onMaximumTimeEditChanged(const QTime &time) {

	QTime baseTime(0,0);
	double totalSeconds = baseTime.secsTo(time);
	configuration_->setMaximumDurationSeconds(int(totalSeconds));
}

void REIXSRIXSScanConfigurationView::onAutoNamingCheckboxClicked(bool autoOn)
{
	nameEdit_->setEnabled(!autoOn);
	/*
	numberEdit_->setEnabled(!autoOn);
	*/
	sampleSelector_->setEnabled(!autoOn);
}

void REIXSRIXSScanConfigurationView::reviewPolarizationAngleBoxEnabled()
{
	polarizationAngleBox_->setEnabled(configuration_->polarization() == 5 && configuration_->applyPolarization());
}

void REIXSRIXSScanConfigurationView::onDataPositionToolChanged(const QPointF &newData)
{
	energyBox_->setValue(newData.x());
}

void REIXSRIXSScanConfigurationView::onEnergySpinBoxChanged(double enegry)
{
	scanView_->blockSignals(true);
	scanView_->setPlotCursorCoordinates(enegry);
	scanView_->blockSignals(false);
}

