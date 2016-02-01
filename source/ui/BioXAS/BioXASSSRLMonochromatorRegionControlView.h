#ifndef BIOXASSSRLMONOCHROMATORREGIONCONTROLVIEW_H
#define BIOXASSSRLMONOCHROMATORREGIONCONTROLVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QGroupBox>

#include "ui/beamline/AMControlValueGreenLEDView.h"
#include "ui/beamline/AMControlValueRedLEDView.h"

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
	BioXASSSRLMonochromatorRegionControl* control() const { return control_; }

signals:
	/// Notifier that the region control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view, clears then updates it.
	void refresh();

	/// Sets the region control being viewed.
	void setControl(BioXASSSRLMonochromatorRegionControl *newControl);

protected:
	/// The region control being viewed.
	BioXASSSRLMonochromatorRegionControl *control_;

	/// Label displaying the slits status green LED.
	AMControlValueGreenLEDView *slitsStatusGreen_;
	/// Label displaying the slits status red LED.
	AMControlValueRedLEDView *slitsStatusRed_;

	/// Label displaying the paddle status green LED.
	AMControlValueGreenLEDView *paddleStatusGreen_;
	/// Label displaying the paddle status red LED.
	AMControlValueRedLEDView *paddleStatusRed_;

	/// Label displaying the key status green LED.
	AMControlValueGreenLEDView *keyStatusGreen_;
	/// Label displaying the key status red LED.
	AMControlValueRedLEDView *keyStatusRed_;

	/// Label displaying the brake status green LED.
	AMControlValueGreenLEDView *brakeStatusGreen_;
	/// Label displaying the brake status red LED.
	AMControlValueRedLEDView *brakeStatusRed_;

	/// Label displaying the 'bragg at crystal change position' status green LED.
	AMControlValueGreenLEDView *braggInPositionGreen_;
	/// Label displaying the 'bragg at crystal change position' status red LED.
	AMControlValueRedLEDView *braggInPositionRed_;

	/// Label displaying the region A status green LED.
	AMControlValueGreenLEDView *regionAStatusGreen_;
	/// Label displaying the region A status red LED.
	AMControlValueRedLEDView *regionAStatusRed_;

	/// Label displaying the region B status green LED.
	AMControlValueGreenLEDView *regionBStatusGreen_;
	/// Label displaying the region B status red LED.
	AMControlValueRedLEDView *regionBStatusRed_;
};

#endif // BIOXASSSRLMONOCHROMATORREGIONCONTROLVIEW_H
