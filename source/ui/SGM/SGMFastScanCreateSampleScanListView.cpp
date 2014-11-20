#include "SGMFastScanCreateSampleScanListView.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QBoxLayout>

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
	AMListAction3 *masterList = new AMListAction3(new AMListActionInfo3(QString("%1 %2").arg(sampleComboBox_->currentText()).arg(configuration_->description()), QString("%1 %2").arg(sampleComboBox_->currentText()).arg(configuration_->description())), AMListAction3::Sequential);

	AMControlMoveActionInfo3 *moveActionInfo;
	AMControlMoveAction3 *moveAction;
	AMControl *tmpControl;

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

	AMActionRunner3::workflow()->addActionToQueue(masterList);
	close();
}
