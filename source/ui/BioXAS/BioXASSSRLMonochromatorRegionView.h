#ifndef BIOXASSSRLMONOCHROMATORREGIONVIEW_H
#define BIOXASSSRLMONOCHROMATORREGIONVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QGroupBox>

#include "beamline/BioXAS/BioXASMainBeamline.h"

class BioXASSSRLMonochromatorRegionView : public QGroupBox
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorRegionView(BioXASSSRLMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorRegionView();
	/// Returns the monochromator being viewed.
	BioXASSSRLMonochromator* mono() const { return mono_; }

signals:
	/// Notifier that the monochromator being viewed has changed.
	void monoChanged(BioXASSSRLMonochromator *newMono);

public slots:
	/// Sets the mono being viewed.
	void setMono(BioXASSSRLMonochromator *newMono);

protected slots:
	/// Handles updating the slits status view when the mono's slits status changes.
	void onSlitsStatusChanged();
	/// Handles updating the paddle status view when the mono's paddle status changes.
	void onPaddleStatusChanged();
	/// Handles updating the key status view when the mono's key status changes.
	void onKeyStatusChanged();
	/// Handles updating the brake status view when the mono's brake status changes.
	void onBrakeStatusChanged();

protected:
	/// The monochromator being viewed.
	BioXASSSRLMonochromator *mono_;

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
};

#endif // BIOXASSSRLMONOCHROMATORREGIONVIEW_H
