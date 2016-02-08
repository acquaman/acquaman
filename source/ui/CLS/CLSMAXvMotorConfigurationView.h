#ifndef CLSMAXVMOTORCONFIGURATIONVIEW_H
#define CLSMAXVMOTORCONFIGURATIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>

#define CLSMAXVMOTORCONFIGURATIONVIEW_VELOCITY_MIN 0
#define CLSMAXVMOTORCONFIGURATIONVIEW_VELOCITY_MAX 1000
#define CLSMAXVMOTORCONFIGURATIONVIEW_BASE_VELOCITY_MIN 0
#define CLSMAXVMOTORCONFIGURATIONVIEW_BASE_VELOCITY_MAX 1000
#define CLSMAXVMOTORCONFIGURATIONVIEW_ACCELERATION_MIN 0
#define CLSMAXVMOTORCONFIGURATIONVIEW_ACCELERATION_MAX 1000
#define CLSMAXVMOTORCONFIGURATIONVIEW_SETTLING_TIME_MIN 0
#define CLSMAXVMOTORCONFIGURATIONVIEW_SETTLING_TIME_MAX 1000

class CLSMAXvMotor;

class CLSMAXvMotorConfigurationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSMAXvMotorConfigurationView(CLSMAXvMotor *motor, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSMAXvMotorConfigurationView();

	/// Returns the motor being viewed.
	CLSMAXvMotor* motor() const { return motor_; }

signals:
	/// Notifier that the motor being viewed has changed.
	void motorChanged(CLSMAXvMotor *newMotor);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the motor being viewed.
	void setMotor(CLSMAXvMotor *newMotor);

protected slots:
	/// Updates the velocity box.
	void updateVelocityBox();
	/// Updates the velocity base box.
	void updateVelocityBaseBox();
	/// Updates the acceleration box.
	void updateAccelerationBox();
	/// Updates the encoder movement type box.
	void updateEncoderMoveTypeBox();
	/// Updates the settling time box.
	void updateSettlingTimeBox();

	/// Updates the motor's velocity.
	void updateMotorVelocity();
	/// Updates the motor's velocity base.
	void updateMotorVelocityBase();
	/// Updates the motor's acceleration.
	void updateMotorAcceleration();
	/// Updates the motor's encoder movement type.
	void updateMotorEncoderMoveType();
	/// Updates the motor's settling time.
	void updateMotorSettlingTime();

protected:
	/// The motor being viewed.
	CLSMAXvMotor *motor_;

	/// The velocity box.
	QDoubleSpinBox *velocityBox_;
	/// The velocity base box.
	QDoubleSpinBox *velocityBaseBox_;
	/// The acceleration box.
	QDoubleSpinBox *accelerationBox_;
	/// The encoder move type box.
	QComboBox *encoderMoveTypeBox_;
	/// The settling time box.
	QDoubleSpinBox *settlingTimeBox_;
};

#endif // CLSMAXVMOTORCONFIGURATIONVIEW_H
