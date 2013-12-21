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
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS 3D Map Configuration");

	// Setup the group box for setting the start and end points.
	QGroupBox *positionsBox = new QGroupBox("Positions");

	hStart_ = buildPositionDoubleSpinBox("H: ", " mm", config_->xStart(), 3);
	connect(hStart_, SIGNAL(editingFinished()), this, SLOT(onXStartChanged()));
	connect(config_, SIGNAL(xStartChanged(double)), hStart_, SLOT(setValue(double)));

	vStart_ = buildPositionDoubleSpinBox("V: ", " mm", config_->yStart(), 3);
	connect(vStart_, SIGNAL(editingFinished()), this, SLOT(onYStartChanged()));
	connect(config_, SIGNAL(yStartChanged(double)), vStart_, SLOT(setValue(double)));

	QPushButton *startUseCurrentButton = new QPushButton("Use Current");
	connect(startUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetStartPosition()));

	QHBoxLayout *startPointLayout = new QHBoxLayout;
	startPointLayout->addWidget(new QLabel("Start:"));
	startPointLayout->addWidget(hStart_);
	startPointLayout->addWidget(vStart_);
	startPointLayout->addWidget(startUseCurrentButton);

	hEnd_ = buildPositionDoubleSpinBox("H: ", " mm", config_->xEnd(), 3);
	connect(hEnd_, SIGNAL(editingFinished()), this, SLOT(onXEndChanged()));
	connect(config_, SIGNAL(xEndChanged(double)), hEnd_, SLOT(setValue(double)));

	vEnd_ = buildPositionDoubleSpinBox("V: ", " mm", config_->yEnd(), 3);
	connect(vEnd_, SIGNAL(editingFinished()), this, SLOT(onYEndChanged()));
	connect(config_, SIGNAL(yEndChanged(double)), vEnd_, SLOT(setValue(double)));

	QPushButton *endUseCurrentButton = new QPushButton("Use Current");
	connect(endUseCurrentButton, SIGNAL(clicked()), this, SLOT(onSetEndPosition()));

	QHBoxLayout *endPointLayout = new QHBoxLayout;
	endPointLayout->addWidget(new QLabel("End:"));
	endPointLayout->addWidget(hEnd_);
	endPointLayout->addWidget(vEnd_);
	endPointLayout->addWidget(endUseCurrentButton);

	hStep_ = buildPositionDoubleSpinBox("H: ", QString(" %1").arg(QString::fromUtf8("µm")), config_->xStep()*1000, 1);	// xStep needs to be in mm.
	connect(hStep_, SIGNAL(editingFinished()), this, SLOT(onXStepChanged()));
	connect(config_, SIGNAL(xStepChanged(double)), this, SLOT(updateXStep(double)));

	vStep_ = buildPositionDoubleSpinBox("V: ", QString(" %1").arg(QString::fromUtf8("µm")), config_->yStep()*1000, 1);	// yStep needs to be in mm.
	connect(vStep_, SIGNAL(editingFinished()), this, SLOT(onYStepChanged()));
	connect(config_, SIGNAL(yStepChanged(double)), this, SLOT(updateYStep(double)));

	QHBoxLayout *stepSizeLayout = new QHBoxLayout;
	stepSizeLayout->addWidget(new QLabel("Step Size:"));
	stepSizeLayout->addWidget(hStep_);
	stepSizeLayout->addWidget(vStep_);
	stepSizeLayout->addStretch();

	wireStart_ = buildPositionDoubleSpinBox("", " mm", config_->zStart(), 3);
	connect(wireStart_, SIGNAL(editingFinished()), this, SLOT(onZStartChanged()));
	connect(config_, SIGNAL(zStartChanged(double)), wireStart_, SLOT(setValue(double)));

	QPushButton *wireStartCurrentButton = new QPushButton("Use Current");
	connect(wireStartCurrentButton, SIGNAL(clicked()), this, SLOT(onSetWireStartPosition()));

	QHBoxLayout *wireStartLayout = new QHBoxLayout;
	wireStartLayout->addWidget(new QLabel("Wire Start:"));
	wireStartLayout->addWidget(wireStart_, 0, Qt::AlignCenter);
	wireStartLayout->addWidget(wireStartCurrentButton);

	wireEnd_ = buildPositionDoubleSpinBox("", " mm", config_->zEnd(), 3);
	connect(wireEnd_, SIGNAL(editingFinished()), this, SLOT(onZEndChanged()));
	connect(config_, SIGNAL(zEndChanged(double)), wireEnd_, SLOT(setValue(double)));

	QPushButton *wireEndCurrentButton = new QPushButton("Use Current");
	connect(wireEndCurrentButton, SIGNAL(clicked()), this, SLOT(onSetWireEndPosition()));

	QHBoxLayout *wireEndLayout = new QHBoxLayout;
	wireEndLayout->addWidget(new QLabel("Wire End:"));
	wireEndLayout->addWidget(wireEnd_, 0, Qt::AlignCenter);
	wireEndLayout->addWidget(wireEndCurrentButton);

	wireStep_ = buildPositionDoubleSpinBox("", QString(" %1").arg(QString::fromUtf8("µm")), config_->zStep()*1000, 1);	// yStep needs to be in mm.
	connect(wireStep_, SIGNAL(editingFinished()), this, SLOT(onZStepChanged()));
	connect(config_, SIGNAL(zStepChanged(double)), this, SLOT(updateZStep(double)));

	QHBoxLayout *wireStepLayout = new QHBoxLayout;
	wireStepLayout->addWidget(new QLabel("Wire Step:"));
	wireStepLayout->addWidget(wireStep_);

	mapInfo_ = new QLabel;
	updateMapInfo();

	QVBoxLayout *positionsLayout = new QVBoxLayout;
	positionsLayout->addLayout(startPointLayout);
	positionsLayout->addLayout(endPointLayout);
	positionsLayout->addLayout(stepSizeLayout);
	positionsLayout->addLayout(wireStartLayout);
	positionsLayout->addLayout(wireEndLayout);
	positionsLayout->addLayout(wireStepLayout);
	positionsLayout->addWidget(mapInfo_);

	positionsBox->setLayout(positionsLayout);

	// Dwell time.
	dwellTime_ = addDwellTimeWidget(config_->timeStep());
	connect(dwellTime_, SIGNAL(editingFinished()), this, SLOT(onDwellTimeChanged()));
	connect(config_, SIGNAL(timeStepChanged(double)), dwellTime_, SLOT(setValue(double)));

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(new QLabel("Dwell Time:"));
	timeLayout->addWidget(dwellTime_);

	// The estimated scan time.
	estimatedTime_ = new QLabel;
	connect(config_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged()));
	onEstimatedTimeChanged();

	QVBoxLayout *timeBoxLayout = new QVBoxLayout;
	timeBoxLayout->addLayout(timeLayout);
	timeBoxLayout->addWidget(estimatedTime_);

	QGroupBox *timeGroupBox = new QGroupBox("Time");
	timeGroupBox->setLayout(timeBoxLayout);

	// Using the CCD.
	QGroupBox *ccdBox = addCCDDetectorSelectionView();
	connect(ccdButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onCCDDetectorChanged(int)));
	connect(config_->dbObject(), SIGNAL(ccdDetectorChanged(int)), this, SLOT(updateCCDDetectorButtons(int)));
	ccdButtonGroup_->button(int(config_->ccdDetector()))->setChecked(true);
	ccdButtonGroup_->button((int)VESPERS::Roper)->hide();
	ccdButtonGroup_->button((int)VESPERS::Mar)->hide();

	configureCCDButton_ = new QPushButton(QIcon(":/hammer-wrench.png"), "Configure Area Detector");
	configureCCDButton_->setEnabled(config_->ccdDetector());
	connect(configureCCDButton_, SIGNAL(clicked()), this, SLOT(onConfigureCCDDetectorClicked()));

	// The fluorescence detector setup
	QGroupBox *fluorescenceDetectorGroupBox  = addFluorescenceDetectorSelectionView();
	fluorescenceButtonGroup_->button(int(VESPERS::NoXRF))->setDisabled(true);
	connect(fluorescenceButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onFluorescenceChoiceChanged(int)));
	connect(config_->dbObject(), SIGNAL(fluorescenceDetectorChanged(int)), this, SLOT(updateFluorescenceDetector(int)));
	fluorescenceButtonGroup_->button((int)config_->fluorescenceDetector())->setChecked(true);

	// Ion chamber selection
	QGroupBox *I0GroupBox = addI0SelectionView();
	connect(I0Group_, SIGNAL(buttonClicked(int)), this, SLOT(onI0Clicked(int)));
	connect(config_->dbObject(), SIGNAL(incomingChoiceChanged(int)), this, SLOT(updateI0Buttons(int)));
	I0Group_->button((int)config_->incomingChoice())->click();

	// Motor selection.
	QGroupBox *motorSetChoiceBox = addMotorSelectionView(QStringList() << "H and V", QList<int>() << (VESPERS::H | VESPERS::V));
	connect(motorButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onMotorChanged(int)));
	motorButtonGroup_->button(int(config_->motor()))->click();

	// CCD label.
	ccdText_ = new QLabel;
	ccdHelpText_ = new QLabel;
	ccdTextBox_ = new QGroupBox("CCD Detector Info");
	QVBoxLayout *ccdTextLayout = new QVBoxLayout;
	ccdTextLayout->addWidget(ccdText_);
	ccdTextLayout->addWidget(ccdHelpText_);
	ccdTextBox_->setLayout(ccdTextLayout);
	ccdTextBox_->setVisible(config_->ccdDetector() != VESPERS::NoCCD);

	// Scan name selection
	scanName_ = addScanNameView(config_->name());
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(config_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
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

	QGroupBox *timeOffsetBox = addTimeOffsetLabel(config_->timeOffset());
	connect(timeOffset_, SIGNAL(valueChanged(double)), this, SLOT(setTimeOffset(double)));

	// Auto-export option.
	QGroupBox *autoExportGroupBox = addExporterOptionsView(QStringList() << "Ascii" << "SMAK", config_->exportSpectraSources(), config_->exportSpectraInRows());
	connect(autoExportButtonGroup_, SIGNAL(buttonClicked(int)), this, SLOT(updateAutoExporter(int)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), config_, SLOT(setExportSpectraSources(bool)));
	connect(autoExportSpectra_, SIGNAL(toggled(bool)), exportSpectraInRows_, SLOT(setEnabled(bool)));
	connect(exportSpectraInRows_, SIGNAL(toggled(bool)), this, SLOT(updateExportSpectraInRows(bool)));
	autoExportButtonGroup_->button(config_->exportAsAscii() ? 0 : 1)->click();

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
	config_->setName(name);
	config_->setUserScanName(name);

	if (config_->ccdDetector() != VESPERS::NoCCD){

		QString path;

		if (config_->ccdDetector() == VESPERS::Roper)
			path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();

		else if (config_->ccdDetector() == VESPERS::Mar)
			path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

		else if (config_->ccdDetector() == VESPERS::Pilatus)
			path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

		ccdText_->setText(QString("Path: %1\nName: %2").arg(path).arg(name));
		config_->setCCDFileName(name);
		checkCCDFileNames(name);
	}

	if (config_->ccdDetector() == VESPERS::Pilatus && name.contains(" ")){

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

	if (config_->ccdDetector() == VESPERS::Roper){

		path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();
		path.replace("Y:\\", "/nas/vespers/");
		path.replace('\\', '/');
	}

	else if (config_->ccdDetector() == VESPERS::Mar)
		path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

	else if (config_->ccdDetector() == VESPERS::Pilatus)
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
	config_->setFluorescenceDetector(id);
	updateRoiText();
}

void VESPERS3DScanConfigurationView::onMotorChanged(int id)
{
	config_->setMotor(id);
}

void VESPERS3DScanConfigurationView::onCCDDetectorChanged(int id)
{
	config_->setCCDDetector(id);

	if (config_->ccdDetector() != VESPERS::NoCCD){

		QString path;
		QString name = config_->ccdFileName().isEmpty() ? scanName_->text() : config_->ccdFileName();

		if (config_->ccdDetector() == VESPERS::Roper)
			path = VESPERSBeamline::vespers()->vespersRoperCCD()->ccdFilePath();

		else if (config_->ccdDetector() == VESPERS::Mar)
			path = VESPERSBeamline::vespers()->vespersMarCCD()->ccdFilePath();

		else if (config_->ccdDetector() == VESPERS::Pilatus)
			path = VESPERSBeamline::vespers()->vespersPilatusAreaDetector()->ccdFilePath();

		config_->setCCDFileName(name);
		ccdText_->setText(QString("Path: %1\nName: %2").arg(path).arg(name));
		checkCCDFileNames(name);
	}

	onScanNameEdited();
	ccdTextBox_->setVisible(config_->ccdDetector() != VESPERS::NoCCD);
	configureCCDButton_->setDisabled(config_->ccdDetector() == VESPERS::NoCCD);
}

void VESPERS3DScanConfigurationView::updateRoiText()
{
	switch((int)config_->fluorescenceDetector()){

	case VESPERS::NoXRF:
		config_->setRoiInfoList(AMROIInfoList());
		break;

	case VESPERS::SingleElement:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList());
		break;

	case VESPERS::FourElement:
		config_->setRoiInfoList(*VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList());
		break;

	case VESPERS::SingleElement | VESPERS::FourElement:{

		AMROIInfoList list;
		AMROIInfoList singleElList = *VESPERSBeamline::vespers()->vortexXRF1E()->roiInfoList();
		AMROIInfoList fourElList = *VESPERSBeamline::vespers()->vortexXRF4E()->roiInfoList();

		for (int i = 0, count = singleElList.count(); i < count; i++)
			list.append(singleElList.at(i));

		for (int i = 0, count = fourElList.count(); i < count; i++)
			list.append(fourElList.at(i));

		config_->setRoiInfoList(list);
		break;
	}
	}

	updateAndSetRoiTextBox(int(config_->fluorescenceDetector()));
}

void VESPERS3DScanConfigurationView::onEstimatedTimeChanged()
{
	estimatedTime_->setText("Estimated time per scan:\t" + VESPERS::convertTimeToString(config_->totalTime()));
}

void VESPERS3DScanConfigurationView::onSetStartPosition()
{
	double h = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl()->value();
	double v = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl()->value();
	double n = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->normalControl()->value();

	config_->setXStart(h);
	hStart_->setValue(h);
	config_->setYStart(v);
	vStart_->setValue(v);
	config_->setNormalPosition(n);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onSetEndPosition()
{
	double h = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->horizontalControl()->value();
	double v = VESPERSBeamline::vespers()->pseudoSampleStageMotorGroupObject()->verticalControl()->value();

	config_->setXEnd(h);
	hEnd_->setValue(h);
	config_->setYEnd(v);
	vEnd_->setValue(v);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onSetWireStartPosition()
{
	double wire = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalControl()->value();

	config_->setZStart(wire);
	wireStart_->setValue(wire);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onSetWireEndPosition()
{
	double wire = VESPERSBeamline::vespers()->pseudoWireStageMotorGroupObject()->verticalControl()->value();

	config_->setZEnd(wire);
	wireEnd_->setValue(wire);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onXStartChanged()
{
	config_->setXStart(hStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onXEndChanged()
{
	config_->setXEnd(hEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onXStepChanged()
{
	config_->setXStep(hStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onYStartChanged()
{
	config_->setYStart(vStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onYEndChanged()
{
	config_->setYEnd(vEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onYStepChanged()
{
	config_->setYStep(vStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onZStartChanged()
{
	config_->setZStart(wireStart_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onZEndChanged()
{
	config_->setZEnd(wireEnd_->value());
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onZStepChanged()
{
	config_->setZStep(wireStep_->value()/1000);
	updateMapInfo();
	axesAcceptable();
}

void VESPERS3DScanConfigurationView::onDwellTimeChanged()
{
	config_->setTimeStep(dwellTime_->value());
}

void VESPERS3DScanConfigurationView::updateMapInfo()
{
	double hSize = fabs(config_->xEnd()-config_->xStart());
	double vSize = fabs(config_->yEnd()-config_->yStart());
	double wireSize = fabs(config_->zEnd()-config_->zStart());

	int hPoints = int((hSize)/config_->xStep());
	if ((hSize - (hPoints + 0.01)*config_->xStep()) < 0)
		hPoints += 1;
	else
		hPoints += 2;

	int vPoints = int((vSize)/config_->yStep());
	if ((vSize - (vPoints + 0.01)*config_->yStep()) < 0)
		vPoints += 1;
	else
		vPoints += 2;

	int wirePoints = int((wireSize)/config_->zStep());
	if ((vSize - (wirePoints + 0.01)*config_->zStep()) < 0)
		wirePoints += 1;
	else
		wirePoints += 2;

	mapInfo_->setText(QString("Map Size: %1 %2 x %3 %2 x %6 %2\t Points: %4 x %5 x %7")
					  .arg(QString::number(hSize*1000, 'f', 1))
					  .arg(QString::fromUtf8("µm"))
					  .arg(QString::number(vSize*1000, 'f', 1))
					  .arg(hPoints)
					  .arg(vPoints)
					  .arg(QString::number(wireSize*1000, 'f', 1))
					  .arg(wirePoints)
					  );
}

void VESPERS3DScanConfigurationView::axesAcceptable()
{
	QPalette good(this->palette());
	QPalette bad(good);
	bad.setColor(QPalette::Base, Qt::red);

	hStep_->setPalette(config_->validXAxis() ? good : bad);
	vStep_->setPalette(config_->validYAxis() ? good : bad);
	wireStep_->setPalette(config_->validZAxis() ? good : bad);
}
