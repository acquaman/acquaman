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
	explicit BioXASCarbonFilterFarmView(BioXASCarbonFilterFarm *filterFarm, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmView();

	/// Returns the carbon filter farm being viewed.
	BioXASCarbonFilterFarm* filterFarm() const { return filterFarm_; }

signals:
	/// Notifier that the filter farm being viewed has changed.
	void filterFarmChanged(BioXASCarbonFilterFarm *newFilterFarm);

public slots:
	/// Sets the carbon filter farm being viewed.
	void setFilterFarm(BioXASCarbonFilterFarm *newFilterFarm);

protected:
	/// The filter farm being viewed.
	BioXASCarbonFilterFarm *filterFarm_;

//	/// The filter farm total thickness control editor.
//	AMExtendedControlEditor *filterEditor_;

	// For testing
//	/// The upstream actuator control editor.
//	AMExtendedControlEditor *upstreamEditor_;
	/// The upstream actuator position control editor.
	AMExtendedControlEditor *upstreamPositionEditor_;
	/// The upstream actuator position status control editor.
	AMExtendedControlEditor *upstreamPositionStatusEditor_;
//	/// The downstream actuator control editor.
//	AMExtendedControlEditor *downstreamEditor_;
	/// The downstream actuator position control editor.
	AMExtendedControlEditor *downstreamPositionEditor_;
	/// The downstream actuator position status control editor.
	AMExtendedControlEditor *downstreamPositionStatusEditor_;

};

#endif // BIOXASCARBONFILTERFARMVIEW_H
