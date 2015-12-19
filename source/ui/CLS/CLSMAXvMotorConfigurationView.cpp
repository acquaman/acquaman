#include "CLSMAXvMotorConfigurationView.h"
#include "beamline/CLS/CLSMAXvMotor.h"

CLSMAXvMotorConfigurationView::CLSMAXvMotorConfigurationView(CLSMAXvMotor *motor, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	motor_ = 0;

	// Create UI elements.

	QLabel *velocityPrompt = new QLabel("Velocity:");
	velocityBox_ = new QDoubleSpinBox();

	QLabel *velocityBasePrompt = new QLabel("Velocity base:");
	velocityBaseBox_ = new QDoubleSpinBox();

	QLabel *accelerationPrompt = new QLabel("Acceleration:");
	accelerationBox_ = new QDoubleSpinBox();

	QLabel *encoderMoveTypePrompt = new QLabel("Encoder move type:");
	encoderMoveTypeBox_ = new QComboBox();

	QLabel *settlingTimePrompt = new QLabel("Settling time:");
	settlingTimeBox_ = new QDoubleSpinBox();

	// Create and set main layout.

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(velocityPrompt, 0, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(velocityBox_, 0, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(velocityBasePrompt, 1, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(velocityBaseBox_, 1, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(accelerationPrompt, 2, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(accelerationBox_, 2, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(encoderMoveTypePrompt, 3, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(encoderMoveTypeBox_, 3, 1, 1, 1, Qt::AlignLeft);
	layout->addWidget(settlingTimePrompt, 4, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(settlingTimeBox_, 4, 1, 1, 1, Qt::AlignLeft);

	setLayout(layout);

	// Make connections.

	connect( velocityBox_, SIGNAL(valueChanged(double)), this, SLOT(updateMotorVelocity()) );
	connect( velocityBaseBox_, SIGNAL(valueChanged(double)), this, SLOT(updateMotorVelocityBase()) );
	connect( accelerationBox_, SIGNAL(valueChanged(double)), this, SLOT(updateMotorAcceleration()) );
	connect( encoderMoveTypeBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateMotorEncoderMoveType()) );
	connect( settlingTimeBox_, SIGNAL(valueChanged(double)), this, SLOT(updateMotorSettlingTime()) );

	// Current settings.

	setMotor(motor);

	refresh();
}

CLSMAXvMotorConfigurationView::~CLSMAXvMotorConfigurationView()
{

}

void CLSMAXvMotorConfigurationView::refresh()
{
	// Clear the view.

	velocityBox_->clear();
	velocityBaseBox_->clear();
	accelerationBox_->clear();
	encoderMoveTypeBox_->clear();
	settlingTimeBox_->clear();

	// Set up view elements.

	velocityBox_->setMinimum(CLSMAXVMOTORCONFIGURATIONVIEW_VELOCITY_MIN);
	velocityBox_->setMaximum(CLSMAXVMOTORCONFIGURATIONVIEW_VELOCITY_MAX);
	velocityBox_->setSuffix(" deg/s");

	velocityBaseBox_->setMinimum(CLSMAXVMOTORCONFIGURATIONVIEW_BASE_VELOCITY_MIN);
	velocityBaseBox_->setMaximum(CLSMAXVMOTORCONFIGURATIONVIEW_BASE_VELOCITY_MAX);
	velocityBaseBox_->setSuffix(" deg/s");

	accelerationBox_->setMinimum(CLSMAXVMOTORCONFIGURATIONVIEW_ACCELERATION_MIN);
	accelerationBox_->setMaximum(CLSMAXVMOTORCONFIGURATIONVIEW_ACCELERATION_MAX);
	accelerationBox_->setSuffix(" deg/s/s");

	encoderMoveTypeBox_->addItem("Off", CLSMAXvMotor::EncoderMovementOff);
	encoderMoveTypeBox_->addItem("Increase Only", CLSMAXvMotor::EncoderMovementIncreaseOnly);
	encoderMoveTypeBox_->addItem("Decrease Only", CLSMAXvMotor::EncoderMovementIncrease);
	encoderMoveTypeBox_->addItem("Increase", CLSMAXvMotor::EncoderMovementIncrease);
	encoderMoveTypeBox_->addItem("Decrease", CLSMAXvMotor::EncoderMovementDecrease);

	settlingTimeBox_->setMinimum(CLSMAXVMOTORCONFIGURATIONVIEW_SETTLING_TIME_MIN);
	settlingTimeBox_->setMaximum(CLSMAXVMOTORCONFIGURATIONVIEW_SETTLING_TIME_MAX);
	settlingTimeBox_->setSuffix(" s");

	// Update view elements.

	updateVelocityBox();
	updateVelocityBaseBox();
	updateAccelerationBox();
	updateEncoderMoveTypeBox();
	updateSettlingTimeBox();
}

void CLSMAXvMotorConfigurationView::setMotor(CLSMAXvMotor *newMotor)
{
	if (motor_ != newMotor) {

		if (motor_)
			disconnect( motor_, 0, this, 0 );

		motor_ = newMotor;

		if (motor_) {
			connect( motor_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( motor_, SIGNAL(EGUVelocityChanged(double)), this, SLOT(updateVelocityBox()) );
			connect( motor_, SIGNAL(EGUBaseVelocityChanged(double)), this, SLOT(updateVelocityBaseBox()) );
			connect( motor_, SIGNAL(EGUAccelerationChanged(double)), this, SLOT(updateAccelerationBox()) );
			connect( motor_, SIGNAL(encoderMovementTypeChanged(CLSMAXvMotor::EncoderMovementType)), this, SLOT(updateEncoderMoveTypeBox()) );
			connect( motor_, SIGNAL(settlingTimeChanged(double)), this, SLOT(updateSettlingTimeBox()) );
		}

		refresh();

		emit motorChanged(motor_);
	}
}

void CLSMAXvMotorConfigurationView::updateVelocityBox()
{
	bool enabled = false;
	double value = 0;

	if (motor_) {
		enabled = true;
		value = motor_->EGUVelocity();
	}

	velocityBox_->blockSignals(true);
	velocityBox_->setEnabled(enabled);
	velocityBox_->setValue(value);
	velocityBox_->blockSignals(false);
}

void CLSMAXvMotorConfigurationView::updateVelocityBaseBox()
{
	bool enabled = false;
	double value = 0;

	if (motor_) {
		enabled = true;
		value = motor_->EGUBaseVelocity();
	}

	velocityBaseBox_->blockSignals(true);
	velocityBaseBox_->setEnabled(enabled);
	velocityBaseBox_->setValue(value);
	velocityBaseBox_->blockSignals(false);
}

void CLSMAXvMotorConfigurationView::updateAccelerationBox()
{
	bool enabled = false;
	double value = 0;

	if (motor_) {
		enabled = true;
		value = motor_->EGUAcceleration();
	}

	accelerationBox_->blockSignals(true);
	accelerationBox_->setEnabled(enabled);
	accelerationBox_->setValue(value);
	accelerationBox_->blockSignals(false);
}

void CLSMAXvMotorConfigurationView::updateEncoderMoveTypeBox()
{
	bool enabled = false;
	int index = 0;

	if (motor_) {
		enabled = true;
		index = encoderMoveTypeBox_->findData(int(motor_->encoderMovementType()));
	}

	encoderMoveTypeBox_->blockSignals(true);
	encoderMoveTypeBox_->setEnabled(enabled);
	encoderMoveTypeBox_->setCurrentIndex(index);
	encoderMoveTypeBox_->blockSignals(false);
}

void CLSMAXvMotorConfigurationView::updateSettlingTimeBox()
{
	bool enabled = false;
	double value = 0;

	if (motor_) {
		enabled = true;
		value = motor_->settlingTime();
	}

	settlingTimeBox_->blockSignals(true);
	settlingTimeBox_->setEnabled(enabled);
	settlingTimeBox_->setValue(value);
	settlingTimeBox_->blockSignals(false);
}

void CLSMAXvMotorConfigurationView::updateMotorVelocity()
{
	if (motor_)
		motor_->setEGUVelocity(velocityBox_->value());
}

void CLSMAXvMotorConfigurationView::updateMotorVelocityBase()
{
	if (motor_)
		motor_->setEGUBaseVelocity(velocityBaseBox_->value());
}

void CLSMAXvMotorConfigurationView::updateMotorAcceleration()
{
	if (motor_)
		motor_->setEGUAcceleration(accelerationBox_->value());
}

void CLSMAXvMotorConfigurationView::updateMotorEncoderMoveType()
{
	int movementIndex = encoderMoveTypeBox_->itemData(encoderMoveTypeBox_->currentIndex()).toInt();

	if (motor_)
		motor_->setEncoderMovementType(CLSMAXvMotor::EncoderMovementType(movementIndex));
}

void CLSMAXvMotorConfigurationView::updateMotorSettlingTime()
{
	if (motor_)
		motor_->setSettlingTime(settlingTimeBox_->value());
}
