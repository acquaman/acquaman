#ifndef BIOXASMAINMONOCHROMATORREGIONVIEW_H
#define BIOXASMAINMONOCHROMATORREGIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>

#include "beamline/BioXAS/BioXASMainBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASMainMonochromatorRegionControlView.h"

class BioXASMainMonochromatorRegionView : public QGroupBox
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMainMonochromatorRegionView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASMainMonochromatorRegionView();

protected slots:
	/// Handles updating the view when the mono's region control connected state changes.
	void onRegionControlConnectedChanged();
	/// Handles updating the view when the mono's key status changes.
	void onKeyStatusChanged();
	/// Handles updating the view when the mono's brake status changes.
	void onBrakeStatusChanged();
	/// Handles initiating a region change.
	void onRegionButtonClicked();

protected:
	/// The monochromator being viewed.
	BioXASMainMonochromator *mono_;
	/// Editor for the mono's region control.
	AMExtendedControlEditor *regionEditor_;
	/// Editor for the mono's upper slit blade motor.
	AMExtendedControlEditor *upperSlitEditor_;
	/// Editor for the mono's lower slit blade motor.
	AMExtendedControlEditor *lowerSlitEditor_;
	/// Editor for the mono's bragg motor.
	AMExtendedControlEditor *braggEditor_;
	/// Editor for the mono's crystal change motor clockwise limit control.
	AMExtendedControlEditor *crystalChangeCWEditor_;
	/// Editor for the mono's crystal change motor counter-clockwise limit control.
	AMExtendedControlEditor *crystalChangeCCWEditor_;
	/// Label displaying the key status text.
	QLabel *keyStatusText_;
	/// Label displaying the key status LED.
	QLabel *keyStatusLED_;
	/// Label displaying the brake status text.
	QLabel *brakeStatusText_;
	/// Label displaying the brake status LED.
	QLabel *brakeStatusLED_;
	/// Button for initiating a region change.
	QPushButton *regionButton_;
};

#endif // BIOXASMAINMONOCHROMATORREGIONVIEW_H
