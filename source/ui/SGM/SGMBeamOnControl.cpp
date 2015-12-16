#include "SGMBeamOnControl.h"
#include <QGridLayout>
#include "beamline/AMControlSet.h"
#include <QDebug>
SGMBeamOnControl::SGMBeamOnControl(AMControl* beamStatusControl,
				   AMControl* beamOnControl,
				   AMControl* beamOffControl,
				   QWidget *parent) :
    QWidget(parent)
{
	connectedOnce_ = false;
	setupUi();
	AMControlSet* requiredControls = new AMControlSet(this);

	beamStatusControl_ = beamStatusControl;
	beamOnControl_ = beamOnControl;
	beamOffControl_ = beamOffControl;

	requiredControls->addControl(beamStatusControl_);
	requiredControls->addControl(beamOnControl_);
	requiredControls->addControl(beamOffControl_);

	connect(requiredControls, SIGNAL(connected(bool)), this, SLOT(onRequiredControlsConnectionChanged(bool)));

	if(beamOnControl_->isConnected() &&
			beamOffControl_->isConnected() &&
			beamStatusControl_->isConnected()) {
		onRequiredControlsConnectionChanged(true);
	}

}

void SGMBeamOnControl::onRequiredControlsConnectionChanged(bool isConnected)
{
	qDebug() << "\t\tIs Connected:"<<isConnected;
	qDebug() << "\t\tIs status connected:" <<beamStatusControl_->isConnected();
	qDebug() << "\t\tIs beam on connected:" << beamOnControl_->isConnected();
	qDebug() << "\t\tIs beam off connected:" << beamOffControl_->isConnected();

	if(isConnected && !connectedOnce_) {

		connectedOnce_ = true;
		setupData();
		setupConnections();
	}
}

void SGMBeamOnControl::onBeamOnButtonPushed()
{
	if(beamStatusControl_->withinTolerance(0)) {
		beamOnControl_->move(1);
	}
}

void SGMBeamOnControl::onBeamOffButtonPushed()
{
	if(beamStatusControl_->withinTolerance(1)) {
		beamOffControl_->move(1);
	}
}

void SGMBeamOnControl::onBeamStatusPVChanged()
{
	if(beamStatusControl_->withinTolerance(0)) {

		qDebug() << "\tstatus is 0, disbling off, enabling on";
		beamOffButton_->setChecked(true);
		beamOffButton_->setEnabled(false);
		beamOnButton_->setChecked(false);
		beamOnButton_->setEnabled(true);
		beamStatusLabel_->setText("Beam Status: Off");

	} else if(beamStatusControl_->withinTolerance(1)) {

		qDebug() << "\tstatus is 1, disbling on, enabling off";
		beamOnButton_->setChecked(true);
		beamOnButton_->setEnabled(false);
		beamOffButton_->setChecked(false);
		beamOffButton_->setEnabled(true);
		beamStatusLabel_->setText("Beam Status: On");

	} else {

		qDebug() << "\tstatus is 2, disabling both";
		beamOffButton_->setEnabled(false);
		beamOnButton_->setEnabled(false);
		beamStatusLabel_->setText("Beam Status: Moving");
	}
}

void SGMBeamOnControl::setupUi()
{

	QGridLayout* mainLayout = new QGridLayout();
	setLayout(mainLayout);
	qDebug() << "\tInitializing the controls disabled";
	beamOnButton_ = new QPushButton("On");
	beamOnButton_->setCheckable(true);
	beamOnButton_->setEnabled(false);
	beamOffButton_ = new QPushButton("Off");
	beamOffButton_->setCheckable(true);
	beamOffButton_->setEnabled(false);
	beamStatusLabel_ = new QLabel("Not Connected");

	mainLayout->addWidget(beamStatusLabel_, 0, 0);
	mainLayout->addWidget(beamOnButton_, 1, 0);
	mainLayout->addWidget(beamOffButton_, 2, 0);
}

void SGMBeamOnControl::setupData()
{
	onBeamStatusPVChanged();
}

void SGMBeamOnControl::setupConnections()
{
	connect(beamOnButton_, SIGNAL(clicked(bool)), this, SLOT(onBeamOnButtonPushed()));
	connect(beamOffButton_, SIGNAL(clicked(bool)), this, SLOT(onBeamOffButtonPushed()));
}

QString SGMBeamOnControl::beamStatusText()
{
	if(!beamStatusControl_->isConnected()) {
		return "Not Connected";
	} else if(beamStatusControl_->withinTolerance(0)) {
		return "Beam Off";
	} else if(beamStatusControl_->withinTolerance(1)) {
		return "Beam On";
	} else {
		return "Moving";
	}
}



