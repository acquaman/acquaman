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
	/// Handles updating the slits status view when the mono's slits status changes.
	void onSlitsStatusChanged();
	/// Handles updating the paddle status view when the mono's paddle status changes.
	void onPaddleStatusChanged();
	/// Handles updating the key status view when the mono's key status changes.
	void onKeyStatusChanged();
	/// Handles updating the brake status view when the mono's brake status changes.
	void onBrakeStatusChanged();
	/// Handles updating the crystal 1 status view when the crystal change motor indicates it's at a cw limit.
	void onCWLimitStatusChanged();
	/// Handles updating the crystal 2 status view when the crystal change motor indicates it's at a ccw limit.
	void onCCWLimitStatusChanged();

protected:
	/// The monochromator being viewed.
	BioXASMainMonochromator *mono_;

	/// Label displaying the slits status green LED.
	QLabel *slitsStatusGreen_;
	/// Label displaying the slits status red LED.
	QLabel *slitsStatusRed_;
	/// Label displaying the paddle status green LED.
	QLabel *paddleStatusGreen_;
	/// Label displaying the paddle status red LED.
	QLabel *paddleStatusRed_;
	/// Label displaying the key status green LED.
	QLabel *keyStatusGreen_;
	/// Label displaying the key status red LED.
	QLabel *keyStatusRed_;
	/// Label displaying the brake status green LED.
	QLabel *brakeStatusGreen_;
	/// Label displaying the brake status red LED.
	QLabel *brakeStatusRed_;
	/// Label displaying the crystal 1 active green LED.
	QLabel *crystal1StatusGreen_;
	/// Label displaying the crystal 1 active red LED.
	QLabel *crystal1StatusRed_;
	/// Label displaying the crystal 2 active green LED.
	QLabel *crystal2StatusGreen_;
	/// Label displaying the crystal 2 active red LED.
	QLabel *crystal2StatusRed_;
};

#endif // BIOXASMAINMONOCHROMATORREGIONVIEW_H
