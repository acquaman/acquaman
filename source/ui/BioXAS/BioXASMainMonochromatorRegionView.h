#ifndef BIOXASMAINMONOCHROMATORREGIONVIEW_H
#define BIOXASMAINMONOCHROMATORREGIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>

#include "beamline/BioXAS/BioXASMainMonochromator.h"
#include "ui/BioXAS/BioXASCLSMAXvMotorControlEditor.h"
#include "ui/BioXAS/BioXASMainMonochromatorRegionControlView.h"

class BioXASMainMonochromatorRegionView : public QGroupBox
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainMonochromatorRegionView(BioXASMainMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromatorRegionView();
	/// Returns the mono being viewed.
	BioXASMainMonochromator* mono() const { return mono_; }

signals:
	/// Notifier that the monochromator being viewed has changed.
	void monoChanged(BioXASMainMonochromator *newMono);

public slots:
	/// Sets the monochromator being viewed.
	void setMono(BioXASMainMonochromator *newMono);

protected slots:
	/// Handles updating the view when the mono's connected state changes.
	void onConnectedChanged();
	/// Handles updating the view when the mono's region status changes.
	void onRegionChanged();
	/// Handles updating the view when the mono's key status changes.
	void onKeyStatusChanged();
	/// Handles updating the view when the mono's brake status changes.
	void onBrakeStatusChanged();
	/// Handles initiating a region change.
	void onRegionButtonClicked();

protected:
	/// The monochromator being viewed.
	BioXASMainMonochromator *mono_;
	/// Label displaying the current region text.
	QLabel *regionText_;
	/// Label displaying the key status text.
	QLabel *keyStatusText_;
	/// Label displaying the key status LED.
	QLabel *keyStatusLED_;
	/// Label displaying the brake status text.
	QLabel *brakeStatusText_;
	/// Label displaying the brake status LED.
	QLabel *brakeStatusLED_;
//	/// Editor for the mono's upper slit blade motor.
//	BioXASCLSMAXvMotorControlEditor *upperSlitEditor_;
//	/// Editor for the mono's lower slit blade motor.
//	BioXASCLSMAXvMotorControlEditor *lowerSlitEditor_;
//	/// Editor for the mono's bragg motor.
//	BioXASCLSMAXvMotorControlEditor *braggEditor_;
//	/// Editor for the mono's crystal change motor.
//	BioXASCLSMAXvMotorControlEditor *crystalChangeEditor_;
	/// Button for initiating a region change.
	QPushButton *regionButton_;

};

#endif // BIOXASMAINMONOCHROMATORREGIONVIEW_H
