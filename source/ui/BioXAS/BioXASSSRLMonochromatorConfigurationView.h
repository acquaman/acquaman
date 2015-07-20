#ifndef BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H

#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QInputDialog>

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"

#define ENERGY_MIN -1000000
#define ENERGY_MAX 1000000
#define BRAGG_POSITION_MIN -1000000
#define BRAGG_POSITION_MAX 1000000
#define SETTLING_TIME_MIN 0
#define SETTLING_TIME_MAX 100
#define VIEW_WIDTH_MIN 300

class BioXASSSRLMonochromatorBraggConfigurationView;

class BioXASSSRLMonochromatorConfigurationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorConfigurationView();

	/// Returns the mono being viewed.
	BioXASSSRLMonochromator* mono() const { return mono_; }

signals:
	/// Notifier that the mono being viewed has changed.
	void monoChanged(BioXASSSRLMonochromator *newMono);

public slots:
	/// Sets the mono being viewed.
	void setMono(BioXASSSRLMonochromator *newMono);

protected slots:
	/// Creates and displays a basic calibration screen for the energy.
	void onCalibrateEnergyButtonClicked();
	/// Creates and displays a basic calibration screen for the goniometer.
	void onCalibrateGoniometerButtonClicked();

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

	/// The upper slit blade editor.
	AMExtendedControlEditor *upperSlitEditor_;
	/// The lower slit blade editor.
	AMExtendedControlEditor *lowerSlitEditor_;
	/// The height editor.
	AMExtendedControlEditor *heightEditor_;
	/// The lateral editor.
	AMExtendedControlEditor *lateralEditor_;
	/// The paddle editor.
	AMExtendedControlEditor *paddleEditor_;
	/// The crystal 1 pitch editor.
	AMExtendedControlEditor *crystal1PitchEditor_;
	/// The crystal 1 roll editor.
	AMExtendedControlEditor *crystal1RollEditor_;
	/// The crystal 2 pitch editor.
	AMExtendedControlEditor *crystal2PitchEditor_;
	/// The crystal 2 roll editor.
	AMExtendedControlEditor *crystal2RollEditor_;

	/// The step-based energy editor.
	AMExtendedControlEditor *stepEnergyEditor_;
	/// The encoder-based energy editor.
	AMExtendedControlEditor *encoderEnergyEditor_;
	/// The step-based bragg editor.
	AMExtendedControlEditor *stepBraggEditor_;
	/// The encoder-based bragg editor.
	AMExtendedControlEditor *encoderBraggEditor_;
	/// The m1 mirror pitch editor.
	AMExtendedControlEditor *m1PitchEditor_;

	/// The calibrate energy button.
	QPushButton *calibrateEnergyButton_;
	/// The calibrate goniometer button.
	QPushButton *calibrateGoniometerButton_;

	/// The region editor.
	BioXASSSRLMonochromatorRegionControlEditor *regionEditor_;
	/// The region status display.
	BioXASSSRLMonochromatorRegionControlView *regionStatusWidget_;

	/// The bragg configuration view.
	BioXASSSRLMonochromatorBraggConfigurationView *braggConfigWidget_;
};









#define BRAGG_VELOCITY_MIN 0
#define BRAGG_VELOCITY_MAX 1000
#define BRAGG_BASE_VELOCITY_MIN 0
#define BRAGG_BASE_VELOCITY_MAX 1000
#define BRAGG_ACCELERATION_MIN 0
#define BRAGG_ACCELERATION_MAX 1000
#define BRAGG_SETTLING_TIME_MIN 0
#define BRAGG_SETTLING_TIME_MAX 1000

class BioXASSSRLMonochromatorBraggConfigurationView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorBraggConfigurationView(CLSMAXvMotor *braggMotor, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorBraggConfigurationView();

	/// Returns the bragg motor being viewed.
	CLSMAXvMotor* braggMotor() const { return braggMotor_; }

signals:
	/// Notifier that the bragg motor being viewed has changed.
	void braggMotorChanged(CLSMAXvMotor *newMotor);

public slots:
	/// Sets the bragg motor being viewed.
	void setBraggMotor(CLSMAXvMotor *newMotor);

protected slots:
	/// Clears the UI.
	void clearUI();
	/// Initializes the UI.
	void initializeUI();
	/// Clears and then initializes the UI.
	void refreshUI();

	/// Updates the view when the bragg motor connected state changes.
	void onBraggMotorConnectedChanged();
	/// Updates the view when the bragg motor velocity changes.
	void onBraggMotorVelocityChanged();
	/// Updates the view when the bragg motor velocity base changes.
	void onBraggMotorVelocityBaseChanged();
	/// Updates the view when the bragg motor acceleration changes.
	void onBraggMotorAccelerationChanged();
	/// Updates the view when the bragg motor encoder move type has changed.
	void onBraggMotorEncoderMoveTypeChanged();
	/// Updates the view when the bragg motor settling time changes.
	void onBraggMotorSettlingTimeChanged();

	/// Sets the bragg motor velocity when a user selection has been made.
	void setBraggMotorVelocity(double newVelocity);
	/// Sets the bragg motor velocity base when a user selection has been made.
	void setBraggMotorVelocityBase(double newBase);
	/// Sets the bragg motor acceleration when a user selection has been made.
	void setBraggMotorAcceleration(double newAcceleration);
	/// Sets the bragg motor encoder movement type when a user selection has been made.
	void setBraggMotorEncoderMovementType(int newType);
	/// Sets the bragg motor settling time.
	void setBraggMotorSettlingTime(double seconds);

protected:
	/// The bragg motor being displayed.
	CLSMAXvMotor *braggMotor_;

	/// Display for the bragg motor velocity.
	QDoubleSpinBox *braggVelocity_;
	/// Display for the bragg motor velocity base.
	QDoubleSpinBox *braggVelocityBase_;
	/// Display for the bragg motor acceleration.
	QDoubleSpinBox *braggAcceleration_;
	/// Display for the bragg motor encoder move type.
	QComboBox *braggEncoderMoveType_;
	/// Display for the bragg motor settling time.
	QDoubleSpinBox *braggSettlingTime_;
};

#endif // BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
