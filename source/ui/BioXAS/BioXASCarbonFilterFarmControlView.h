#ifndef BIOXASCARBONFILTERFARMCONTROLVIEW_H
#define BIOXASCARBONFILTERFARMCONTROLVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASCarbonFilterFarmControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASCarbonFilterFarmActuatorControlView.h"

class BioXASControlEditor;

class BioXASCarbonFilterFarmControlView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmControlView(BioXASCarbonFilterFarmControl *filterFarm, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmControlView();

	/// Returns the carbon filter farm control being viewed.
	BioXASCarbonFilterFarmControl* filterFarm() const { return filterFarm_; }

signals:
	/// Notifier that the carbon filter farm control being viewed has changed.
	void filterFarmChanged(AMControl *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the carbon filter farm control being viewed.
	void setFilterFarm(BioXASCarbonFilterFarmControl *newControl);

protected:
	/// The carbon filter farm control being viewed.
	BioXASCarbonFilterFarmControl *filterFarm_;

	/// The filter editor.
	BioXASControlEditor *filterEditor_;
	/// The upstream actuator control view.
	BioXASCarbonFilterFarmActuatorControlView *upstreamActuatorView_;
	/// The downstream actuator control view.
	BioXASCarbonFilterFarmActuatorControlView *downstreamActuatorView_;
};

#endif // BIOXASCARBONFILTERFARMCONTROLVIEW_H
