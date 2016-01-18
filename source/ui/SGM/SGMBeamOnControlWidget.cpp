#include "SGMBeamOnControlWidget.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include "beamline/AMControlSet.h"

SGMBeamOnControlWidget::SGMBeamOnControlWidget(AMControl* beamStatusControl,
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

void SGMBeamOnControlWidget::onRequiredControlsConnectionChanged(bool isConnected)
{
	if(isConnected && !connectedOnce_) {

		connectedOnce_ = true;
		setupData();
		setupConnections();
	}
}

void SGMBeamOnControlWidget::onBeamOnButtonPushed()
{
	if(beamStatusControl_->withinTolerance(0)) {
		beamOnControl_->move(1);
	}
}

void SGMBeamOnControlWidget::onBeamOffButtonPushed()
{
	if(beamStatusControl_->withinTolerance(1)) {
		beamOffControl_->move(1);
	}
}

void SGMBeamOnControlWidget::onBeamStatusPVChanged()
{
	if(beamStatusControl_->withinTolerance(0)) {

		beamOffButton_->setChecked(true);
		beamOffButton_->setEnabled(false);
		beamOnButton_->setChecked(false);
		beamOnButton_->setEnabled(true);
		beamStatusIcon_->setPixmap(QIcon(":/22x22/greenLEDOff.png").pixmap(22));
		beamStatusLabel_->setText("Beam Status: Off");

	} else if(beamStatusControl_->withinTolerance(1)) {

		beamOnButton_->setChecked(true);
		beamOnButton_->setEnabled(false);
		beamOffButton_->setChecked(false);
		beamOffButton_->setEnabled(true);
		beamStatusIcon_->setPixmap(QIcon(":/22x22/greenLEDOn.png").pixmap(22));
		beamStatusLabel_->setText("Beam Status: On");

	} else {

		beamOffButton_->setEnabled(false);
		beamOnButton_->setEnabled(false);
		beamStatusIcon_->setPixmap(QIcon(":/22x22/yellowLEDOn.png").pixmap(22));
		beamStatusLabel_->setText("Beam Status: Moving");
	}
}

void SGMBeamOnControlWidget::setupUi()
{

	QGridLayout* mainLayout = new QGridLayout();
	setLayout(mainLayout);
	beamOnButton_ = new QPushButton("On");
	beamOnButton_->setCheckable(true);
	beamOnButton_->setEnabled(false);
	beamOffButton_ = new QPushButton("Off");
	beamOffButton_->setCheckable(true);
	beamOffButton_->setEnabled(false);

	beamStatusLabel_ = new QLabel("Not Connected");
	beamStatusIcon_ = new QLabel();

	QHBoxLayout* statusLayout = new QHBoxLayout();
	statusLayout->addWidget(beamStatusIcon_);
	statusLayout->addWidget(beamStatusLabel_);

	mainLayout->addLayout(statusLayout, 0, 0);
	mainLayout->addWidget(beamOnButton_, 1, 0);
	mainLayout->addWidget(beamOffButton_, 1, 1);
}

void SGMBeamOnControlWidget::setupData()
{
	onBeamStatusPVChanged();
}

void SGMBeamOnControlWidget::setupConnections()
{
	connect(beamOnButton_, SIGNAL(clicked(bool)), this, SLOT(onBeamOnButtonPushed()));
	connect(beamOffButton_, SIGNAL(clicked(bool)), this, SLOT(onBeamOffButtonPushed()));
	connect(beamStatusControl_, SIGNAL(valueChanged(double)), this, SLOT(onBeamStatusPVChanged()));
}



