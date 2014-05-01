#include "VESPERS3DScanConfigurationView.h"

#include "ui/AMTopFrame.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "util/VESPERS/GeneralUtilities.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPushButton>
#include <QSpinBox>
#include <QMenu>

VESPERS3DScanConfigurationView::~VESPERS3DScanConfigurationView(){}

VESPERS3DScanConfigurationView::VESPERS3DScanConfigurationView(VESPERS3DScanConfiguration *config, QWidget *parent)
	: VESPERSScanConfigurationView(parent)
{
	configuration_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS 3D Map Configuration");

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

//	hStart_ = buildPositionDoubleSpinBox("H: ", " mm", configuration_->xStart(), 3);
//	connect(hStart_, SIGNAL(editingFinished()), this, SLOT(onXStartChanged()));
//	connect(configuration_, SIGNAL(xStartChanged(double)), hStart_, SLOT(setValue(double)));

//	vStart_ = buildPositionDoubleSpinBox("V: ", " mm", configuration_->yStart(), 3);
//	connect(vStart_, SIGNAL(editingFinished()), this, SLOT(onYStartChanged()));
//	connect(configuration_, SIGNAL(yStartChanged(double)), vStart_, SLOT(setValue(double)));

//	QPushButton *startUseCurrentButton = new QPushButton("Use Current");
//	connect(startUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetStartPosition()));

//	QHBoxLayout *startPointLayout = new QHBoxLayout;
//	startPointLayout->addWidget(new QLabel("Start:"));
//	startPointLayout->addWidget(hStart_);
//	startPointLayout->addWidget(vStart_);
//	startPointLayout->addWidget(startUseCurrentButton);

//	hEnd_ = buildPositionDoubleSpinBox("H: ", " mm", configuration_->xEnd(), 3);
//	connect(hEnd_, SIGNAL(editingFinished()), this, SLOT(onXEndChanged()));
//	connect(configuration_, SIGNAL(xEndChanged(double)), hEnd_, SLOT(setValue(double)));

//	vEnd_ = buildPositionDoubleSpinBox("V: ", " mm", configuration_->yEnd(), 3);
//	connect(vEnd_, SIGNAL(editingFinished()), this, SLOT(onYEndChanged()));
//	connect(configuration_, SIGNAL(yEndChanged(double)), vEnd_, SLOT(setValue(double)));

//	QPushButton *endUseCurrentButton = new QPushButton("Use Current");
//	connect(endUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetEndPosition()));

//	QHBoxLayout *endPointLayout = new QHBoxLayout;
//	endPointLayout->addWidget(new QLabel("End:"));
//	endPointLayout->addWidget(hEnd_);
//	endPointLayout->addWidget(vEnd_);
//	endPointLayout->addWidget(endUseCurrentButton);

//	hStep_ = buildPositionDoubleSpinBox("H: ", QString(" %1").arg(QString::fromUtf8("µm")), configuration_->xStep()*1000, 1);	// xStep needs to be in mm.
//	connect(hStep_, SIGNAL(editingFinished()), this, SLOT(onXStepChanged()));
//	connect(configuration_, SIGNAL(xStepChanged(double)), this, SLOT(updateXStep(double)));

//	vStep_ = buildPositionDoubleSpinBox("V: ", QString(" %1").arg(QString::fromUtf8("µm")), configuration_->yStep()*1000, 1);	// yStep needs to be in mm.
//	connect(vStep_, SIGNAL(editingFinished()), this, SLOT(onYStepChanged()));
//	connect(configuration_, SIGNAL(yStepChanged(double)), this, SLOT(updateYStep(double)));

//	QHBoxLayout *stepSizeLayout = new QHBoxLayout;
//	stepSizeLayout->addWidget(new QLabel("Step Size:"));
//	stepSizeLayout->addWidget(hStep_);
//	stepSizeLayout->addWidget(vStep_);
//	stepSizeLayout->addStretch();

//	wireStart_ = buildPositionDoubleSpinBox("", " mm", configuration_->zStart(), 3);
//	connect(wireStart_, SIGNAL(editingFinished()), this, SLOT(onZStartChanged()));
//	connect(configuration_, SIGNAL(zStartChanged(double)), wireStart_, SLOT(setValue(double)));

//	QPushButton *wireStartCurrentButton = new QPushButton("Use Current");
//	connect(wireStartCurrentButton, SIGNAL(clicked()), this, SLOT(onSetWireStartPosition()));

//	QHBoxLayout *wireStartLayout = new QHBoxLayout;
//	wireStartLayout->addWidget(new QLabel("Wire Start:"));
//	wireStartLayout->addWidget(wireStart_, 0, Qt::AlignCenter);
//	wireStartLayout->addWidget(wireStartCurrentButton);

//	wireEnd_ = buildPositionDoubleSpinBox("", " mm", configuration_->zEnd(), 3);
//	connect(wireEnd_, SIGNAL(editingFinished()), this, SLOT(onZEndChanged()));
//	connect(configuration_, SIGNAL(zEndChanged(double)), wireEnd_, SLOT(setValue(double)));

//	QPushButton *wireEndCurrentButton = new QPushButton("Use Current");
//	connect(wireEndCurrentButton, SIGNAL(clicked()), this, SLOT(onSetWireEndPosition()));

//	QHBoxLayout *wireEndLayout = new QHBoxLayout;
//	wireEndLayout->addWidget(new QLabel("Wire End:"));
//	wireEndLayout->addWidget(wireEnd_, 0, Qt::AlignCenter);
//	wireEndLayout->addWidget(wireEndCurrentButton);

//	wireStep_ = buildPositionDoubleSpinBox("", QString(" %1").arg(QString::fromUtf8("µm")), configuration_->zStep()*1000, 1);	// yStep needs to be in mm.
//	connect(wireStep_, SIGNAL(editingFinished()), this, SLOT(onZStepChanged()));
//	connect(configuration_, SIGNAL(zStepChanged(double)), this, SLOT(updateZStep(double)));

//	QHBoxLayout *wireStepLayout = new QHBoxLayout;
//	wireStepLayout->addWidget(new QLabel("Wire Step:"));
//	wireStepLayout->addWidget(wireStep_);

	mapInfo_ = new QLabel;
	updateMapInfo();

	QVBoxLayout *positionsLayout = new QVBoxLayout;
//	positionsLayout->addLayout(startPointLayout);
//	positionsLayout->addLayout(endPointLayout);
//	positionsLayout->addLayout(stepSizeLayout);
//	positionsLayout->addLayout(wireStartLayout);
//	positionsLayout->addLayout(wireEndLayout);
//	positionsLayout->addLayout(wireStepLayout);
//	positionsLayout->addWidget(mapInfo_);

	positionsBox->setLayout(positionsLayout);

	// Dwell time.
	dwellTime_ = addDwellTimeWidget(configuration_->scanAxisAt(0)->regionAt(0)->regionTime());
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(configuration_, SIGNAL(timeStepChanged(double)), dwellTime_, SLOT(setValue(double)));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addLayout(timeLayout);
	timeBoxLayout->addWidget(estimatedTime_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	// Using the CCD.
	QGroupBox *ccdBox = addCCDDetectorSelectionView();
	connect(ccdButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onCCDDetectorChanged(int)));
	connect(configuration_->dbObject(), SIGNAL(ccdDetectorChanged(int)), this, SLOT(updateCCDDetectorButtons(int)));
	ccdButtonGroup_->button(int(configuration_->ccdDetector()))->setChecked(true);
	ccdButtonGroup_->button((int)VESPERS::Roper)->hide();
	ccdButtonGroup_->button((int)VESPERS::Mar)->hide();

	configureCCDButton_ = new QPushButton(QIcon(":/hammer-wrench.png"), "Configure Area Detector");
	configureCCDButton_->setEnabled(configuration_->ccdDetector());
	connect(configureCCDButton_, SIGNAL(clicked()), this, SLOT(onConfigureCCDDetectorClicked()));

	// The fluorescence detector setup
	QGroupBox *fluorescenceDetectorGroupBox  = addFluorescenceDetectorSelectionView();
	fluorescenceButtonGroup_->button(int(VESPERS::NoXRF))->setDisabled(true);
	connect(fluorescenceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(configuration_->dbObject(), SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceDetector(int)));
	fluorescenceButtonGroup_->button((int)configuration_->fluorescenceDetector())->setChecked(true);

	// Ion chamber selection
	QGroupBox *I0GroupBox = addI0SelectionView();
	connect(I0Group_, SIGNAL(buttonClicked(int)), this, SLOT(onI0Clicked(int)));
	connect(configuration_->dbObject(), SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0Buttons(int)));
	I0Group_->button((int)configuration_->incomingChoice())->click();

	// Motor selection.
	QGroupBox *motorSetChoiceBox = addMotorSelectionView(QStringList() << "H and V", QList<int>() << (VESPERS::H | VESPERS::V));
	connect(motorButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onMotorChanged(int)));
	motorButtonGroup_->button(int(configuration_->motor()))->click();

	// CCD label.
	ccdText_ = new QLabel;
	ccdHelpText_ = new QLabel;
	ccdTextBox_ = new QGroupBox("CCD Detector Info");
	QVBoxLayout *ccdTextLayout = new QVBoxLayout;
	ccdTextLayout->addWidget(ccdText_);
	ccdTextLayout->addWidget(ccdHelpText_);
	ccdTextBox_->setLayout(ccdTextLayout);
	ccdTextBox_->setVisible(configuration_->ccdDetector() != VESPERS::NoCCD);

	// Scan name selection
	scanName_ = addScanNameView(configuration_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	// Only connecting this signal because it is the only CCD available currently.  It would need some logic for switching which CCD it was actually connected to.
	connect(VESPERSBeamline::vespers()->vespersPilatusAreaDetector(), SIGNAL(ccdPathChanged(QString)), this, SLOT(onScanNameEdited()));
	onScanNameEdited();

	QFormLayout *scanNameLayout = new QFormLayout;
	scanNameLayout->addRow("Scan Name:", scanName_);

	// The roi text edit and configuration.
	roiText_ = new QTextEdit;
	roiText_->setReadOnly(true);

	QPushButton *configureXRFDetectorButton = new QPushButton(QIcon(":/hammer-wrench.png"), "Configure XRF Detector");
	connect(configureXRFDetectorButton, SIGNAL(clicked()), this, SLOT(onConfigureXRFDetectorClicked()));

	QFormLayout *roiTextLayout = new QFormLayout;
	roiTextLayout->addRow(roiText_);
	roiTextLayout->addRow(configureXRFDetectorButton);

	QGroupBox *roiTextBox = new QGroupBox("Regions Of Interest");
	roiTextBox->setLayout(roiTextLayout);

	// Label showing where the data will be saved.
	QLabel *exportPath = addExportPathLabel();

	QGroupBox *timeOffsetBox = addTimeOffsetLabel(configuration_->timeOffset());
	connect(timeOffset_, SIGNAL(valueChanged(double)), this, SLOT(setTimeOffset(double)));

	// Auto-export option.
	QGroupBox *autoExportGroupBox = addExporterOptionsView(QStringList() << "Ascii" << "SMAK", configuration_->exportSpectraSources(), configuration_->exportSpectraInRows());
	connect(autoExportButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateAutoExporter(int)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), configuration_, SLOT(setExportSpectraSources(bool)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), exportSpectraInRows_, SLOT(setEnabled(bool)));
	connect(exportSpectraInRows_, SIGNAL(toggled(bool)), this, SLOT(updateExportSpectraInRows(bool)));
	autoExportButtonGroup_->button(configuration_->exportAsAscii() ? 0 : 1)->click();

	// Setting up the layout.
	QGridLayout *contentsLayout = new QGridLayout;
	contentsLayout->addWidget(positionsBox, 0, 0, 2, 3);
	contentsLayout->addWidget(timeGroupBox, 2, 0, 1, 1);
	contentsLayout->addWidget(ccdBox, 3, 0, 1, 1);
	contentsLayout->addLayout(scanNameLayout, 4, 0, 1, 1);
	contentsLayout->addWidget(timeOffsetBox, 5, 0, 1, 1);
	contentsLayout->addWidget(configureCCDButton_, 6, 0, 1, 1);
	contentsLayout->addWidget(motorSetChoiceBox, 0, 3, 1, 1);
	contentsLayout->addWidget(fluorescenceDetectorGroupBox, 1, 3, 2, 1);
	contentsLayout->addWidget(I0GroupBox, 3, 3, 2, 1);
	contentsLayout->addWidget(ccdTextBox_, 7, 0, 1, 6);
	contentsLayout->addWidget(roiTextBox, 0, 5, 3, 3);
	contentsLayout->addWidget(autoExportGroupBox, 3, 5, 2, 3);

	QHBoxLayout *squeezeContents = new QHBoxLayout;
	squeezeContents->addStretch();
	squeezeContents->addLayout(contentsLayout);
	squeezeContents->addStretch();

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addWidget(frame);
	configViewLayout->addStretch();
	configViewLayout->addLayout(squeezeContents);
	configViewLayout->addStretch();
	configViewLayout->addWidget(exportPath, 0, Qt::AlignCenter);
	configViewLayout->addSpacing(30);

	setLayout(configViewLayout);
}

void VESPERS3DScanConfigurationView::onScanNameEdited()
{
	QString name = scanName_->text();
	configuration_->setName(name);
	configuration_->setUserScanName(name);

	if (configuration_->ccdDetector() != VESPERS::NoCCD){

		QString path;

		if (configuration_->ccdDetector() == VESPERS::Roper)
			path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();

		else if (configuration_->ccdDetector() == VESPERS::Mar)
			path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

		else if (configuration_->ccdDetector() == VESPERS::Pilatus)
			path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

		ccdText_->setText(QString("Path: %1\nName: %2").arg(path).arg(name));
		configuration_->setCCDFileName(name);
		checkCCDFileNames(name);
	}

	if (configuration_->ccdDetector() == VESPERS::Pilatus && name.contains(" ")){

		QPalette palette = scanName_->palette();
		palette.setColor(QPalette::Base, Qt::red);
		scanName_->setPalette(palette);
	}

	else
		scanName_->setPalette(this->palette());
}

void VESPERS3DScanConfigurationView::checkCCDFileNames(const QString &name) const
{
	QString path;

	if (configuration_->ccdDetector() == VESPERS::Roper){

		path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();
		path.replace("Y:\\", "/nas/vespers/");
		path.replace('\\', '/');
	}

	else if (configuration_->ccdDetector() == VESPERS::Mar)
		path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

	else if (configuration_->ccdDetector() == VESPERS::Pilatus)
		path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

	if (VESPERS::fileNameExists(path, name)){

		ccdHelpText_->show();
		ccdHelpText_->setText(QString("The scan name you have chosen conflicts with existing CCD file names.\nIf you don't a random suffix will be added to avoid name conflicts.\neg. %1").arg(VESPERS::appendUniqueIdentifier(name)));
	}

	else{

		ccdHelpText_->setText("");
		ccdHelpText_->hide();
	}
}

void VESPERS3DScanConfigurationView::onFluorescenceChoiceChanged(int id)
{
	configuration_->setFluorescenceDetector(id);
	updateRoiText();
}

void VESPERS3DScanConfigurationView::onMotorChanged(int id)
{
	configuration_->setMotor(id);
}

void VESPERS3DScanConfigurationView::onCCDDetectorChanged(int id)
{
	configuration_->setCCDDetector(id);

	if (configuration_->ccdDetector() != VESPERS::NoCCD){

		QString path;
		QString name = configuration_->ccdFileName().isEmpty() ? scanName_->text() : configuration_->ccdFileName();

		if (configuration_->ccdDetector() == VESPERS::Roper)
			path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();

		else if (configuration_->ccdDetector() == VESPERS::Mar)
			path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

		else if (configuration_->ccdDetector() == VESPERS::Pilatus)
			path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

		configuration_->setCCDFileName(name);
		ccdText_->setText(QString("Path: %1\nName: %2").arg(path).arg(name));
		checkCCDFileNames(name);
	}

	onScanNameEdited();
	ccdTextBox_->setVisible(configuration_->ccdDetector() != VESPERS::NoCCD);
	configureCCDButton_->setDisabled(configuration_->ccdDetector() == VESPERS::NoCCD);
}

void VESPERS3DScanConfigurationView::updateRoiText()
{
	VESPERS::FluorescenceDetectors xrfFlag = configuration_->fluorescenceDetector();

	if (xrfFlag == VESPERS::NoXRF)
		configuration_->setRoiInfoList(AMROIInfoList());

	else if (xrfFlag == VESPERS::SingleElement)
		configuration_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList());

	else if (xrfFlag == VESPERS::FourElement)
		configuration_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());

	else if (xrfFlag == (VESPERS::SingleElement | VESPERS::FourElement)){

		AMROIInfoList list;
		AMROIInfoList singleElList = *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();
		AMROIInfoList fourElList = *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();

		for (int i = 0, count = singleElList.count(); i < count; i++)
			list.append(singleElList.at(i));

		for (int i = 0, count = fourElList.count(); i < count; i++)
			list.append(fourElList.at(i));

		configuration_->setRoiInfoList(list);
	}

	updateAndSetRoiTextBox(int(configuration_->fluorescenceDetector()));
}

void VESPERS3DScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + VESPERS::convertTimeToString(configuration_->totalTime()));
}

void VESPERS3DScanConfigurationView::onSetStartPosition()
{
//	double h = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl()->value();
//	double v = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl()->value();
//	double n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalControl()->value();

//	configuration_->setXStart(h);
//	hStart_->setValue(h);
//	configuration_->setYStart(v);
//	vStart_->setValue(v);
//	configuration_->setNormalPosition(n);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onSetEndPosition()
{
//	double h = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl()->value();
//	double v = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl()->value();

//	configuration_->setXEnd(h);
//	hEnd_->setValue(h);
//	configuration_->setYEnd(v);
//	vEnd_->setValue(v);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onSetWireStartPosition()
{
//	double wire = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalControl()->value();

//	configuration_->setZStart(wire);
//	wireStart_->setValue(wire);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onSetWireEndPosition()
{
//	double wire = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalControl()->value();

//	configuration_->setZEnd(wire);
//	wireEnd_->setValue(wire);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onXStartChanged()
{
//	configuration_->setXStart(hStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onXEndChanged()
{
//	configuration_->setXEnd(hEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onXStepChanged()
{
//	configuration_->setXStep(hStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onYStartChanged()
{
//	configuration_->setYStart(vStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onYEndChanged()
{
//	configuration_->setYEnd(vEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onYStepChanged()
{
//	configuration_->setYStep(vStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onZStartChanged()
{
//	configuration_->setZStart(wireStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onZEndChanged()
{
//	configuration_->setZEnd(wireEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onZStepChanged()
{
//	configuration_->setZStep(wireStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onDwellTimeChanged()
{
//	configuration_->setTimeStep(dwellTime_->value());
}

void VESPERS3DScanConfigurationView::updateMapInfo()
{
//	double hSize = fabs(configuration_->xEnd()-configuration_->xStart());
//	double vSize = fabs(configuration_->yEnd()-configuration_->yStart());
//	double wireSize = fabs(configuration_->zEnd()-configuration_->zStart());

//	int hPoints = int((hSize)/configuration_->xStep());
//	if ((hSize - (hPoints + 0.01)*configuration_->xStep()) < 0)
//		hPoints += 1;
//	else
//		hPoints += 2;

//	int vPoints = int((vSize)/configuration_->yStep());
//	if ((vSize - (vPoints + 0.01)*configuration_->yStep()) < 0)
//		vPoints += 1;
//	else
//		vPoints += 2;

//	int wirePoints = int((wireSize)/configuration_->zStep());
//	if ((vSize - (wirePoints + 0.01)*configuration_->zStep()) < 0)
//		wirePoints += 1;
//	else
//		wirePoints += 2;

//	mapInfo_->setText(QString("Map Size: %1 %2 x %3 %2 x %6 %2\t Points: %4 x %5 x %7")
//					  .arg(QString::number(hSize*1000, 'f', 1))
//					  .arg(QString::fromUtf8("µm"))
//					  .arg(QString::number(vSize*1000, 'f', 1))
//					  .arg(hPoints)
//					  .arg(vPoints)
//					  .arg(QString::number(wireSize*1000, 'f', 1))
//					  .arg(wirePoints)
//					  );
}

void VESPERS3DScanConfigurationView::axesAcceptable()
{
	QPalette good(this->palette());
	QPalette bad(good);
	bad.setColor(QPalette::Base, Qt::red);

//	hStep_->setPalette(configuration_->validXAxis() ? good : bad);
//	vStep_->setPalette(configuration_->validYAxis() ? good : bad);
//	wireStep_->setPalette(configuration_->validZAxis() ? good : bad);
}
