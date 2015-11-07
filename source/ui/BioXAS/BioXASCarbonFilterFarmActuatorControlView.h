#ifndef BIOXASCARBONFILTERFARMACTUATORCONTROLVIEW_H
#define BIOXASCARBONFILTERFARMACTUATORCONTROLVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASCarbonFilterFarmActuatorPositionControlView.h"

class BioXASCarbonFilterFarmActuatorControlView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorControlView(BioXASCarbonFilterFarmActuatorControl *actuator, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorControlView();

	/// Returns the actuator control being viewed.
	BioXASCarbonFilterFarmActuatorControl* actuator() const { return actuator_; }

signals:
	/// Notifier that the actuator control being viewed has changed.
	void actuatorChanged(AMControl *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the actuator control being viewed.
	void setActuator(BioXASCarbonFilterFarmActuatorControl *newControl);

protected slots:
	/// Refreshes the filter editor.
	void refreshFilterEditor();
	/// Refreshes the window editor.
	void refreshWindowEditor();
	/// Refreshes the position editor.
	void refreshPositionView();

protected:
	/// The actuator control being viewed.
	BioXASCarbonFilterFarmActuatorControl *actuator_;

	/// The filter editor.
	AMExtendedControlEditor *filterEditor_;
	/// The window editor.
	AMExtendedControlEditor *windowEditor_;
	/// The position view.
	BioXASCarbonFilterFarmActuatorPositionControlView *positionView_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORCONTROLVIEW_H
