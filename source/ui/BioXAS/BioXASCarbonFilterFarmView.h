#ifndef BIOXASCARBONFILTERFARMVIEW_H
#define BIOXASCARBONFILTERFARMVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"

class AMExtendedControlEditor;

class BioXASCarbonFilterFarmView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmView(BioXASCarbonFilterFarmControl *filterFarm, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmView();

	/// Returns the carbon filter farm being viewed.
	BioXASCarbonFilterFarmControl* filterFarm() const { return filterFarm_; }

signals:
	/// Notifier that the filter farm being viewed has changed.
	void filterFarmChanged(BioXASCarbonFilterFarmControl *newFilterFarm);

public slots:
	/// Sets the carbon filter farm being viewed.
	void setFilterFarm(BioXASCarbonFilterFarmControl *newFilterFarm);

protected:
	/// The filter farm being viewed.
	BioXASCarbonFilterFarmControl *filterFarm_;

	/// The main layout.
	QVBoxLayout *layout_;
	/// The filter farm control editor.
	AMExtendedControlEditor *editor_;

};

#endif // BIOXASCARBONFILTERFARMVIEW_H
