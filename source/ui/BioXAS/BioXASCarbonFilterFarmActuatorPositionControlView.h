#ifndef BIOXASCARBONFILTERFARMACTUATORPOSITIONCONTROLVIEW_H
#define BIOXASCARBONFILTERFARMACTUATORPOSITIONCONTROLVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASCarbonFilterFarmActuatorPositionControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASCarbonFilterFarmActuatorPositionControlView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorPositionControlView(BioXASCarbonFilterFarmActuatorPositionControl *positionControl, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorPositionControlView();

	/// Returns the position control being viewed.
	BioXASCarbonFilterFarmActuatorPositionControl* positionControl();

signals:
	/// Notifier that the position control being viewed has changed.
	void positionChanged(BioXASCarbonFilterFarmActuatorPositionControl *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the actuator position control being viewed.
	void setPosition(BioXASCarbonFilterFarmActuatorPositionControl *newControl);

protected slots:
	/// Refreshes the position editor.
	void refreshPositionEditor();
	/// Refreshes the status editor.
	void refreshPositionStatusEditor();

protected:
	/// The position control being viewed.
	BioXASCarbonFilterFarmActuatorPositionControl *position_;

	/// The position editor.
	AMExtendedControlEditor *positionEditor_;
	/// The position status editor.
	AMExtendedControlEditor *positionStatusEditor_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORPOSITIONCONTROLVIEW_H
