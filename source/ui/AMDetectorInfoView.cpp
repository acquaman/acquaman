#include "AMDetectorInfoView.h"

#include <QCheckBox>
#include <QLabel>

AMDetectorInfoView::AMDetectorInfoView(AMDetectorInfo *detectorInfo, AMDetectorInfo *writeDetectorInfo, bool interactive, QWidget *parent) :
		QGroupBox(parent)
{
	detectorInfo_ = detectorInfo;
	if(!writeDetectorInfo)
		writeDetectorInfo_ = detectorInfo_;
	else
		writeDetectorInfo_ = writeDetectorInfo;

	interactive_ = interactive;
	vl_ = new QVBoxLayout();
	hl_ = new QHBoxLayout();
	hl_->setObjectName("Detector Horizontal Layout");
	switchToEditBox_ = new QPushButton("Switch to Edit Mode");
	switchToEditBox_->setEnabled(false);
	QSpacerItem *spc1 = new QSpacerItem(10, 10, QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
	hl_->addSpacerItem(spc1);
	hl_->addWidget(switchToEditBox_, 0, Qt::AlignRight);
	vl_->addLayout(hl_);
}

PGTDetectorInfoView::PGTDetectorInfoView(PGTDetectorInfo *detectorInfo, AMDetectorInfo *writeDetectorInfo, bool interactive, QWidget *parent) :
		AMDetectorInfoView(detectorInfo, writeDetectorInfo, interactive, parent)
{
	sDetectorInfo_ = detectorInfo;
	interactive_ = interactive;
	setTitle("PGT Silicon Drift Detector");
	fl_ = new QFormLayout();
	QHBoxLayout *tmpHB;
	tmpHB = new QHBoxLayout();
	integrationTimeBox_ = new QDoubleSpinBox();
	integrationTimeBox_->setValue(sDetectorInfo_->integrationTime());
	integrationTimeBox_->setEnabled(interactive_);
	QPair<double, double> itRange = sDetectorInfo_->integrationTimeRange();
	integrationTimeBox_->setRange(itRange.first, itRange.second);
	connect(integrationTimeBox_, SIGNAL(valueChanged(double)), (PGTDetectorInfo*)writeDetectorInfo_, SLOT(setIntegrationTime(double)));
	tmpHB->addWidget(integrationTimeBox_);
	fl_->addRow("Integration Time", tmpHB);
	tmpHB = new QHBoxLayout();
	integrationModeBox_ = new QComboBox();
	integrationModeBox_->addItems(sDetectorInfo_->integrationModeList());
	integrationModeBox_->setCurrentIndex( sDetectorInfo_->integrationModeList().indexOf(sDetectorInfo_->integrationMode()) );
	integrationModeBox_->setEnabled(interactive_);
	tmpHB->addWidget(integrationModeBox_);
	connect(integrationModeBox_, SIGNAL(currentIndexChanged(QString)), (PGTDetectorInfo*)writeDetectorInfo_, SLOT(setIntegrationMode(QString)));
	fl_->addRow("Integration Mode", tmpHB);
	tmpHB = new QHBoxLayout();
	hvSetpointBox_ = new QDoubleSpinBox();
	hvSetpointBox_->setValue(sDetectorInfo_->hvSetpoint());
	hvSetpointBox_->setEnabled(interactive_);
	QPair<double, double> hvRange = sDetectorInfo_->hvSetpointRange();
	hvSetpointBox_->setRange(hvRange.first, hvRange.second);
	connect(hvSetpointBox_, SIGNAL(valueChanged(double)), (PGTDetectorInfo*)writeDetectorInfo_, SLOT(setHVSetpoint(double)));
	tmpHB->addWidget(hvSetpointBox_);
	fl_->addRow("HV Setpoint", tmpHB);
	allBoxes_.append(integrationTimeBox_);
	allBoxes_.append(integrationModeBox_);
	allBoxes_.append(hvSetpointBox_);
	vl_->insertLayout(0, fl_);
	setLayout(vl_);
	setMinimumWidth(250);
}

MCPDetectorInfoView::MCPDetectorInfoView(MCPDetectorInfo *detectorInfo, AMDetectorInfo *writeDetectorInfo, bool interactive, QWidget *parent) :
		AMDetectorInfoView(detectorInfo, writeDetectorInfo, interactive, parent)
{
	sDetectorInfo_ = detectorInfo;
	interactive_ = interactive;
	setTitle("MCP Total Fluoresence Yield");
	fl_ = new QFormLayout();
	QHBoxLayout *tmpHB;
	tmpHB = new QHBoxLayout();
	hvSetpointBox_ = new QDoubleSpinBox();
	hvSetpointBox_->setValue(sDetectorInfo_->hvSetpoint());
	hvSetpointBox_->setEnabled(interactive_);
	QPair<double, double> hvRange = sDetectorInfo_->hvSetpointRange();
	hvSetpointBox_->setRange(hvRange.first, hvRange.second);
	tmpHB->addWidget(hvSetpointBox_);
	connect(hvSetpointBox_, SIGNAL(valueChanged(double)), (MCPDetectorInfo*)writeDetectorInfo_, SLOT(setHVSetpoint(double)));
	fl_->addRow("HV Setpoint", tmpHB);
	allBoxes_.append(hvSetpointBox_);
	vl_->insertLayout(0, fl_);
	setLayout(vl_);
	setMinimumWidth(250);
}

AMDetectorInfoSetView::AMDetectorInfoSetView(AMDetectorInfoSet *viewSet, AMDetectorInfoSet *writeSet, bool setup, QWidget *parent) :
		QGroupBox(parent)
{
	viewSet_ = viewSet;
	if(!writeSet)
		writeSet_ = viewSet_;
	else
		writeSet_ = writeSet;

	setTitle(viewSet->name());
	if(setup)
		runSetup();
}



void AMDetectorInfoSetView::runSetup(){
	QGridLayout *gl = new QGridLayout();
	QCheckBox *tmpBox;
	QPushButton *tmpDetails;
	QLabel *tmpLabel;
	AMDetectorInfo *tmpDetector;
	for(int x = 0; x < viewSet_->count(); x++){
		tmpDetector = viewSet_->detectorAt(x);
		tmpBox = new QCheckBox();
		tmpBox->setChecked(viewSet_->isDefaultAt(x));
		tmpDetails = new QPushButton("Details");
		if( !tmpDetector->hasDetails() )
			tmpDetails->setEnabled(false);
		tmpLabel = new QLabel(tmpDetector->description());
		gl->addWidget(tmpLabel, x, 0, 1, 1, Qt::AlignLeft);
		gl->addWidget(tmpBox, x, 1, 1, 1, Qt::AlignLeft);
		gl->addWidget(tmpDetails, x, 2, 1, 1, Qt::AlignLeft);
		detectorBoxes_.append(tmpBox);
		detectorDetails_.append(tmpDetails);
		detailViews_.append(detailViewByType(tmpDetector, writeSet_->detectorAt(x)));
		detailViews_.last()->hide();
		connect(tmpDetails, SIGNAL(clicked()), detailViews_.last(), SLOT(show()));
		connect(tmpDetails, SIGNAL(clicked()), detailViews_.last(), SLOT(raise()));
	}
	hl_ = new QHBoxLayout(this);
	hl_->addLayout(gl);
	setLayout(hl_);
}

QWidget* AMDetectorInfoSetView::detailViewByType(AMDetectorInfo *detectorInfo, AMDetectorInfo *writeDetectorInfo){
	#warning "David... please review. Needed to change because of removed AMDbObject::typeDescription."

	/* typeDescription was never the safest way to tell what type a class was anyway... nor the fastest.
	if(detectorInfo->typeDescription() == "PGT SDD Spectrum-Output Detector")
		return new PGTDetectorInfoView( (PGTDetectorInfo*)detectorInfo, writeDetectorInfo, true);
	else if(detectorInfo->typeDescription() == "MCP Detector")
		return new MCPDetectorInfoView( (MCPDetectorInfo*)detectorInfo, writeDetectorInfo, true );
	else
		return new QGroupBox();
		*/
	PGTDetectorInfo* pgtDetectorInfo;
	MCPDetectorInfo* mcpDetectorInfo;

	if( (pgtDetectorInfo = qobject_cast<PGTDetectorInfo*>(detectorInfo)) )
		return new PGTDetectorInfoView(pgtDetectorInfo, writeDetectorInfo, true);
	else if( (mcpDetectorInfo = qobject_cast<MCPDetectorInfo*>(detectorInfo)) )
		return new MCPDetectorInfoView(mcpDetectorInfo, writeDetectorInfo, true);
	else
		return new QGroupBox();

}
