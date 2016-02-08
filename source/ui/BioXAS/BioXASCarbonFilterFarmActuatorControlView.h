#ifndef BIOXASCARBONFILTERFARMACTUATORCONTROLVIEW_H
#define BIOXASCARBONFILTERFARMACTUATORCONTROLVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorControl.h"

class BioXASControlEditor;

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
	void refreshPositionEditor();
	/// Refreshes the position status editor.
	void refreshPositionStatusEditor();

protected:
	/// The actuator control being viewed.
	BioXASCarbonFilterFarmActuatorControl *actuator_;

	/// The filter editor.
	BioXASControlEditor *filterEditor_;
	/// The window editor.
	BioXASControlEditor *windowEditor_;
	/// The position editor.
	BioXASControlEditor *positionEditor_;
	/// The position status editor.
	BioXASControlEditor *positionStatusEditor_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORCONTROLVIEW_H
