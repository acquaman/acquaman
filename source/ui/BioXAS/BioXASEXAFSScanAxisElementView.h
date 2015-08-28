#ifndef BIOXASEXAFSSCANAXISELEMENTVIEW_H
#define BIOXASEXAFSSCANAXISELEMENTVIEW_H

#include <QWidget>

#include "dataman/AMScanAxisEXAFSRegion.h"

#include <QLabel>
#include <QDoubleSpinBox>
#include <QMap>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QToolButton>
#include <QCheckBox>

class BioXASEXAFSScanAxisElementView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASEXAFSScanAxisElementView(AMScanAxisEXAFSRegion *region, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASEXAFSScanAxisElementView();

	/// Returns the region being viewed.
	AMScanAxisEXAFSRegion* region() const { return region_; }

signals:
	/// Notifier that the region being viewed has changed.
	void regionChanged(AMScanAxisEXAFSRegion *newRegion);
	/// Notifier that the start energy (eV) has changed.
	void startValueChanged(const AMNumber &);
	/// Notifier that the end energy (eV) has changed.
	void endValueChanged(const AMNumber &);

public slots:
	/// Sets the region being viewed.
	void setRegion(AMScanAxisEXAFSRegion *newRegion);

	/// Clears the view.
	void clear();
	/// Updates the view with the latest region information.
	void update();
	/// Clears and updates the view.
	void refresh();

	/// Sets the value for the start spin box.
	void setStartSpinBox(const AMNumber &value);
	/// Sets the value for the delta spin box.
	void setDeltaSpinBox(const AMNumber &value);
	/// Sets the value for the end spin box.
	void setEndSpinBox(const AMNumber &value);
	/// Sets the value for the time spin box.
	void setTimeSpinBox(const AMNumber &value);
	/// Sets the value for the maximum time sping box.
	void setMaximumTimeSpinBox(const AMNumber &value);

protected slots:
	/// Updates the start spinbox to reflect current region information.
	void updateStartSpinBox();
	/// Updates the delta spinbox to reflect current region information.
	void updateDeltaSpinBox();
	/// Updates the end spinbox to reflect current region information.
	void updateEndSpinBox();
	/// Updates the time spinbox to reflect current region information.
	void updateTimeSpinBox();
	/// Updates the maximum time label to reflect current region information.
	void updateMaximumTimeLabel();
	/// Updates the maximum time spinbox to reflect current region information.
	void updateMaximumTimeSpinBox();
	/// Updates the tooltip, it's properties depend on current region information.
	void updateToolTip();

	/// Updates the region start with the current displayed value.
	void updateRegionStart();
	/// Updates the region step with the current displayed value.
	void updateRegionStep();
	/// Updates the region end with the current displayed value.
	void updateRegionEnd();
	/// Updates the region time with the current displayed value.
	void updateRegionTime();
	/// Updates the region maximum time with the current displayed value.
	void updateRegionMaximumTime();

protected:
	/// The region being viewed.
	AMScanAxisEXAFSRegion *region_;

	/// The spin box that holds the start position.
	QDoubleSpinBox *start_;
	/// The spin box that holds the delta position.
	QDoubleSpinBox *delta_;
	/// The spin box that holds the end position.
	QDoubleSpinBox *end_;
	/// The spin box that holds the time per point.
	QDoubleSpinBox *time_;
	/// The label prompt for the maximum time spin box.
	QLabel *maximumTimeLabel_;
	/// The spin box thhat holds the maximum time per point.
	QDoubleSpinBox *maximumTime_;
};

#endif // BIOXASEXAFSSCANAXISELEMENTVIEW_H
