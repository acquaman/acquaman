#ifndef BIOXASSSRLMONOCHROMATORREGIONCONTROLVIEW_H
#define BIOXASSSRLMONOCHROMATORREGIONCONTROLVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QGroupBox>

class BioXASSSRLMonochromatorRegionControl;

class BioXASSSRLMonochromatorRegionControlView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorRegionControlView(BioXASSSRLMonochromatorRegionControl *regionControl, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorRegionControlView();
	/// Returns the region control being viewed.
	BioXASSSRLMonochromatorRegionControl* regionControl() const { return regionControl_; }

signals:
	/// Notifier that the region control being viewed has changed.
	void regionControlChanged(BioXASSSRLMonochromatorRegionControl *newControl);

public slots:
	/// Sets the region control being viewed.
	void setRegionControl(BioXASSSRLMonochromatorRegionControl *newControl);

protected slots:
	/// Handles updating the slits status view when the mono's slits status changes.
	void onSlitsStatusChanged();
	/// Handles updating the paddle status view when the mono's paddle status changes.
	void onPaddleStatusChanged();
	/// Handles updating the key status view when the mono's key status changes.
	void onKeyStatusChanged();
	/// Handles updating the brake status view when the mono's brake status changes.
	void onBrakeStatusChanged();
	/// Handles updating the 'bragg at crystal change position' status when the mono indicates a change.
	void onBraggAtCrystalChangePositionStatusChanged();
	/// Handles updating the region A status view when the mono's region A status changes.
	void onRegionAStatusChanged();
	/// Handles updating the region B status view when the mono's region B status changes.
	void onRegionBStatusChanged();

protected:
	/// The region control being viewed.
	BioXASSSRLMonochromatorRegionControl *regionControl_;

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

	/// Label displaying the 'bragg at crystal change position' status green LED.
	QLabel *braggInPositionGreen_;
	/// Label displaying the 'bragg at crystal change position' status red LED.
	QLabel *braggInPositionRed_;

	/// Label displaying the region A status green LED.
	QLabel *regionAStatusGreen_;
	/// Label displaying the region A status red LED.
	QLabel *regionAStatusRed_;

	/// Label displaying the region B status green LED.
	QLabel *regionBStatusGreen_;
	/// Label displaying the region B status red LED.
	QLabel *regionBStatusRed_;
};

#endif // BIOXASSSRLMONOCHROMATORREGIONCONTROLVIEW_H
