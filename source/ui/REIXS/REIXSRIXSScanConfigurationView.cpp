#include "REIXSRIXSScanConfigurationView.h"

#include <QFormLayout>
#include <QBoxLayout>
#include <QGroupBox>

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
#include <QSlider>
#include <QIcon>

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotRectangle.h"
#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotColorMap.h"
#include "MPlot/MPlotPoint.h"

#include "dataman/datasource/AMDataSourceImageData.h"
#include "ui/dataman/AMChooseScanDialog.h"
#include "ui/dataman/AMSampleSelector.h"



REIXSRIXSScanConfigurationView::REIXSRIXSScanConfigurationView(REIXSXESScanConfiguration* config, QWidget *parent) :
	AMScanConfigurationView(parent)
{


	QGroupBox* detectorOptions = new QGroupBox("Initial Setup");
	QGroupBox* stopScanOptions = new QGroupBox("Stop scan when...");
	QGroupBox* nameOptions = new QGroupBox("Scan meta-data");
	QGroupBox* XASEnergySelect = new QGroupBox("Select Scan Energies");
	QGroupBox* energyListBox = new QGroupBox("Selected Energies");


	energyBox_ = new QDoubleSpinBox();
	energyBox_->setDecimals(2);
	energyBox_->setEnabled(false);
	energyBox_->setRange(92,2000);
	energyBox_->setValue(400);
//	applyEnergyBox_ = new QCheckBox("Beamline Energy");

	slitWidthBox_= new QDoubleSpinBox();
	slitWidthBox_->setRange(5,500);
	slitWidthBox_->setValue(25);
	slitWidthBox_->setSingleStep(5);
	slitWidthBox_->setDecimals(0);
	slitWidthBox_->setEnabled(false);
	applySlitWidthBox_ = new QCheckBox("Slit Width");


	polarizationBox_ = new QComboBox();
	polarizationBox_->addItem("Circular Left");
	polarizationBox_->addItem("Circular Right");
	polarizationBox_->addItem("Linear Horizontal");
	polarizationBox_->addItem("Linear Vertical +");
	polarizationBox_->addItem("Linear Vertical -");
	polarizationBox_->addItem("Linear Inclined");
	polarizationBox_->setCurrentIndex(1);
	polarizationBox_->setEnabled(false);
	applyPolarizationBox_ = new QCheckBox("Polarization");

	polarizationAngleBox_ = new QDoubleSpinBox();
	polarizationAngleBox_->setDecimals(0);
	polarizationAngleBox_->setEnabled(false);
	polarizationAngleBox_->setRange(-180,180);
	polarizationAngleBox_->setValue(0);
	polarizationAngleBox_->setSingleStep(10);
	polarizationAngleBox_->setEnabled(false);

	polarizationAngleLabel_ = new QLabel();
	polarizationAngleLabel_->setText("      Angle");

	doNotClearExistingCountsOption_ = new QCheckBox("Do not clear previous detector data");

	maximumTotalCounts_ = new QDoubleSpinBox();
	maximumTimeEdit_ = new QTimeEdit();

	numberEdit_ = new QSpinBox();
	numberEdit_->setRange(0, 10000);
	nameEdit_ = new QLineEdit();
	sampleSelector_ = new AMSampleSelector(AMDatabase::database("user"));
	autoNamingCheckBox_ = new QCheckBox("from last sample move");

	maximumTotalCounts_->setDecimals(0);
	maximumTotalCounts_->setRange(10, 1000000000);
	maximumTimeEdit_->setDisplayFormat("hh:mm:ss");

	addEnergy_ =  new QPushButton();
	addEnergy_->setText("Add Energy");
	QIcon down;
	down.addFile(QString::fromUtf8(":/22x22/go-down-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
	addEnergy_->setIcon(down);
	enegySlider_ = new QSlider();
	enegySlider_->setOrientation(Qt::Horizontal);
	energyList_ = new QTableWidget();
	loadXASData_ = new QPushButton();
	loadXASData_->setText("Load XAS Scan");
	removeEnergy_ = new QPushButton();
	removeEnergy_->setText("Remove");


	plot_ = new MPlot();
	plotWidget_ = new MPlotWidget();
	plotWidget_->setPlot(plot_);

//	plot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
//	plot_->axisRight()->setTicks(0);
//	plot_->axisBottom()->setTicks(4);
//	plot_->axisLeft()->showGrid(false);
//	plot_->axisBottom()->showAxisName(false);
//	plot_->axisLeft()->showAxisName(false);
//	plot_->axisScaleBottom()->setPadding(0);
//	plot_->axisScaleLeft()->setPadding(0);
//	image_ = 0;



	QGridLayout* RIXSView = new QGridLayout();
	QGridLayout* gl = new QGridLayout();


//	gl->addWidget(applyEnergyBox_,0,0);
//	gl->addWidget(energyBox_,0,1);
	gl->addWidget(applySlitWidthBox_,1,0);
	gl->addWidget(slitWidthBox_,1,1);
	gl->addWidget(applyPolarizationBox_,2,0);
	gl->addWidget(polarizationBox_,2,1);
	gl->addWidget(polarizationAngleLabel_,3,0);
	gl->addWidget(polarizationAngleBox_,3,1);

	gl->addWidget(doNotClearExistingCountsOption_,4,0,1,2,0);

	detectorOptions->setLayout(gl);

	QFormLayout* fl2 = new QFormLayout();
	fl2->addRow("Total Counts Reaches", maximumTotalCounts_);
	fl2->addRow("After this much time", maximumTimeEdit_);

	stopScanOptions->setLayout(fl2);

	QFormLayout* fl3 = new QFormLayout();
	fl3->addRow("Scan base name", nameEdit_);
//	fl3->addRow("Number", numberEdit_);
	fl3->addRow("Sample", sampleSelector_);
	fl3->addRow("Set automatically", autoNamingCheckBox_);

	nameOptions->setLayout(fl3);

	QGridLayout* gl2 = new QGridLayout();
	gl2->addWidget(plotWidget_,0,0,1,3);
	gl2->addWidget(enegySlider_,1,0,1,3);
	gl2->addWidget(energyBox_,2,0);
	gl2->addWidget(addEnergy_,2,1);
	gl2->addWidget(loadXASData_,2,2);

	XASEnergySelect->setLayout(gl2);

	QGridLayout* gl3 = new QGridLayout();
	gl3->addWidget(energyList_,0,0,1,3);
	gl3->addWidget(removeEnergy_,1,2);

	energyListBox->setLayout(gl3);

//	XASEnergySelect->setLayout(vl2);

	RIXSView->addWidget(detectorOptions, 0,0);
	RIXSView->addWidget(stopScanOptions,1,0);
	RIXSView->addWidget(nameOptions,2,0);
	RIXSView->addWidget(XASEnergySelect,0,1,3,1);
	RIXSView->addWidget(energyListBox,3,1);


	setLayout(RIXSView);




}

REIXSRIXSScanConfigurationView::~REIXSRIXSScanConfigurationView()
{
	//delete ui;
}
