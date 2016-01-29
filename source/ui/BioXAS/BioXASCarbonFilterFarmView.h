#ifndef BIOXASCARBONFILTERFARMVIEW_H
#define BIOXASCARBONFILTERFARMVIEW_H

#include <QWidget>

class BioXASCarbonFilterFarm;
class BioXASControlEditor;
class BioXASCarbonFilterFarmActuatorControlView;

class BioXASCarbonFilterFarmView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmView(BioXASCarbonFilterFarm *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmView();

	/// Returns the carbon filter farm control being viewed.
	BioXASCarbonFilterFarm* control() const { return filterFarm_; }

signals:
	/// Notifier that the carbon filter farm control being viewed has changed.
	void controlChanged(BioXASCarbonFilterFarm *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the carbon filter farm control being viewed.
	void setControl(BioXASCarbonFilterFarm *newControl);

protected:
	/// The carbon filter farm control being viewed.
	BioXASCarbonFilterFarm *filterFarm_;

	/// The filter editor.
	BioXASControlEditor *filterEditor_;
	/// The upstream actuator control view.
	BioXASCarbonFilterFarmActuatorControlView *upstreamActuatorView_;
	/// The downstream actuator control view.
	BioXASCarbonFilterFarmActuatorControlView *downstreamActuatorView_;
};

#endif // BIOXASCARBONFILTERFARMVIEW_H
