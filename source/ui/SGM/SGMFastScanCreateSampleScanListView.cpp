#include "SGMFastScanCreateSampleScanListView.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QBoxLayout>
#include <QCheckBox>
#include <QApplication>

#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "dataman/AMSamplePlate.h"
#include "actions3/actions/AMSampleMoveAction.h"
#include "actions3/actions/AMScanAction.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
#include "actions3/AMActionRunner3.h"

SGMFastScanCreateSampleScanListView::SGMFastScanCreateSampleScanListView(SGMFastScanConfiguration2013 *configuration, QWidget *parent) :
	QWidget(parent)
{
	configuration_ = configuration;

	sampleComboBox_ = new QComboBox();

	if(!SGMBeamline::sgm()->samplePlate()){
		sampleComboBox_->addItem(QString("No Sample Plate"), QVariant(-1));
		sampleComboBox_->setEnabled(false);
	}
	else if(SGMBeamline::sgm()->samplePlate()->sampleCount() == 0){
		sampleComboBox_->addItem(QString("No Samples on Plate"), QVariant(-1));
		sampleComboBox_->setEnabled(false);
	}
	else{
		sampleComboBox_->addItem(QString("No Sample Selected"), QVariant(0));
		AMSamplePlate *currentPlate = SGMBeamline::sgm()->samplePlate();
		for(int x = 0, size = currentPlate->sampleCount(); x < size; x++)
			sampleComboBox_->addItem(QString(currentPlate->sampleAt(x)->name()), QVariant(x));
	}

	oneTimeInitializationCheckBox_ = new QCheckBox();
	oneTimeInitializationCheckBox_->setChecked(true);

	rowsSpinBox_ = new QSpinBox();
	rowsSpinBox_->setRange(0, 100);
	rowsSpinBox_->setValue(15);

	verticalStepSpinBox_ = new QDoubleSpinBox();
	verticalStepSpinBox_->setRange(0, 1.0);
	verticalStepSpinBox_->setValue(0.1);
	verticalStepSpinBox_->setSingleStep(0.01);

	columnsSpinBox_ = new QSpinBox();
	columnsSpinBox_->setRange(0, 100);
	columnsSpinBox_->setValue(1);

	horizontalStepSpinBox_ = new QDoubleSpinBox();
	horizontalStepSpinBox_->setRange(0, 2.5);
	horizontalStepSpinBox_->setValue(1.0);
	horizontalStepSpinBox_->setSingleStep(0.1);

	rotationDoubleSpinBox_ = new QDoubleSpinBox();
	rotationDoubleSpinBox_->setRange(-45, 45);
	rotationDoubleSpinBox_->setValue(SGMBeamline::sgm()->ssaManipulatorRot()->value());
	rotationDoubleSpinBox_->setSingleStep(0.1);

	addToWorkflowButton_ = new QPushButton("Add To Worklow");
	cancelButton_ = new QPushButton("Cancel");

	QFormLayout *leftFL = new QFormLayout();
	leftFL->addRow("Sample", sampleComboBox_);
	leftFL->addRow("Rotation", rotationDoubleSpinBox_);
	leftFL->addRow("Initialize Once", oneTimeInitializationCheckBox_);

	QFormLayout *centerFL = new QFormLayout();
	centerFL->addRow("Rows", rowsSpinBox_);
	centerFL->addRow("Vertical Step", verticalStepSpinBox_);

	QFormLayout *rightFL = new QFormLayout();
	rightFL->addRow("Columns", columnsSpinBox_);
	rightFL->addRow("Horizontal Step", horizontalStepSpinBox_);

	QHBoxLayout *buttonsHL = new QHBoxLayout();
	buttonsHL->addStretch();
	buttonsHL->addWidget(addToWorkflowButton_);
	buttonsHL->addWidget(cancelButton_);

	QHBoxLayout *topHL = new QHBoxLayout();
	topHL->addLayout(leftFL);
	topHL->addLayout(centerFL);
	topHL->addLayout(rightFL);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addLayout(topHL);
	mainVL->addLayout(buttonsHL);

	setLayout(mainVL);

	connect(addToWorkflowButton_, SIGNAL(clicked()), this, SLOT(onAddToWorkflowButtonClicked()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(close()));
}

void SGMFastScanCreateSampleScanListView::closeEvent(QCloseEvent *closeEvent){
	Q_UNUSED(closeEvent)
	configuration_->deleteLater();
	deleteLater();
}

void SGMFastScanCreateSampleScanListView::onAddToWorkflowButtonClicked(){
	AMListAction3 *masterList = new AMListAction3(new AMListActionInfo3(QString("%1 %2 - [%3 x %4]").arg(sampleComboBox_->currentText()).arg(configuration_->description()).arg(rowsSpinBox_->value()).arg(columnsSpinBox_->value()), QString("%1 %2 - [%3 x %4]").arg(sampleComboBox_->currentText()).arg(configuration_->description()).arg(rowsSpinBox_->value()).arg(columnsSpinBox_->value())), AMListAction3::Sequential);

	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

	if(oneTimeInitializationCheckBox_->isChecked()){
		SGMFastScanParameters *settings = configuration_->currentParameters();

		AMListAction3 *oneTimeInitializationList = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Initialization", "SGM Fast Actions Initialization"));

		// Grating to required value
		tmpControl = SGMBeamline::sgm()->grating();
		AMControlInfo gratingSetpoint = tmpControl->toInfo();
		gratingSetpoint.setValue(settings->scanInfo().start().sgmGrating());
		moveActionInfo = new AMControlMoveActionInfo3(gratingSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		oneTimeInitializationList->addSubAction(moveAction);

		// Tracking off:
		// Undulator Tracking to 0
		// Exit Slit Tracking to 0
		AMListAction3 *fastActionsTrackingOff = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Tracking Off", "SGM Fast Actions Tracking Off"), AMListAction3::Parallel);
		tmpControl = SGMBeamline::sgm()->undulatorTracking();
		AMControlInfo undulatorTrackingSetpoint = tmpControl->toInfo();
		undulatorTrackingSetpoint.setValue(0);
		moveActionInfo = new AMControlMoveActionInfo3(undulatorTrackingSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsTrackingOff->addSubAction(moveAction);
		tmpControl = SGMBeamline::sgm()->exitSlitTracking();
		AMControlInfo exitSlitTrackingSetpoint = tmpControl->toInfo();
		exitSlitTrackingSetpoint.setValue(0);
		moveActionInfo = new AMControlMoveActionInfo3(exitSlitTrackingSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsTrackingOff->addSubAction(moveAction);
		oneTimeInitializationList->addSubAction(fastActionsTrackingOff);
		// End Tracking Off

		// Initial velocity for mono and undulator ... if we haven't done a regular move in a while we can easily wind up with a slow velocity stuck in there
		AMListAction3 *fastActionsInitializeVelocity = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Initialize Velocity", "SGM Fast Actions Initialize Velocity"), AMListAction3::Parallel);

		tmpControl = SGMBeamline::sgm()->undulatorVelocity();
		AMControlInfo undulatorVelocityStartupSetpoint = tmpControl->toInfo();
		undulatorVelocityStartupSetpoint.setValue(11811);
		moveActionInfo = new AMControlMoveActionInfo3(undulatorVelocityStartupSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsInitializeVelocity->addSubAction(moveAction);
		tmpControl = SGMBeamline::sgm()->gratingVelocity();
		AMControlInfo gratingVelocityStartupSetpoint = tmpControl->toInfo();
		gratingVelocityStartupSetpoint.setValue(10000);
		moveActionInfo = new AMControlMoveActionInfo3(gratingVelocityStartupSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsInitializeVelocity->addSubAction(moveAction);

		oneTimeInitializationList->addSubAction(fastActionsInitializeVelocity);
		// End initial velocity

		// Initial positions while still moving quickly (energy and undulator)
		// Energy to start position
		// Undulator to start step
		AMListAction3 *fastActionsFastInitialPositions = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Fast Initial Positions", "SGM Fast Actions Fast Initial Positions"), AMListAction3::Parallel);
		tmpControl = SGMBeamline::sgm()->energy();
		AMControlInfo energyStartSetpoint = tmpControl->toInfo();
		energyStartSetpoint.setValue(configuration_->energyStart());
		moveActionInfo = new AMControlMoveActionInfo3(energyStartSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsFastInitialPositions->addSubAction(moveAction);

		tmpControl = SGMBeamline::sgm()->undulatorStep();
		AMControlInfo undulatorStepSetpoint = tmpControl->toInfo();
		undulatorStepSetpoint.setValue(configuration_->undulatorStartStep());
		moveActionInfo = new AMControlMoveActionInfo3(undulatorStepSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsFastInitialPositions->addSubAction(moveAction);

		oneTimeInitializationList->addSubAction(fastActionsFastInitialPositions);

		// Initialization:
		// Undulator Velocity to undulator velocity
		// Grating Velocity, Velocity Base, Acceleration to motor settings
		AMListAction3 *fastActionsInitializeTriggers = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Initialize Triggers", "SGM Fast Actions Initialize Triggers"), AMListAction3::Parallel);

		tmpControl = SGMBeamline::sgm()->undulatorVelocity();
		AMControlInfo undulatorVelocityInitSetpoint = tmpControl->toInfo();
		undulatorVelocityInitSetpoint.setValue(settings->undulatorVelocity());
		moveActionInfo = new AMControlMoveActionInfo3(undulatorVelocityInitSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsInitializeTriggers->addSubAction(moveAction);

		tmpControl = SGMBeamline::sgm()->gratingVelocity();
		AMControlInfo gratingVelocityInitSetpoint = tmpControl->toInfo();
		gratingVelocityInitSetpoint.setValue(settings->velocity());
		moveActionInfo = new AMControlMoveActionInfo3(gratingVelocityInitSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsInitializeTriggers->addSubAction(moveAction);
		tmpControl = SGMBeamline::sgm()->gratingBaseVelocity();
		AMControlInfo gratingBaseVelocityInitSetpoint = tmpControl->toInfo();
		gratingBaseVelocityInitSetpoint.setValue(settings->velocityBase());
		moveActionInfo = new AMControlMoveActionInfo3(gratingBaseVelocityInitSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsInitializeTriggers->addSubAction(moveAction);
		tmpControl = SGMBeamline::sgm()->gratingAcceleration();
		AMControlInfo gratingAccelerationInitSetpoint = tmpControl->toInfo();
		gratingAccelerationInitSetpoint.setValue(settings->acceleration());
		moveActionInfo = new AMControlMoveActionInfo3(gratingAccelerationInitSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsInitializeTriggers->addSubAction(moveAction);

		oneTimeInitializationList->addSubAction(fastActionsInitializeTriggers);
		// End Initialization

		// Initial Positions:
		// Exit Slit to median position
		// Continuous Scaler to Off
		AMListAction3 *fastActionsInitialPositions = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Initial Positions", "SGM Fast Actions Initial Positions"), AMListAction3::Parallel);

		tmpControl = SGMBeamline::sgm()->exitSlit();
		AMControlInfo exitSlitSetpoint = tmpControl->toInfo();
		exitSlitSetpoint.setValue(settings->exitSlitDistance());
		moveActionInfo = new AMControlMoveActionInfo3(exitSlitSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsInitialPositions->addSubAction(moveAction);
		fastActionsInitialPositions->addSubAction(SGMBeamline::sgm()->scaler()->createContinuousEnableAction3(false));
		oneTimeInitializationList->addSubAction(fastActionsInitialPositions);
		// End Initial Positions

		// Scaler and Synchronized Dwell Settings:
		// Dwell time to milliseconds (seconds/1000)
		// Scans per Buffer to 1000
		// Total Scans to 1000
		// Turn off synchronized dwell coordination for the scaler
		AMListAction3 *fastActionsScalerSettings = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Scaler Settings", "SGM Fast Actions Scaler Settings"), AMListAction3::Parallel);
		if(QApplication::instance()->arguments().contains("--advanced"))
			fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createDwellTimeAction3(settings->fastScanSettings().runSeconds()/1000));
		else
			fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createDwellTimeAction3(settings->scalerTime()/1000));
		fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createScansPerBufferAction3(1000));
		fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->scaler()->createTotalScansAction3(1000));
		oneTimeInitializationList->addSubAction(fastActionsScalerSettings);

		AMListAction3 *fastActionsScalerEnableSettings = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Scaler Enable Settings", "SGM Fast Actions Scaler Enable Settings"), AMListAction3::Parallel);
		for(int x = 0; x < 32; x++)
			fastActionsScalerEnableSettings->addSubAction(SGMBeamline::sgm()->scaler()->channelAt(x)->createEnableAction3(true));

		fastActionsScalerSettings->addSubAction(SGMBeamline::sgm()->synchronizedDwellTime()->createEnableAtAction3(0, false));
		oneTimeInitializationList->addSubAction(fastActionsScalerEnableSettings);
		// End Scaler Settings

		masterList->addSubAction(oneTimeInitializationList);
	}

	if(!SGMBeamline::sgm()->ssaManipulatorRot()->withinTolerance(rotationDoubleSpinBox_->value())){
		tmpControl = SGMBeamline::sgm()->ssaManipulatorRot();
		AMControlInfo rotationSetpoint = tmpControl->toInfo();
		rotationSetpoint.setValue(rotationDoubleSpinBox_->value());
		moveActionInfo = new AMControlMoveActionInfo3(rotationSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		masterList->addSubAction(moveAction);
	}

	AMSamplePlate *currentPlate = SGMBeamline::sgm()->samplePlate();
	if(currentPlate && sampleComboBox_->currentIndex() != 0){
		masterList->addSubAction(new AMSampleMoveAction(new AMSampleMoveActionInfo(currentPlate->sampleAt(sampleComboBox_->itemData(sampleComboBox_->currentIndex()).toInt()))));
	}

	AMListAction3 *innerLoopHolder = new AMListAction3(new AMListActionInfo3(QString("%1 Vertical Steps and Scans").arg(rowsSpinBox_->value()), QString("%1 Vertical Steps and Scans").arg(rowsSpinBox_->value())), AMListAction3::Sequential);
	AMScanAction *scanAction = new AMScanAction(new AMScanActionInfo(configuration_->createCopy()));
	if(rowsSpinBox_->value() > 1){
		AMLoopAction3 *innerLoop = new AMLoopAction3(new AMLoopActionInfo3(rowsSpinBox_->value()-1, QString("First %1 Vertical Steps and Scans").arg(rowsSpinBox_->value()-1), QString("First %1 Vertical Steps and Scans").arg(rowsSpinBox_->value()-1)));

		innerLoop->addSubAction(scanAction->createCopy());

		tmpControl = SGMBeamline::sgm()->ssaManipulatorZ();
		AMControlInfo verticalRelativeSetpoint = tmpControl->toInfo();
		verticalRelativeSetpoint.setValue(verticalStepSpinBox_->value());
		moveActionInfo = new AMControlMoveActionInfo3(verticalRelativeSetpoint);
		moveActionInfo->setIsRelativeMove(true);
		moveActionInfo->setIsRelativeFromSetpoint(true);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		innerLoop->addSubAction(moveAction);

		innerLoopHolder->addSubAction(innerLoop);
	}
	innerLoopHolder->addSubAction(scanAction);


	if(columnsSpinBox_->value() > 1){
		AMListAction3 *outerLoopHolder = new AMListAction3(new AMListActionInfo3(QString("%1 Columns").arg(columnsSpinBox_->value()), QString("%1 Columns").arg(rowsSpinBox_->value())), AMListAction3::Sequential);
		if(columnsSpinBox_->value() > 1){
			AMLoopAction3 *outerLoop = new AMLoopAction3(new AMLoopActionInfo3(columnsSpinBox_->value()-1, QString("First %1 Columns").arg(columnsSpinBox_->value()-1), QString("First %1 Columns").arg(rowsSpinBox_->value()-1)));
			outerLoop->addSubAction(innerLoopHolder->createCopy());

			tmpControl = SGMBeamline::sgm()->ssaManipulatorX();
			AMControlInfo horizontalRelativeSetpoint = tmpControl->toInfo();
			horizontalRelativeSetpoint.setValue(-horizontalStepSpinBox_->value());
			moveActionInfo = new AMControlMoveActionInfo3(horizontalRelativeSetpoint);
			moveActionInfo->setIsRelativeMove(true);
			moveActionInfo->setIsRelativeFromSetpoint(true);
			moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
			outerLoop->addSubAction(moveAction);

			tmpControl = SGMBeamline::sgm()->ssaManipulatorZ();
			AMControlInfo verticalLargeRelativeSetpoint = tmpControl->toInfo();
			verticalLargeRelativeSetpoint.setValue(-verticalStepSpinBox_->value()*rowsSpinBox_->value());
			moveActionInfo = new AMControlMoveActionInfo3(verticalLargeRelativeSetpoint);
			moveActionInfo->setIsRelativeMove(true);
			moveActionInfo->setIsRelativeFromSetpoint(true);
			moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
			outerLoop->addSubAction(moveAction);

			outerLoopHolder->addSubAction(outerLoop);
		}
		outerLoopHolder->addSubAction(innerLoopHolder);

		masterList->addSubAction(outerLoopHolder);
	}
	else{
		masterList->addSubAction(innerLoopHolder);
	}

	if(oneTimeInitializationCheckBox_->isChecked()){
		AMListAction3 *oneTimeCleanUp = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Cleanup", "SGM Fast Actions Cleanup"));

		// Tracking restore:
		// Undulator Tracking to current
		// Exit Slit Tracking to current
		AMListAction3 *fastActionsTrackingRestore = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Tracking Restore", "SGM Fast Actions Tracking Restore"), AMListAction3::Parallel);
		tmpControl = SGMBeamline::sgm()->undulatorTracking();
		AMControlInfo undulatorTrackingRestoreSetpoint = tmpControl->toInfo();
		undulatorTrackingRestoreSetpoint.setValue(SGMBeamline::sgm()->undulatorTracking()->value());
		moveActionInfo = new AMControlMoveActionInfo3(undulatorTrackingRestoreSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsTrackingRestore->addSubAction(moveAction);
		tmpControl = SGMBeamline::sgm()->exitSlitTracking();
		AMControlInfo exitSlitTrackingRestoreSetpoint = tmpControl->toInfo();
		exitSlitTrackingRestoreSetpoint.setValue(SGMBeamline::sgm()->exitSlitTracking()->value());
		moveActionInfo = new AMControlMoveActionInfo3(exitSlitTrackingRestoreSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsTrackingRestore->addSubAction(moveAction);
		oneTimeCleanUp->addSubAction(fastActionsTrackingRestore);
		// End Tracking Off

		// Grating Restore:
		// Grating Velocity, Velocity Base, Acceleration to current settings
		AMListAction3 *fastActionsGratingRestore = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Grating Restore", "SGM Fast Actions Grating Restore"), AMListAction3::Parallel);

		tmpControl = SGMBeamline::sgm()->gratingVelocity();
		AMControlInfo gratingVelocityRestoreSetpoint = tmpControl->toInfo();
		gratingVelocityRestoreSetpoint.setValue(SGMBeamline::sgm()->gratingVelocity()->value());
		moveActionInfo = new AMControlMoveActionInfo3(gratingVelocityRestoreSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsGratingRestore->addSubAction(moveAction);
		tmpControl = SGMBeamline::sgm()->gratingBaseVelocity();
		AMControlInfo gratingBaseVelocityRestoreSetpoint = tmpControl->toInfo();
		gratingBaseVelocityRestoreSetpoint.setValue(SGMBeamline::sgm()->gratingBaseVelocity()->value());
		moveActionInfo = new AMControlMoveActionInfo3(gratingBaseVelocityRestoreSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsGratingRestore->addSubAction(moveAction);
		tmpControl = SGMBeamline::sgm()->gratingAcceleration();
		AMControlInfo gratingAccelerationRestoreSetpoint = tmpControl->toInfo();
		gratingAccelerationRestoreSetpoint.setValue(SGMBeamline::sgm()->gratingAcceleration()->value());
		moveActionInfo = new AMControlMoveActionInfo3(gratingAccelerationRestoreSetpoint);
		moveAction = new AMControlMoveAction3(moveActionInfo, tmpControl);
		fastActionsGratingRestore->addSubAction(moveAction);

		oneTimeCleanUp->addSubAction(fastActionsGratingRestore);
		// End Grating Restore

		// Scaler and Synchronized Dwell Settings Restore:
		// Dwell time to current
		// Scans per Buffer to current
		// Total Scans to current
		AMListAction3 *fastActionsScalerSettingsRestore = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Scaler Settings Restore", "SGM Fast Actions Scaler Settings Restore"), AMListAction3::Parallel);
		fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->scaler()->createDwellTimeAction3(SGMBeamline::sgm()->scaler()->dwellTime()));
		fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->scaler()->createContinuousEnableAction3(SGMBeamline::sgm()->scaler()->isContinuous()));
		fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->scaler()->createScansPerBufferAction3(SGMBeamline::sgm()->scaler()->isContinuous() ? 1 : SGMBeamline::sgm()->scaler()->scansPerBuffer()));
		fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->scaler()->createTotalScansAction3(SGMBeamline::sgm()->scaler()->isContinuous() ? 1 : SGMBeamline::sgm()->scaler()->totalScans()));
		for(int x = 0; x < 32; x++)
			fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->scaler()->channelAt(x)->createEnableAction3(SGMBeamline::sgm()->scaler()->channelAt(x)->isEnabled()));

		fastActionsScalerSettingsRestore->addSubAction(SGMBeamline::sgm()->synchronizedDwellTime()->createEnableAtAction3(0, SGMBeamline::sgm()->synchronizedDwellTime()->enabledAt(0)));
		oneTimeCleanUp->addSubAction(fastActionsScalerSettingsRestore);
		// End Scaler Settings

		// Synchronized Dwell Restore:
		// Loop elements and enable prior settings
		AMListAction3 *fastActionsSyncDwellEnablesRestore = new AMListAction3(new AMListActionInfo3("SGM Fast Actions Synchronized Dwell Time Enable Settings Restore", "SGM Fast Actions Synchronized Dwell Time Enable Settings Restore"), AMListAction3::Parallel);

		CLSSynchronizedDwellTime *syncDwell = qobject_cast<CLSSynchronizedDwellTime*>(SGMBeamline::sgm()->synchronizedDwellTime());
		for(int x = 0; x < syncDwell->elementCount(); x++)
			fastActionsSyncDwellEnablesRestore->addSubAction(syncDwell->elementAt(x)->createEnableAction3(syncDwell->enabledAt(x)));
		oneTimeCleanUp->addSubAction(fastActionsSyncDwellEnablesRestore);
		// End Sync Dwell

		masterList->addSubAction(oneTimeCleanUp);
	}

	AMActionRunner3::workflow()->addActionToQueue(masterList);
	close();
}
