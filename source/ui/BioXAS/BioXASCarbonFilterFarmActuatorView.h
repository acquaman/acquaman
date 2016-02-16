#ifndef BIOXASCARBONFILTERFARMACTUATORVIEW_H
#define BIOXASCARBONFILTERFARMACTUATORVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASCarbonFilterFarmActuator;
class BioXASControlEditor;

class BioXASCarbonFilterFarmActuatorView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmActuatorView(BioXASCarbonFilterFarmActuator *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmActuatorView();

	/// Returns the actuator control being viewed.
	BioXASCarbonFilterFarmActuator* control() const { return actuator_; }

signals:
	/// Notifier that the actuator control being viewed has changed.
	void controlChanged(BioXASCarbonFilterFarmActuator *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the actuator control being viewed.
	void setControl(BioXASCarbonFilterFarmActuator *newControl);

protected slots:
	/// Updates the filter editor.
	void updateFilterEditor();
	/// Updates the window editor.
	void updateWindowEditor();
	/// Updates the motor editor.
	void updateMotorEditor();
	/// Updates the position status editor.
	void updatePositionStatusEditor();

protected:
	/// The actuator control being viewed.
	BioXASCarbonFilterFarmActuator *actuator_;

	/// The filter editor.
	BioXASControlEditor *filterEditor_;
	/// The window editor.
	BioXASControlEditor *windowEditor_;
	/// The position editor.
	BioXASControlEditor *motorEditor_;
	/// The position status editor.
	BioXASControlEditor *positionStatusEditor_;
};

#endif // BIOXASCARBONFILTERFARMACTUATORVIEW_H
