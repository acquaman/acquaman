#ifndef BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H

#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLayout>
#include <QLabel>

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"

#define BRAGG_OFFSET_MIN -1000
#define BRAGG_OFFSET_MAX 1000

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

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;
	/// The region status display.
	BioXASSSRLMonochromatorRegionControlView *regionStatusWidget_;
	/// The bragg configuration view.
	BioXASSSRLMonochromatorBraggConfigurationView *braggConfigWidget_;
};









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
	/// Updates the view when the bragg motor offset has changed.
	void onBraggMotorOffsetChanged();
	/// Updates the view when the bragg encoder move type has changed.
	void onBraggMotorEncoderMoveTypeChanged();
	/// Updates the view when the bragg motor settling time has changed.
	void onBraggMotorSettlingTimeChanged();

	/// Sets the bragg motor offset when a user selection has been made.
	void setBraggMotorOffset(double newValue);
	/// Sets the bragg motor encoder movement type when a user selection has been made.
	void setBraggMotorEncoderMovementType(int newType);
	/// Sets the bragg motor settling time when a user selection has been made.
	void setBraggMotorSettlingTime(double newTime);

protected:
	/// The bragg motor being displayed.
	CLSMAXvMotor *braggMotor_;

	/// Display for the bragg motor degree offset.
	QDoubleSpinBox *braggOffset_;
	/// Display for the bragg motor encoder move type.
	QComboBox *braggEncoderMoveType_;
	/// Display for the bragg motor move settling time.
	QDoubleSpinBox *braggSettlingTime_;
};

#endif // BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
