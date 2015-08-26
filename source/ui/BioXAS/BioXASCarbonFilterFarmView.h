#ifndef BIOXASCARBONFILTERFARMVIEW_H
#define BIOXASCARBONFILTERFARMVIEW_H

#include <QGridLayout>

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

	/// The upstream window control editor.
	AMExtendedControlEditor *upstreamWindowEditor_;
	/// The upstream filter thickness control editor.
	AMExtendedControlEditor *upstreamThicknessEditor_;
	/// The downstream window control editor.
	AMExtendedControlEditor *downstreamWindowEditor_;
	/// The downstream filter thickness control editor.
	AMExtendedControlEditor *downstreamThicknessEditor_;
	/// The total filter thickness control editor.
	AMExtendedControlEditor *totalThicknessEditor_;
};

#endif // BIOXASCARBONFILTERFARMVIEW_H
