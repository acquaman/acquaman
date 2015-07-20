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


#include "SGMSidebar.h"

#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QToolButton>

#include "ui/beamline/AMControlButton.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"

SGMSidebar::SGMSidebar(QWidget *parent) :
	QWidget(parent)
{
	mainBox_ = new QGroupBox("SGM Beamline");
	mainLayout_ = new QVBoxLayout();
	mainLayout_->addWidget(mainBox_);
	gl_ = new QGridLayout();
	mainBox_->setLayout(gl_);

	setMaximumWidth(350);

	readyLabel_ = new AMExtendedControlEditor(SGMBeamline::sgm()->beamlineReady(), NULL, true);
	readyLabel_->setNoUnitsBox(true);
	readyLabel_->overrideTitle("");
	readyLabel_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	beamOnAction_ = 0;//NULL
	beamOnButton_ = new QToolButton();
	beamOnButton_->setText("Beam On");
	beamOnButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnButtonClicked()));
	beamOffCButton_ = new AMControlButton(SGMBeamline::sgm()->fastShutterVoltage());
	beamOffCButton_->overrideText("Beam Off");
	beamOffCButton_->setDownValue(5);
	beamOffCButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	stopMotorsAction_ = 0;//NULL
	stopMotorsButton_ = new QToolButton();
	stopMotorsButton_->setText("Emergency\nMotor Stop");
	stopMotorsButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(stopMotorsButton_, SIGNAL(clicked()), this, SLOT(onStopMotorsButtonClicked()));
	closeVacuumButton_ = new QToolButton();
	closeVacuumButton_->setText("Close Vacuum");
	closeVacuumButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(closeVacuumButton_, SIGNAL(clicked()), this, SLOT(onCloseVacuumButtonClicked()));
	visibleLightButton_ = new QToolButton();
	visibleLightButton_->setText("Visible Light");
	visibleLightButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(visibleLightButton_, SIGNAL(clicked()), this, SLOT(onVisibleLightClicked()));
	connect(SGMBeamline::sgm(), SIGNAL(visibleLightStatusChanged(QString)), this, SLOT(onVisibleLightStatusChanged(QString)));
	energyNC_ = new AMExtendedControlEditor(SGMBeamline::sgm()->energy(), SGMBeamline::sgm()->energyMovingStatus());
	energyNC_->setControlFormat('f', 2);
	energyNC_->overrideTitle("Energy");
	energyNC_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	trackUndulatorCButton_ = new AMControlButton(SGMBeamline::sgm()->undulatorTracking());
	trackUndulatorCButton_->overrideText("Undulator");
	trackUndulatorCButton_->setCheckable(true);
	trackUndulatorCButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	trackGratingCButton_ = new AMControlButton(SGMBeamline::sgm()->monoTracking());
	trackGratingCButton_->overrideText("Grating");
	trackGratingCButton_->setCheckable(true);
	trackGratingCButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	trackExitSlitCButton_ = new AMControlButton(SGMBeamline::sgm()->exitSlitTracking());
	trackExitSlitCButton_->overrideText("Exit Slit");
	trackExitSlitCButton_->setCheckable(true);
	trackExitSlitCButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	gratingNC_ = new AMExtendedControlEditor(SGMBeamline::sgm()->grating());
	gratingNC_->overrideTitle("Grating");
	gratingNC_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	exitSlitNC_ = new AMExtendedControlEditor(SGMBeamline::sgm()->exitSlitGap());
	exitSlitNC_->setControlFormat('f', 1);
	exitSlitNC_->overrideTitle("Exit Slit");
	exitSlitNC_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	restoreBeamlineAction_ = 0; //NULL
	restoreBeamlineSettingsButton_ = new QPushButton("Restore Settings");
	connect(restoreBeamlineSettingsButton_, SIGNAL(clicked()), this, SLOT(onRestoreBeamlineSettingsButtonClicked()));

	scanningLabel_ = new AMExtendedControlEditor(SGMBeamline::sgm()->beamlineScanning(), NULL, true);
	scanningLabel_->setNoUnitsBox(true);
	scanningLabel_->overrideTitle("");
	scanningLabel_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	scanningResetButton_ = new QToolButton();
	scanningResetButton_->setText("Reset");
	scanningResetButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(scanningResetButton_, SIGNAL(clicked()), this, SLOT(onScanningResetButtonClicked()));
	QHBoxLayout *shl = new QHBoxLayout();
//	shl->addWidget(scanningLabel_);
	shl->addWidget(scanningResetButton_);
	shl->addWidget(restoreBeamlineSettingsButton_);
	shl->setSpacing(0);
	shl->setContentsMargins(2, 2, 2, 2);

	controlsConnectedLabel_ = new QLabel();
	detectorsConnectedLabel_ = new QLabel();
	QVBoxLayout *vhl = new QVBoxLayout();
	QHBoxLayout *tmpHL = new QHBoxLayout();
	tmpHL->addWidget(controlsConnectedLabel_);
	tmpHL->addWidget(new QLabel("  Controls"));
	tmpHL->addStretch(10);
	vhl->addLayout(tmpHL);
	tmpHL = new QHBoxLayout();
	tmpHL->addWidget(detectorsConnectedLabel_);
	tmpHL->addWidget(new QLabel("  Detectors"));
	tmpHL->addStretch(10);
	vhl->addLayout(tmpHL);
	vhl->setSpacing(0);
	vhl->setContentsMargins(2, 2, 2, 2);

	onBeamlineCriticalControlSetConnectedChanged(SGMBeamline::sgm()->criticalControlsSet()->isConnected());
	onBeamlineCriticalDetectorSetConnectedChanged(SGMBeamline::sgm()->criticalDetectorSet()->isConnnected());

	beamlineWarningsLabel_ = new QLabel(SGMBeamline::sgm()->beamlineWarnings());
	connect(SGMBeamline::sgm(), SIGNAL(beamlineWarningsChanged(QString)), beamlineWarningsLabel_, SLOT(setText(QString)));
	connect(SGMBeamline::sgm(), SIGNAL(beamlineWarningsChanged(QString)), this, SLOT(onBeamlineWarnings(QString)));

	connect(SGMBeamline::sgm()->criticalControlsSet(), SIGNAL(connected(bool)), this, SLOT(onBeamlineCriticalControlSetConnectedChanged(bool)));
	connect(SGMBeamline::sgm()->criticalDetectorSet(), SIGNAL(connected(bool)), this, SLOT(onBeamlineCriticalDetectorSetConnectedChanged(bool)));

	i0CheckBox_ = new QCheckBox("I0");
	teyCheckBox_ = new QCheckBox("TEY");
	tfyCheckBox_ = new QCheckBox("TFY");
	pdCheckBox_ = new QCheckBox("PD");
	fpd1CheckBox_ = new QCheckBox("FPD1");
	fpd2CheckBox_ = new QCheckBox("FPD2");
	fpd3CheckBox_ = new QCheckBox("FPD3");
	fpd4CheckBox_ = new QCheckBox("FPD4");

	// create UI elements
	imageView_ = new MPlotWidget();
	imageView_->setMinimumHeight(200);
	imagePlot_ = new MPlot();

	QHBoxLayout *checkBoxHL1 = new QHBoxLayout();
	checkBoxHL1->addWidget(i0CheckBox_);
	checkBoxHL1->addWidget(teyCheckBox_);
	checkBoxHL1->addWidget(tfyCheckBox_);
	checkBoxHL1->addWidget(pdCheckBox_);
	checkBoxHL1->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout *checkBoxHL2 = new QHBoxLayout();
	checkBoxHL2->addWidget(fpd1CheckBox_);
	checkBoxHL2->addWidget(fpd2CheckBox_);
	checkBoxHL2->addWidget(fpd3CheckBox_);
	checkBoxHL2->addWidget(fpd4CheckBox_);
	checkBoxHL2->setContentsMargins(0, 0, 0, 0);

	QVBoxLayout *checkBoxVL = new QVBoxLayout();
	checkBoxVL->addLayout(checkBoxHL1);
	checkBoxVL->addLayout(checkBoxHL2);
	checkBoxVL->setContentsMargins(1, 1, 1, 1);

	autoScaleCheckBox_ = new QCheckBox("Auto");
	logCheckBox_ = new QCheckBox("Log");
	QVBoxLayout *autoLogVL = new QVBoxLayout();
	autoLogVL->addWidget(autoScaleCheckBox_);
	autoLogVL->addWidget(logCheckBox_);
	autoLogVL->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout *masterCheckBoxHL = new QHBoxLayout();
	masterCheckBoxHL->addLayout(checkBoxVL);
	masterCheckBoxHL->addLayout(autoLogVL);
	masterCheckBoxHL->setContentsMargins(0, 0, 0, 0);

	plotLeftAxisMinimum_ = 0;
	plotLeftAxisMaximum_ = 1000000;
	minSpinBox_ = new QDoubleSpinBox();
	minSpinBox_->setRange(0, 1100000);
	minSpinBox_->setValue(plotLeftAxisMinimum_);
	maxSpinBox_ = new QDoubleSpinBox();
	maxSpinBox_->setRange(1, 1100001);
	maxSpinBox_->setValue(plotLeftAxisMaximum_);
	minLabel_ = new QLabel("Min");
	maxLabel_ = new QLabel("Max");

	QHBoxLayout *minHL = new QHBoxLayout();
	minHL->addWidget(minLabel_);
	minHL->addWidget(minSpinBox_);
	minHL->setContentsMargins(0, 0, 0, 0);
	QHBoxLayout *maxHL = new QHBoxLayout();
	maxHL->addWidget(maxLabel_);
	maxHL->addWidget(maxSpinBox_);
	maxHL->setContentsMargins(0, 0, 0, 0);
	QHBoxLayout *rangeHL = new QHBoxLayout();
	rangeHL->addLayout(minHL);
	rangeHL->addLayout(maxHL);
	rangeHL->setContentsMargins(0, 0, 0, 0);

	QVBoxLayout *plotOptionsVL = new QVBoxLayout();
	plotOptionsVL->addLayout(masterCheckBoxHL);
	plotOptionsVL->addLayout(rangeHL);
	plotOptionsVL->setContentsMargins(0, 0, 0, 0);

	plotLayout_ = new QVBoxLayout();
	plotLayout_->addWidget(imageView_);
	plotLayout_->addLayout(plotOptionsVL);
	plotLayout_->setContentsMargins(1, 1, 1, 1);

	// ATTENTION DAVE: New special axis scale on bottom of strip tool plot:
	stripToolSpecialAxisScale_ = new MPlotAxisScale(Qt::Horizontal);
	stripToolSpecialAxisScale_->setPadding(2);
	imagePlot_->addAxisScale(stripToolSpecialAxisScale_);
	imagePlot_->axisBottom()->setAxisScale(stripToolSpecialAxisScale_);	// have the bottom (visible) axis display this axisScale instead
	/////////////////

	MPlotAxisRange leftRange;
	leftRange.setRange(plotLeftAxisMinimum_, plotLeftAxisMaximum_);


	imageView_->setPlot(imagePlot_);

	imagePlot_->axisScaleLeft()->setAutoScaleEnabled(false);
	imagePlot_->axisScaleLeft()->setDataRange(leftRange);

	imagePlot_->axisScaleLeft()->setPadding(2);
	imagePlot_->axisScaleBottom()->setAutoScaleEnabled();
	imagePlot_->axisScaleBottom()->setPadding(2);
	imagePlot_->setMarginBottom(10);
	imagePlot_->setMarginLeft(15);
	imagePlot_->setMarginRight(5);
	imagePlot_->setMarginTop(5);
	imagePlot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	imagePlot_->axisRight()->setTicks(4);
	imagePlot_->axisBottom()->setTicks(4);
	imagePlot_->axisLeft()->showGrid(false);
	imagePlot_->axisBottom()->showAxisName(false);
	imagePlot_->axisLeft()->showAxisName(false);

	i0Model_ = new MPlotRealtimeModel(this);
	teyModel_ = new MPlotRealtimeModel(this);
	tfyModel_ = new MPlotRealtimeModel(this);
	pdModel_ = new MPlotRealtimeModel(this);
	fpd1Model_ = new MPlotRealtimeModel(this);
	fpd2Model_ = new MPlotRealtimeModel(this);
	fpd3Model_ = new MPlotRealtimeModel(this);
	fpd4Model_ = new MPlotRealtimeModel(this);

	i0Series_ = new MPlotSeriesBasic(i0Model_);
	i0Series_->setDescription("I0");
	i0Series_->setLinePen(QPen(QColor(255, 0, 0)));
	i0Series_->setMarker(MPlotMarkerShape::None);
	teySeries_ = new MPlotSeriesBasic(teyModel_);
	teySeries_->setDescription("TEY");
	teySeries_->setLinePen(QPen(QColor(0, 255, 0)));
	teySeries_->setMarker(MPlotMarkerShape::None);
	tfySeries_ = new MPlotSeriesBasic(tfyModel_);
	tfySeries_->setDescription("TFY");
	tfySeries_->setLinePen(QPen(QColor(0, 0, 255)));
	tfySeries_->setMarker(MPlotMarkerShape::None);
	pdSeries_ = new MPlotSeriesBasic(pdModel_);
	pdSeries_->setDescription("PD");
	pdSeries_->setLinePen(QPen(QColor(255, 0, 255)));
	pdSeries_->setMarker(MPlotMarkerShape::None);

	fpd1Series_ = new MPlotSeriesBasic(fpd1Model_);
	fpd1Series_->setDescription("FPD1");
	fpd1Series_->setLinePen(QPen(QColor(255, 255, 0)));
	fpd1Series_->setMarker(MPlotMarkerShape::None);
	fpd2Series_ = new MPlotSeriesBasic(fpd2Model_);
	fpd2Series_->setDescription("FPD2");
	fpd2Series_->setLinePen(QPen(QColor(0, 255, 255)));
	fpd2Series_->setMarker(MPlotMarkerShape::None);
	fpd3Series_ = new MPlotSeriesBasic(fpd3Model_);
	fpd3Series_->setDescription("FPD3");
	fpd3Series_->setLinePen(QPen(QColor(255, 127, 127)));
	fpd3Series_->setMarker(MPlotMarkerShape::None);
	fpd4Series_ = new MPlotSeriesBasic(fpd4Model_);
	fpd4Series_->setDescription("FPD4");
	fpd4Series_->setLinePen(QPen(QColor(127, 255, 127)));
	fpd4Series_->setMarker(MPlotMarkerShape::None);

	imagePlot_->addItem(i0Series_);
	i0CheckBox_->setChecked(true);

	connect(i0CheckBox_, SIGNAL(toggled(bool)), this, SLOT(onI0CheckBoxToggled(bool)));
	connect(teyCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onTEYCheckBoxToggled(bool)));
	connect(tfyCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onTFYCheckBoxToggled(bool)));
	connect(pdCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onPDCheckBoxToggled(bool)));
	connect(fpd1CheckBox_, SIGNAL(toggled(bool)), this, SLOT(onFPD1CheckBoxToggled(bool)));
	connect(fpd2CheckBox_, SIGNAL(toggled(bool)), this, SLOT(onFPD2CheckBoxToggled(bool)));
	connect(fpd3CheckBox_, SIGNAL(toggled(bool)), this, SLOT(onFPD3CheckBoxToggled(bool)));
	connect(fpd4CheckBox_, SIGNAL(toggled(bool)), this, SLOT(onFPD4CheckBoxToggled(bool)));

	connect(minSpinBox_, SIGNAL(editingFinished()), this, SLOT(onMinSpinBoxEditingFinished()));
	connect(maxSpinBox_, SIGNAL(editingFinished()), this, SLOT(onMaxSpinBoxEditingFinished()));
	connect(autoScaleCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onAutoScaleCheckBoxToggled(bool)));
	connect(logCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onLogCheckBoxToggled(bool)));

	stripToolCounter_ = 0;
	stripToolTimer_ = new QTimer(this);
	connect(stripToolTimer_, SIGNAL(timeout()), this, SLOT(onStripToolTimerTimeout()));
	stripToolTimer_->start(1000);

	warningAndPlotHL_ = new QHBoxLayout();
	warningAndPlotHL_->addWidget(beamlineWarningsLabel_);

	gl_->addWidget(readyLabel_,		0, 0, 1, 6, 0);
	gl_->addWidget(beamOnButton_,		1, 0, 1, 2, 0);
	gl_->addWidget(beamOffCButton_,		1, 2, 1, 2, 0);
	gl_->addWidget(stopMotorsButton_,	1, 4, 1, 2, 0);
	gl_->addWidget(closeVacuumButton_,	2, 0, 1, 3, 0);
	gl_->addWidget(visibleLightButton_,	2, 3, 1, 3, 0);
	gl_->addWidget(energyNC_,		3, 0, 1, 6, 0);
	gl_->addWidget(trackUndulatorCButton_,	4, 0, 1, 2, 0);
	gl_->addWidget(trackGratingCButton_,	4, 2, 1, 2, 0);
	gl_->addWidget(trackExitSlitCButton_,	4, 4, 1, 2, 0);
	gl_->addWidget(gratingNC_,		5, 0, 1, 6, 0);
	gl_->addWidget(exitSlitNC_,		6, 0, 1, 3, 0);
	gl_->addLayout(shl,			6, 3, 1, 3, 0);
	gl_->addLayout(vhl,			7, 0, 1, 3, 0);
	gl_->addLayout(warningAndPlotHL_,	10, 0, 1, 6, 0);

	gl_->setRowStretch(9, 10);

	setLayout(mainLayout_);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

	scanningResetButton_->setContentsMargins(2,2,2,2);
	onBeamlineWarnings("");
}

SGMSidebar::~SGMSidebar() {
	// must delete series objects first.  If left to ~QObject / QObject::deleteChildren() called from ~MPlot, the ~MPlotAbstractSeries() crashes when trying to disconnect from its data, because we've deleted the data already.
	delete i0Series_;
	delete teySeries_;
	delete tfySeries_;
	delete pdSeries_;
}


void SGMSidebar::showEvent(QShowEvent *se){
	int minWidth = std::max(exitSlitNC_->size().width()/3, trackUndulatorCButton_->size().width()/2);
	minWidth = std::max(minWidth, trackGratingCButton_->size().width()/2);
	minWidth = std::max(minWidth, trackExitSlitCButton_->size().width()/2);
	gl_->setColumnMinimumWidth(0, minWidth);
	gl_->setColumnMinimumWidth(1, minWidth);
	gl_->setColumnMinimumWidth(2, minWidth);
	gl_->setColumnMinimumWidth(3, minWidth);
	gl_->setColumnMinimumWidth(4, minWidth);
	gl_->setColumnMinimumWidth(5, minWidth);
	QWidget::showEvent(se);
}

void SGMSidebar::onVisibleLightStatusChanged(const QString &status){
	if(visibleLightButton_)
		visibleLightButton_->setText(status);
}

void SGMSidebar::onVisibleLightClicked(){
	if(SGMBeamline::sgm()->isVisibleLightOn())
		SGMBeamline::sgm()->visibleLightOff();
	else{
		AMListAction3 *visibleLightOnListAction = new AMListAction3(new AMListActionInfo3("Move to visible light", "Move to visible light"));
		if(SGMBeamline::sgm()->gratingVelocity()->value() != 10000){

			AMAction3 *gratingVelocityMoveAction = AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->gratingVelocity(), 10000);
			visibleLightOnListAction->addSubAction(gratingVelocityMoveAction);
		}

		AMAction3 *visibleLightToggleAction = AMActionSupport::buildControlMoveAction(SGMBeamline::sgm()->visibleLightToggle(), 1);
		visibleLightOnListAction->addSubAction(visibleLightToggleAction);

		visibleLightOnListAction->start();
	}
}

void SGMSidebar::onCloseVacuumButtonClicked(){
	SGMBeamline::sgm()->closeVacuum();
}

void SGMSidebar::onBeamOnButtonClicked(){
	if(beamOnAction_)
		return;
	beamOnAction_ = SGMBeamline::sgm()->createBeamOnActions3();
	connect(beamOnAction_, SIGNAL(succeeded()), this, SLOT(onBeamOnActionFinished()));
	connect(beamOnAction_, SIGNAL(failed()), this, SLOT(onBeamOnActionFinished()));
	beamOnAction_->start();
}

void SGMSidebar::onBeamOnActionFinished(){
	disconnect(beamOnAction_, SIGNAL(succeeded()), this, SLOT(onBeamOnActionFinished()));
	disconnect(beamOnAction_, SIGNAL(failed()), this, SLOT(onBeamOnActionFinished()));
	beamOnAction_->deleteLater();
	beamOnAction_ = 0;
}

void SGMSidebar::onStopMotorsButtonClicked(){
	if(stopMotorsAction_)
		return;
	stopMotorsAction_ = SGMBeamline::sgm()->createStopMotorsActions3();
	connect(stopMotorsAction_, SIGNAL(succeeded()), this, SLOT(onStopMotorsActionFinished()));
	connect(stopMotorsAction_, SIGNAL(failed()), this, SLOT(onStopMotorsActionFinished()));
	stopMotorsAction_->start();
}

void SGMSidebar::onStopMotorsActionFinished(){
	disconnect(stopMotorsAction_, SIGNAL(succeeded()), this, SLOT(onStopMotorsActionFinished()));
	disconnect(stopMotorsAction_, SIGNAL(failed()), this, SLOT(onStopMotorsActionFinished()));
	stopMotorsAction_->deleteLater();
	stopMotorsAction_ = 0;
}

void SGMSidebar::onRestoreBeamlineSettingsButtonClicked(){
	if(restoreBeamlineAction_)
		return;
	restoreBeamlineAction_ = SGMBeamline::sgm()->createRestorePreFastScanDefaultActions();
	connect(restoreBeamlineAction_, SIGNAL(succeeded()), this, SLOT(onRestoreBeamlineActionFinished()));
	connect(restoreBeamlineAction_, SIGNAL(failed()), this, SLOT(onRestoreBeamlineActionFinished()));
	restoreBeamlineAction_->start();
}

void SGMSidebar::onRestoreBeamlineActionFinished(){
	disconnect(restoreBeamlineAction_, SIGNAL(succeeded()), this, SLOT(onRestoreBeamlineActionFinished()));
	disconnect(restoreBeamlineAction_, SIGNAL(failed()), this, SLOT(onRestoreBeamlineActionFinished()));
	restoreBeamlineAction_->deleteLater();
	restoreBeamlineAction_ = 0;
}

//#include "actions3/actions/AMRestAction.h"

void SGMSidebar::onScanningResetButtonClicked(){
//	SGMBeamline::sgm()->beamlineScanning()->move(0);

	qDebug() << "Clicked here";

//	AMListAction3 *masterList = new AMSequentialListAction3(new AMSequentialListActionInfo3("Rest Reqeusts", "Rest Requests"));

//	//QString userInfo = userName_+":"+password_;
//	QString userInfo = QString("davidChevrier:D1k2C#27G");
//	//QString userInfo = QString("davidChevrier:aabbccdd");
//	QByteArray userData = userInfo.toLocal8Bit().toBase64();
////	QString headerData = "Basic " + userData;
//	headerData_ = "Basic " + userData;
//	//request.setRawHeader("Authorization", headerData.toLocal8Bit());

//	manager_ = new QNetworkAccessManager(this);

//	AMRestActionInfo *restActionInfo = new AMRestActionInfo("https://api.github.com/user", AMRestActionInfo::GetRequest);
//	restActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
//	authenicationAction_ = new AMRestAction(restActionInfo, manager_);
//	masterList->addSubAction(authenicationAction_);
////	restAction->start();

//	currentClosedIssuesPage_ = 1;
//	lastPage_ = false;
//	QString issuesString = QString("https://api.github.com/repos/acquaman/acquaman/issues?filter=all&state=closed&page=%1&per_page=30").arg(currentClosedIssuesPage_);
//	AMRestActionInfo *getAllClosedIssuesActionInfo = new AMRestActionInfo(issuesString, AMRestActionInfo::GetRequest);
//	getAllClosedIssuesActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
//	getAllClosedIssuesAction_ = new AMRestAction(getAllClosedIssuesActionInfo, manager_);
//	masterList->addSubAction(getAllClosedIssuesAction_);

////	connect(masterList, SIGNAL(succeeded()), this, SLOT(onRestActionsSucceeded()));
//	connect(getAllClosedIssuesAction_, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetAllClosedActionsFullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)));
//	masterList->start();
}

void SGMSidebar::onRestActionsSucceeded(){

}

void SGMSidebar::onGetAllClosedActionsFullResponseReady(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs){
//	qDebug() << "REST response ready";

//	QVariant allClosedIssuesResponse = fullResponse;

//	int lastPageNumber = -1;
//	int nextPageNumber = -1;
//	for(int x = 0; x < headerPairs.count(); x++){
//		if(headerPairs.at(x).first == "Link"){
//			QString linkHeader = headerPairs.at(x).second;
//			qDebug() << "Full link header: " << linkHeader;
//			QStringList linkHeaderItems = linkHeader.split(',');
//			for(int y = 0; y < linkHeaderItems.count(); y++){
//				if(linkHeaderItems.at(y).contains("; rel=\"last\""))
//					lastPageNumber = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
//				if(linkHeaderItems.at(y).contains("; rel=\"next\""))
//					nextPageNumber = AMRestAction::pageNumberFromURLString(linkHeaderItems.at(y));
//			}

//			//lastPageNumber_ = lastPageNumber;
//			qDebug() << "Last page " << lastPageNumber;
//			qDebug() << "Next page " << nextPageNumber;
//		}
//	}

//	if(allClosedIssuesResponse.canConvert(QVariant::Map)){
//		QVariantMap jsonMap = allClosedIssuesResponse.toMap();
//		QMap<QString, QVariant>::const_iterator i = jsonMap.constBegin();
//		while (i != jsonMap.constEnd()) {
//			qDebug() << i.key() << i.value();
//			i++;
//		}
////		jsonSensiblePrint(githubFullReply.toMap());
//	}
//	else if(allClosedIssuesResponse.canConvert(QVariant::List)){
//		QVariantMap jsonMap;
//		QVariantList githubListReply = allClosedIssuesResponse.toList();
//		if(githubListReply.at(0).canConvert(QVariant::Map)){
//			for(int x = 0; x < githubListReply.count(); x++){
//				jsonMap = githubListReply.at(x).toMap();
//				QMap<QString, QVariant>::const_iterator i = jsonMap.constBegin();
//				int number;
//				QString complexity;
//				QString title;
//				QVariant pullRequest;
//				int comments;
//				QString commentsURL;
//				QString assignee;
//				bool projectTrackingDisabled = false;
//				bool inlineIssue = false;
//				while (i != jsonMap.constEnd()) {
////					qDebug() << i.key() << i.value();
////					qDebug() << i.key();
//					if(i.key() == "number"){
////						qDebug() << "Number is " << i.value();
//						number = i.value().toInt();
//					}
//					else if(i.key() == "labels"){
//						QVariantList labelsList = i.value().toList();
//						for(int x = 0, size = labelsList.count(); x < size; x++){
//							QVariantMap labelMap = labelsList.at(x).toMap();
//							if(labelMap.contains("name") && labelMap.value("name").toString().contains("Complexity"))
//								complexity = labelMap.value("name").toString();
//							if(labelMap.contains("name") && labelMap.value("name").toString().contains("Project Tracking Disabled"))
//								projectTrackingDisabled = true;
//							if(labelMap.contains("name") && labelMap.value("name").toString().contains("Project Tracking Inline Issue"))
//								inlineIssue = true;
//						}
//					}
//					else if(i.key() == "title")
//						title = i.value().toString();
//					else if(i.key() == "pull_request")
//						pullRequest = i.value();
//					else if(i.key() == "comments")
//						comments = i.value().toInt();
//					else if(i.key() == "comments_url" )
//						commentsURL = i.value().toString();
//					else if(i.key() == "assignee")
//						assignee = i.value().toMap().value("login").toString();
//					else if(i.key() == "milestone" && !i.value().toMap().value("title").toString().isEmpty()){
//						qDebug() << "Found a milestone for " << number;
//						qDebug() << i.value().toMap().value("title");
////						qDebug() << i.value();
//					}
//					i++;
//				}
//				QString debugString;
//				QString originatingIssue;
//				if(pullRequest.isNull())
//					debugString = QString("[%1] %2: %3").arg(number).arg(title).arg(complexity);
//				else{
//					int startPoint = title.indexOf("Issue") + 5;
//					int endPoint = title.indexOf(":");
//					originatingIssue = title.mid(startPoint, endPoint-startPoint);
//					debugString = QString("[%1 --> %2] %3: %4").arg(number).arg(originatingIssue).arg(title).arg(complexity);
////					qDebug() << pullRequest;
//				}
//				if(!commentsURL.isEmpty())
//					debugString.append(QString(" [@%1]").arg(commentsURL));
////				qDebug() << debugString;

//				bool isPullRequest = false;
//				if(!pullRequest.isNull())
//					isPullRequest = true;
//				int originatingIssueNumber = -1;
//				if(!originatingIssue.isEmpty())
//					originatingIssueNumber = originatingIssue.toInt();
////				AMGitHubIssue *oneIssue = new AMGitHubIssue(number, AMGitHubIssue::complexityFromString(complexity), title, isPullRequest, 0, originatingIssueNumber, comments, commentsURL, QString(), assignee, projectTrackingDisabled, inlineIssue, this);

//				allIssues_.insert(oneIssue->issueNumber(), oneIssue);

////				qDebug() << comments;
////				qDebug() << "\n\n";
//			}
//		}
//	}

//	if(!lastPage_){
//		currentClosedIssuesPage_ = nextPageNumber;
////		if(nextPageNumber == lastPageNumber)
////			lastPage_ = true;

//		if(nextPageNumber == 10)
//			lastPage_ = true;

//		qDebug() << "More closed issues to fetch, going to page " << currentClosedIssuesPage_;

//		QString issuesString = QString("https://api.github.com/repos/acquaman/acquaman/issues?filter=all&state=closed&page=%1&per_page=30").arg(currentClosedIssuesPage_);
//		AMRestActionInfo *getOneClosedIssuesPageActionInfo = new AMRestActionInfo(issuesString, AMRestActionInfo::GetRequest);
//		getOneClosedIssuesPageActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
//		AMRestAction *getOneClosedIssuesPageAction = new AMRestAction(getOneClosedIssuesPageActionInfo, manager_);

//	//	connect(masterList, SIGNAL(succeeded()), this, SLOT(onRestActionsSucceeded()));
//		connect(getOneClosedIssuesPageAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetAllClosedActionsFullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)));
//		getOneClosedIssuesPageAction->start();
//	}
//	else{

//		QMap<int, AMGitHubIssue*>::const_iterator j = allIssues_.constBegin();
//		while (j != allIssues_.constEnd()) {
//			if(j.value()->isPullRequest() && allIssues_.contains(j.value()->originatingIssueNumber()))
//				j.value()->setOriginatingIssue(allIssues_.value(j.value()->originatingIssueNumber()));

//			if( (!j.value()->isPullRequest()) && (j.value()->commentCount() > 0) && (!j.value()->commentsURL().isEmpty()) && (j.value()->complexityValue() != AMGitHubIssue::InvalidComplexity) ){
//				commentURLs_.append(j.value()->commentsURL());
//			}
//			j++;
//		}

//		//	QMap<int, AMGitHubIssue*>::const_iterator k = allIssues_.constBegin();
//		//	while (k != allIssues_.constEnd()) {
//		//		qDebug() << k.value()->oneLineDebugInfo();
//		//		k++;
//		//	}

//		if(commentURLs_.count() > 0){
//			QString oneCommentURL = commentURLs_.takeFirst();

//			AMRestActionInfo *getOneIssueCommentsActionInfo = new AMRestActionInfo(oneCommentURL, AMRestActionInfo::GetRequest);
//			getOneIssueCommentsActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
//			AMRestAction *getOneIssueCommentsAction = new AMRestAction(getOneIssueCommentsActionInfo, manager_);

//			connect(getOneIssueCommentsAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetOneIssueCommentsReturned(QVariant, QList<QNetworkReply::RawHeaderPair>)));
//			getOneIssueCommentsAction->start();
//		}
//	}
}

#include <QScrollArea>

void SGMSidebar::onGetOneIssueCommentsReturned(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs)
{
////	qDebug() << "\n\n\n ONE ISSUE COMMENTS";


//	QVariantList githubListReply = fullResponse.toList();
//	int issueNumber = githubListReply.at(0).toMap().value("issue_url").toString().section("/", -1, -1).toInt();
//	qDebug() << "Checking Issue " << issueNumber << "(" << commentURLs_.count() << " comments remaining to fetch)";
////	qDebug() << issueNumber;
//	for(int x = 0, size = githubListReply.count(); x < size; x++){
////		qDebug() << githubListReply.at(x).toMap().value("body").toString();
//		if(githubListReply.at(x).toMap().value("body").toString().contains("Time Estimate:")){
//			QStringList splitCommentBody = githubListReply.at(x).toMap().value("body").toString().split("\n");
//			for(int y = 0, ySize = splitCommentBody.count(); y < ySize; y++){
//				if(splitCommentBody.at(y).contains("Time Estimate:")){
//					QString timeEstimateString = splitCommentBody.at(y);
//					timeEstimateString.remove("Time Estimate:");
//					timeEstimateString = timeEstimateString.simplified();
//					if(timeEstimateString.endsWith("."))
//						timeEstimateString.remove(timeEstimateString.count()-1, 1);

//					//qDebug() << "Parsed time estimate for " << issueNumber << timeEstimateString.remove("Time Estimate:");
//					qDebug() << "Parsed time estimate for " << issueNumber << timeEstimateString;

//					if(allIssues_.contains(issueNumber))
//						allIssues_.value(issueNumber)->setTimeEstimateString(timeEstimateString);
////						allIssues_.value(issueNumber)->setTimeEstimateString(timeEstimateString.remove("Time Estimate:"));
//				}
//			}
//		}
//	}

//	if(commentURLs_.count() > 0){
//		QString oneCommentURL = commentURLs_.takeFirst();

//		AMRestActionInfo *getOneIssueCommentsActionInfo = new AMRestActionInfo(oneCommentURL, AMRestActionInfo::GetRequest);
//		getOneIssueCommentsActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
//		AMRestAction *getOneIssueCommentsAction = new AMRestAction(getOneIssueCommentsActionInfo, manager_);

//		connect(getOneIssueCommentsAction, SIGNAL(fullResponseReady(QVariant, QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetOneIssueCommentsReturned(QVariant, QList<QNetworkReply::RawHeaderPair>)));
//		getOneIssueCommentsAction->start();
//	}
//	else{

//		QMap<int, AMGitHubIssue*>::const_iterator i = allIssues_.constBegin();
//		while (i != allIssues_.constEnd()) {
//			if(!i.value()->isPullRequest() && i.value()->inlineIssue()){
//				AMGitHubIssueFamily *oneIssueFamily = new AMGitHubIssueFamily(i.value(), i.value());
//				allIssueFamilies_.insert(i.value()->issueNumber(), oneIssueFamily);
//			}
//			else if(!i.value()->isPullRequest() && !i.value()->projectTrackingDisabled()){
//				AMGitHubIssueFamily *oneIssueFamily = new AMGitHubIssueFamily(i.value(), 0);
//				allIssueFamilies_.insert(i.value()->issueNumber(), oneIssueFamily);
//			}
//			else if(!i.value()->isPullRequest() && (i.value()->complexityValue() != AMGitHubIssue::InvalidComplexity) && (i.value()->timeEstimateString() != "Invalid Time Estimate") && (!i.value()->timeEstimateString().isEmpty()))
//				qDebug() << "Issue " << i.value()->issueNumber() << " may be an inline issue";
//			i++;
//		}

//		QMap<int, AMGitHubIssue*>::const_iterator j = allIssues_.constBegin();
//		while (j != allIssues_.constEnd()) {
//			if(j.value()->isPullRequest() && allIssueFamilies_.contains(j.value()->originatingIssueNumber()))
//				allIssueFamilies_.value(j.value()->originatingIssueNumber())->setPullRequestIssue(j.value());

//			j++;
//		}

//		QList<int> totallyUnspecifiedIssues;
//		QMap<int, AMGitHubIssueFamily*>::const_iterator k = allIssueFamilies_.constBegin();
//		while(k != allIssueFamilies_.constEnd()){
//			if(k.value()->totallyIncomplete())
//				totallyUnspecifiedIssues.append(k.value()->originatingIssueNumber());
//			k++;
//		}
//		for(int x = 0, size = totallyUnspecifiedIssues.size(); x < size; x++)
//			allIssueFamilies_.remove(totallyUnspecifiedIssues.at(x));


//		QWidget *familiesMasterWidget = new QWidget();
//		QVBoxLayout *familiesMasterVL = new QVBoxLayout();
//		QScrollArea *familiesScrollArea = new QScrollArea();
//		QWidget *familiesWidget = new QWidget();
//		QVBoxLayout *familiesVL = new QVBoxLayout();
//		qDebug() << "\n\n\n\n";
//		QMap<int, AMGitHubIssueFamily*>::const_iterator m = allIssueFamilies_.constBegin();
//		while(m != allIssueFamilies_.constEnd()){
//			bool printIt = true;
//			if(m.value()->completeInfo())
//				printIt = false;
//			if(m.value()->totallyIncomplete())
//				printIt = false;
//			if(m.value()->onlyMissingActualComplexity())
//				printIt = false;
//			if(m.value()->onlyMissingTimeEstimate())
//				printIt = false;
////			if(!m.value()->completeInfo() || !m.value()->totallyIncomplete())
//			if(printIt)
//				qDebug() << m.value()->multiLineDebugInfo();
//			AMGitHubIssueFamilyView *oneFamilyView = new AMGitHubIssueFamilyView(m.value());
//			familiesVL->addWidget(oneFamilyView);
//			m++;
//		}

//		familiesWidget->setLayout(familiesVL);
//		familiesScrollArea->setWidget(familiesWidget);

//		familiesMasterVL->addWidget(familiesScrollArea);
//		familiesMasterWidget->setLayout(familiesMasterVL);
//		familiesMasterWidget->show();

//		qDebug() << "\n\n\n\n";
//		QMap<int, AMGitHubIssueFamily*>::const_iterator o = allIssueFamilies_.constBegin();
//		while(o != allIssueFamilies_.constEnd()){
//			if(o.value()->onlyMissingActualComplexity())
//				qDebug() << "Need to do actual complexity for:\n" << o.value()->multiLineDebugInfo() << "\n";
//			o++;
//		}

//		qDebug() << "\n\n\n\n";
//		QMap<int, AMGitHubIssueFamily*>::const_iterator p = allIssueFamilies_.constBegin();
//		while(p != allIssueFamilies_.constEnd()){
//			if(p.value()->onlyMissingTimeEstimate())
//				qDebug() << p.value()->originatingIssue()->assignee() << " needs to do time estimate for: " << p.value()->originatingIssueNumber() << "\n";
//			p++;
//		}

//		int totallyCompleteIssues = 0;
//		int totallyIncompleteIssues = 0;
//		int onlyMissingActualComplexityIssues = 0;
//		int onlyMissingTimeEstimateIssues = 0;

//		qDebug() << "\n\n\n\n";
//		QMap<int, AMGitHubIssueFamily*>::const_iterator q = allIssueFamilies_.constBegin();
//		while(q != allIssueFamilies_.constEnd()){
//			if(q.value()->completeInfo())
//				totallyCompleteIssues++;
//			if(q.value()->totallyIncomplete())
//				totallyIncompleteIssues++;
//			if(q.value()->onlyMissingActualComplexity())
//				onlyMissingActualComplexityIssues++;
//			if(q.value()->onlyMissingTimeEstimate())
//				onlyMissingTimeEstimateIssues++;
//			q++;
//		}
//		qDebug() << totallyCompleteIssues << " of " << allIssueFamilies_.count() << " are fully specfied";
//		qDebug() << totallyIncompleteIssues << " of " << allIssueFamilies_.count() << " are totally unspecified";
//		qDebug() << onlyMissingActualComplexityIssues << " of " << allIssueFamilies_.count() << " are only missing an actual complexity";
//		qDebug() << onlyMissingTimeEstimateIssues << " of " << allIssueFamilies_.count() << " are only missing a time estimate";

//		qDebug() << "\n\n\n\n";
//		QVector<int> complexityMappingMatrix = QVector<int>(31, 0);
//		QList<double> estimatedComplexity1Times;
//		QList<double> estimatedComplexity2Times;
//		QList<double> estimatedComplexity3Times;
//		QList<double> estimatedComplexity5Times;
//		QList<double> estimatedComplexity8Times;
//		QList<double> estimatedComplexityKTimes;
//		QList<double> actualComplexity1Times;
//		QList<double> actualComplexity2Times;
//		QList<double> actualComplexity3Times;
//		QList<double> actualComplexity5Times;
//		QList<double> actualComplexity8Times;
//		QMap<int, AMGitHubIssueFamily*>::const_iterator r = allIssueFamilies_.constBegin();
//		while(r != allIssueFamilies_.constEnd()){
//			complexityMappingMatrix[r.value()->complexityMapping()] = complexityMappingMatrix.at(r.value()->complexityMapping())+1;

//			if(r.value()->normalizedTimeEstiamte() > 0){
//				switch(r.value()->estimatedComplexity()){
//				case AMGitHubIssue::Complexity1:
//					estimatedComplexity1Times.append(r.value()->normalizedTimeEstiamte());
//					break;
//				case AMGitHubIssue::Complexity2:
//					estimatedComplexity2Times.append(r.value()->normalizedTimeEstiamte());
//					break;
//				case AMGitHubIssue::Complexity3:
//					estimatedComplexity3Times.append(r.value()->normalizedTimeEstiamte());
//					break;
//				case AMGitHubIssue::Complexity5:
//					estimatedComplexity5Times.append(r.value()->normalizedTimeEstiamte());
//					break;
//				case AMGitHubIssue::Complexity8:
//					estimatedComplexity8Times.append(r.value()->normalizedTimeEstiamte());
//					break;
//				case AMGitHubIssue::ComplexityK:
//					estimatedComplexityKTimes.append(r.value()->normalizedTimeEstiamte());
//					break;
//				}

//				switch(r.value()->actualComplexity()){
//				case AMGitHubIssue::Complexity1:
//					actualComplexity1Times.append(r.value()->normalizedTimeEstiamte());
//					break;
//				case AMGitHubIssue::Complexity2:
//					actualComplexity2Times.append(r.value()->normalizedTimeEstiamte());
//					break;
//				case AMGitHubIssue::Complexity3:
//					actualComplexity3Times.append(r.value()->normalizedTimeEstiamte());
//					break;
//				case AMGitHubIssue::Complexity5:
//					actualComplexity5Times.append(r.value()->normalizedTimeEstiamte());
//					break;
//				case AMGitHubIssue::Complexity8:
//					actualComplexity8Times.append(r.value()->normalizedTimeEstiamte());
//					break;
//				}
//			}
//			else
//				qDebug() << "Issue " << r.value()->originatingIssueNumber() << " cannot normalize " << r.value()->timeEstimate();

//			r++;
//		}

//		QString debugString;
//		for(int x = 1, xSize = complexityMappingMatrix.count(); x < xSize; x++){
//			double percentValue = ((double)complexityMappingMatrix.at(x))/((double)allIssueFamilies_.count())*100;
//			debugString.append(QString("%1 \t").arg(percentValue, 0, 'f', 2));
//			if(x%5 == 0){
//				qDebug() << debugString;
//				debugString.clear();
//			}
//		}

//		double averageEstimatedComplexity1Time = 0;
//		double averageEstimatedComplexity2Time = 0;
//		double averageEstimatedComplexity3Time = 0;
//		double averageEstimatedComplexity5Time = 0;
//		double averageEstimatedComplexity8Time = 0;
//		double averageEstimatedComplexityKTime = 0;
//		double averageActualComplexity1Time = 0;
//		double averageActualComplexity2Time = 0;
//		double averageActualComplexity3Time = 0;
//		double averageActualComplexity5Time = 0;
//		double averageActualComplexity8Time = 0;

//		for(int x = 0, size = estimatedComplexity1Times.count(); x < size; x++)
//			averageEstimatedComplexity1Time += estimatedComplexity1Times.at(x);
//		averageEstimatedComplexity1Time = averageEstimatedComplexity1Time/((double)estimatedComplexity1Times.count());

//		for(int x = 0, size = estimatedComplexity2Times.count(); x < size; x++)
//			averageEstimatedComplexity2Time += estimatedComplexity2Times.at(x);
//		averageEstimatedComplexity2Time = averageEstimatedComplexity2Time/((double)estimatedComplexity2Times.count());

//		for(int x = 0, size = estimatedComplexity3Times.count(); x < size; x++)
//			averageEstimatedComplexity3Time += estimatedComplexity3Times.at(x);
//		averageEstimatedComplexity3Time = averageEstimatedComplexity3Time/((double)estimatedComplexity3Times.count());

//		for(int x = 0, size = estimatedComplexity5Times.count(); x < size; x++)
//			averageEstimatedComplexity5Time += estimatedComplexity5Times.at(x);
//		averageEstimatedComplexity5Time = averageEstimatedComplexity5Time/((double)estimatedComplexity5Times.count());

//		for(int x = 0, size = estimatedComplexity8Times.count(); x < size; x++)
//			averageEstimatedComplexity8Time += estimatedComplexity8Times.at(x);
//		averageEstimatedComplexity8Time = averageEstimatedComplexity8Time/((double)estimatedComplexity8Times.count());

//		for(int x = 0, size = estimatedComplexityKTimes.count(); x < size; x++)
//			averageEstimatedComplexityKTime += estimatedComplexityKTimes.at(x);
//		averageEstimatedComplexityKTime = averageEstimatedComplexityKTime/((double)estimatedComplexityKTimes.count());


//		for(int x = 0, size = actualComplexity1Times.count(); x < size; x++)
//			averageActualComplexity1Time += actualComplexity1Times.at(x);
//		averageActualComplexity1Time = averageActualComplexity1Time/((double)actualComplexity1Times.count());

//		for(int x = 0, size = actualComplexity2Times.count(); x < size; x++)
//			averageActualComplexity2Time += actualComplexity2Times.at(x);
//		averageActualComplexity2Time = averageActualComplexity2Time/((double)actualComplexity2Times.count());

//		for(int x = 0, size = actualComplexity3Times.count(); x < size; x++)
//			averageActualComplexity3Time += actualComplexity3Times.at(x);
//		averageActualComplexity3Time = averageActualComplexity3Time/((double)actualComplexity3Times.count());

//		for(int x = 0, size = actualComplexity5Times.count(); x < size; x++)
//			averageActualComplexity5Time += actualComplexity5Times.at(x);
//		averageActualComplexity5Time = averageActualComplexity5Time/((double)actualComplexity5Times.count());

//		for(int x = 0, size = actualComplexity8Times.count(); x < size; x++)
//			averageActualComplexity8Time += actualComplexity8Times.at(x);
//		averageActualComplexity8Time = averageActualComplexity8Time/((double)actualComplexity8Times.count());

//		qDebug() << "Average Estimated Complexity 1 time estimate: " << averageEstimatedComplexity1Time;
//		qDebug() << "Average Estimated Complexity 2 time estimate: " << averageEstimatedComplexity2Time;
//		qDebug() << "Average Estimated Complexity 3 time estimate: " << averageEstimatedComplexity3Time;
//		qDebug() << "Average Estimated Complexity 5 time estimate: " << averageEstimatedComplexity5Time;
//		qDebug() << "Average Estimated Complexity 8 time estimate: " << averageEstimatedComplexity8Time;
//		qDebug() << "Average Estimated Complexity K time estimate: " << averageEstimatedComplexityKTime;

//		qDebug() << "Average Actual Complexity 1 time estimate: " << averageActualComplexity1Time;
//		qDebug() << "Average Actual Complexity 2 time estimate: " << averageActualComplexity2Time;
//		qDebug() << "Average Actual Complexity 3 time estimate: " << averageActualComplexity3Time;
//		qDebug() << "Average Actual Complexity 5 time estimate: " << averageActualComplexity5Time;
//		qDebug() << "Average Actual Complexity 8 time estimate: " << averageActualComplexity8Time;

////		QString milestonesString = QString("https://api.github.com/repos/acquaman/acquaman/milestones");
////		AMRestActionInfo *getAllOpenMilestonesActionInfo = new AMRestActionInfo(milestonesString, AMRestActionInfo::GetRequest);
////		getAllOpenMilestonesActionInfo->setRawHeader("Authorization", headerData_.toLocal8Bit());
////		AMRestAction *getAllOpenMilestonesAction = new AMRestAction(getAllOpenMilestonesActionInfo, manager_);
////		connect(getAllOpenMilestonesAction, SIGNAL(fullResponseReady(QVariant,QList<QNetworkReply::RawHeaderPair>)), this, SLOT(onGetAllOpenMilestonesReturned(QVariant,QList<QNetworkReply::RawHeaderPair>)));
////		getAllOpenMilestonesAction->start();
//	}
}

void SGMSidebar::onGetAllOpenMilestonesReturned(QVariant fullResponse, QList<QNetworkReply::RawHeaderPair> headerPairs){
//	qDebug() << "Milestones: \n\n\n";
//	QVariantList fullResponseList = fullResponse.toList();
//	for(int x = 0, size = fullResponseList.count(); x < size; x++){
//		QVariantMap oneResponseMap = fullResponseList.at(x).toMap();

//		QMap<QString, QVariant>::const_iterator r = oneResponseMap.constBegin();
//		while(r != oneResponseMap.constEnd()){
//			qDebug() << r.key() << r.value() << "\n";
//			r++;
//		}
//	}
}

void SGMSidebar::onStripToolTimerTimeout(){
	if(i0Model_->count() <= 50) {
		stripToolSpecialAxisScale_->setDataRange(MPlotAxisRange(-i0Model_->count(),0));	// would need to correct this if not doing exactly one model point per second
	}
	else {
		i0Model_->removePointFront();
		teyModel_->removePointFront();
		tfyModel_->removePointFront();
		pdModel_->removePointFront();
		fpd1Model_->removePointFront();
		fpd2Model_->removePointFront();
		fpd3Model_->removePointFront();
		fpd4Model_->removePointFront();
	}

	// inserted to prevent crashes before SGM detectors connected
	double i0Reading = 0, teyReading = 0, tfyReading = 0, pdReading = 0, fpd1Reading = 0, fpd2Reading = 0, fpd3Reading = 0, fpd4Reading = 0;
	if(SGMBeamline::sgm()->newI0Detector())
		i0Reading = SGMBeamline::sgm()->newI0Detector()->singleReading();
	if(SGMBeamline::sgm()->newTEYDetector())
		teyReading = SGMBeamline::sgm()->newTEYDetector()->singleReading();
	if(SGMBeamline::sgm()->newTFYDetector())
		tfyReading = SGMBeamline::sgm()->newTFYDetector()->singleReading();
	if(SGMBeamline::sgm()->newPDDetector())
		pdReading = SGMBeamline::sgm()->newPDDetector()->singleReading();
	if(SGMBeamline::sgm()->newFilteredPD1Detector())
		fpd1Reading = SGMBeamline::sgm()->newFilteredPD1Detector()->singleReading();
	if(SGMBeamline::sgm()->newFilteredPD2Detector())
		fpd2Reading = SGMBeamline::sgm()->newFilteredPD2Detector()->singleReading();
	if(SGMBeamline::sgm()->newFilteredPD3Detector())
		fpd3Reading = SGMBeamline::sgm()->newFilteredPD3Detector()->singleReading();
	if(SGMBeamline::sgm()->newFilteredPD4Detector())
		fpd4Reading = SGMBeamline::sgm()->newFilteredPD4Detector()->singleReading();

	///////////////

	if(logCheckBox_->isChecked()){
		i0Reading++;
		teyReading++;
		tfyReading++;
		pdReading++;
		fpd1Reading++;
		fpd2Reading++;
		fpd3Reading++;
		fpd4Reading++;
	}

	i0Series_->setDescription(QString("I0 %1").arg(i0Reading, 0, 'e', 2));
	i0Model_->insertPointBack(stripToolCounter_, i0Reading);
	teySeries_->setDescription(QString("TEY %1").arg(teyReading, 0, 'e', 2));
	teyModel_->insertPointBack(stripToolCounter_, teyReading);
	tfySeries_->setDescription(QString("TFY %1").arg(tfyReading, 0, 'e', 2));
	tfyModel_->insertPointBack(stripToolCounter_, tfyReading);
	pdSeries_->setDescription(QString("PD %1").arg(pdReading, 0, 'e', 2));
	pdModel_->insertPointBack(stripToolCounter_, pdReading);
	fpd1Series_->setDescription(QString("FPD1 %1").arg(fpd1Reading, 0, 'e', 2));
	fpd1Model_->insertPointBack(stripToolCounter_, fpd1Reading);
	fpd2Series_->setDescription(QString("FPD2 %1").arg(fpd2Reading, 0, 'e', 2));
	fpd2Model_->insertPointBack(stripToolCounter_, fpd2Reading);
	fpd3Series_->setDescription(QString("FPD3 %1").arg(fpd3Reading, 0, 'e', 2));
	fpd3Model_->insertPointBack(stripToolCounter_, fpd3Reading);
	fpd4Series_->setDescription(QString("FPD4 %1").arg(fpd4Reading, 0, 'e', 2));
	fpd4Model_->insertPointBack(stripToolCounter_, fpd4Reading);
	stripToolCounter_++;
}

void SGMSidebar::onI0CheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(i0Series_);
	else
		imagePlot_->removeItem(i0Series_);
}

void SGMSidebar::onTEYCheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(teySeries_);
	else
		imagePlot_->removeItem(teySeries_);
}

void SGMSidebar::onTFYCheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(tfySeries_);
	else
		imagePlot_->removeItem(tfySeries_);
}

void SGMSidebar::onPDCheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(pdSeries_);
	else
		imagePlot_->removeItem(pdSeries_);
}

void SGMSidebar::onFPD1CheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(fpd1Series_);
	else
		imagePlot_->removeItem(fpd1Series_);
}

void SGMSidebar::onFPD2CheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(fpd2Series_);
	else
		imagePlot_->removeItem(fpd2Series_);
}

void SGMSidebar::onFPD3CheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(fpd3Series_);
	else
		imagePlot_->removeItem(fpd3Series_);
}

void SGMSidebar::onFPD4CheckBoxToggled(bool toggled){
	if(toggled)
		imagePlot_->addItem(fpd4Series_);
	else
		imagePlot_->removeItem(fpd4Series_);
}

void SGMSidebar::onMinSpinBoxEditingFinished(){
	plotLeftAxisMinimum_ = minSpinBox_->value();
	if(plotLeftAxisMaximum_ < plotLeftAxisMinimum_){
		plotLeftAxisMaximum_ = plotLeftAxisMinimum_+1;
		maxSpinBox_->setValue(plotLeftAxisMaximum_);
	}
	MPlotAxisRange leftRange;
	leftRange.setRange(plotLeftAxisMinimum_, plotLeftAxisMaximum_);

	imagePlot_->axisScaleLeft()->setDataRange(leftRange);
}

void SGMSidebar::onMaxSpinBoxEditingFinished(){
	plotLeftAxisMaximum_ = maxSpinBox_->value();
	if(plotLeftAxisMinimum_ > plotLeftAxisMaximum_){
		plotLeftAxisMinimum_ = plotLeftAxisMaximum_-1;
		minSpinBox_->setValue(plotLeftAxisMinimum_);
	}
	MPlotAxisRange leftRange;
	leftRange.setRange(plotLeftAxisMinimum_, plotLeftAxisMaximum_);

	imagePlot_->axisScaleLeft()->setDataRange(leftRange);
}

void SGMSidebar::onAutoScaleCheckBoxToggled(bool toggled){
	imagePlot_->axisScaleLeft()->setAutoScaleEnabled(toggled);
	minSpinBox_->setEnabled(!toggled);
	maxSpinBox_->setEnabled(!toggled);
	if(!toggled)
		onMinSpinBoxEditingFinished();
}

void SGMSidebar::onLogCheckBoxToggled(bool toggled){
	if(toggled && plotLeftAxisMinimum_ < 1){
		minSpinBox_->setValue(1);
		onMinSpinBoxEditingFinished();
	}
	else if(!toggled && plotLeftAxisMinimum_ == 1){
		minSpinBox_->setValue(0);
		onMinSpinBoxEditingFinished();
	}

	int alterBy = 0;
	if(toggled)
		alterBy = 1;
	else
		alterBy = -1;

	for(int x = 0; x < i0Model_->count(); x++){
		QModelIndex nextI0Index = i0Model_->index(x, 1);
		i0Model_->setData(nextI0Index, i0Model_->y(x)+alterBy, Qt::EditRole);
		QModelIndex nextTEYIndex = teyModel_->index(x, 1);
		teyModel_->setData(nextTEYIndex, teyModel_->y(x)+alterBy, Qt::EditRole);
		QModelIndex nextTFYIndex = tfyModel_->index(x, 1);
		tfyModel_->setData(nextTFYIndex, tfyModel_->y(x)+alterBy, Qt::EditRole);
		QModelIndex nextPDIndex = pdModel_->index(x, 1);
		pdModel_->setData(nextPDIndex, pdModel_->y(x)+alterBy, Qt::EditRole);
		QModelIndex nextFPD1Index = fpd1Model_->index(x, 1);
		fpd1Model_->setData(nextFPD1Index, fpd1Model_->y(x)+alterBy, Qt::EditRole);
		QModelIndex nextFPD2Index = fpd2Model_->index(x, 1);
		fpd2Model_->setData(nextFPD2Index, fpd2Model_->y(x)+alterBy, Qt::EditRole);
		QModelIndex nextFPD3Index = fpd3Model_->index(x, 1);
		fpd3Model_->setData(nextFPD3Index, fpd3Model_->y(x)+alterBy, Qt::EditRole);
		QModelIndex nextFPD4Index = fpd4Model_->index(x, 1);
		fpd4Model_->setData(nextFPD4Index, fpd4Model_->y(x)+alterBy, Qt::EditRole);
	}

	imagePlot_->axisScaleLeft()->setLogScaleEnabled(toggled);
}

void SGMSidebar::onBeamlineWarnings(const QString &newWarnings){
	if(newWarnings.isEmpty() && warningAndPlotHL_->itemAt(0)->widget() == beamlineWarningsLabel_){
		warningAndPlotHL_->removeWidget(beamlineWarningsLabel_);
		beamlineWarningsLabel_->hide();
		warningAndPlotHL_->addLayout(plotLayout_);
		imageView_->show();
		i0CheckBox_->show();
		teyCheckBox_->show();
		tfyCheckBox_->show();
		pdCheckBox_->show();
		fpd1CheckBox_->show();
		fpd2CheckBox_->show();
		fpd3CheckBox_->show();
		fpd4CheckBox_->show();
		minSpinBox_->show();
		maxSpinBox_->show();
		minLabel_->show();
		maxLabel_->show();
	}
	else if(!newWarnings.isEmpty() && warningAndPlotHL_->itemAt(0)->layout() == plotLayout_){
		warningAndPlotHL_->removeItem(plotLayout_);
		imageView_->hide();
		i0CheckBox_->hide();
		teyCheckBox_->hide();
		tfyCheckBox_->hide();
		pdCheckBox_->hide();
		fpd1CheckBox_->hide();
		fpd2CheckBox_->hide();
		fpd3CheckBox_->hide();
		fpd4CheckBox_->hide();
		minSpinBox_->hide();
		maxSpinBox_->hide();
		minLabel_->hide();
		maxLabel_->hide();
		warningAndPlotHL_->addWidget(beamlineWarningsLabel_);
		beamlineWarningsLabel_->show();
	}
}

void SGMSidebar::onBeamlineCriticalControlSetConnectedChanged(bool isConnected){
	if(isConnected)
		controlsConnectedLabel_->setPixmap(QIcon(":/ON.png").pixmap(20));
	else
		controlsConnectedLabel_->setPixmap(QIcon(":/RED.png").pixmap(20));
}

void SGMSidebar::onBeamlineCriticalDetectorSetConnectedChanged(bool isConnected){
	if(isConnected)
		detectorsConnectedLabel_->setPixmap(QIcon(":/ON.png").pixmap(20));
	else
		detectorsConnectedLabel_->setPixmap(QIcon(":/RED.png").pixmap(20));
}
