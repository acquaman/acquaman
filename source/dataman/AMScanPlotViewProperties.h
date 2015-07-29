#ifndef AMSCANPLOTVIEWPROPERTIES_H
#define AMSCANPLOTVIEWPROPERTIES_H

#include <QObject>
#include <QMap>

#include "MPlot/MPlot.h"

class AMScanPlotViewProperties : public QObject
{
    Q_OBJECT

public:
	/// Constructor.
    explicit AMScanPlotViewProperties(QObject *parent = 0);
	/// Destructor.
	virtual ~AMScanPlotViewProperties();

	/// Returns the x axis units.
	QString xAxisUnits() const { return xAxisUnits_; }
	/// Returns the y axis units.
	QString yAxisUnits() const { return yAxisUnits_; }
	/// Returns the flag indicating whether the log scale is enabled.
	bool logScaleEnabled() const { return logScaleEnabled_; }
	/// Returns the flag indicating whether normalization is enabled.
	bool normalizationEnabled() const { return normalizationEnabled_; }
	/// Returns the flag indicating whether the waterfall offset is enabled.
	bool waterfallOffsetEnabled() const { return waterfallOffsetEnabled_; }
	/// Returns the value of the waterfall offset.
	double waterfallOffset() const { return waterfallOffset_; }
	/// Returns the data range constraint for the given axis.
	MPlotAxisRange dataRangeConstraint(MPlot::StandardAxis axis) const { return axisDataRangeConstraintMap_.value(axis, MPlotAxisRange()); }

signals:
	/// Notifier that the x axis units have changed.
	void xAxisUnitsChanged(const QString &newUnits);
	/// Notifier that the y axis units have changed.
	void yAxisUnitsChanged(const QString &newUnits);
	/// Notifier that the log scale enabled state has changed.
	void logScaleEnabledChanged(bool isEnabled);
	/// Notifier that the normalization enabled state has changed.
	void normalizationEnabledChanged(bool isEnabled);
	/// Notifier that the waterfall offset enabled state has changed.
	void waterfallOffsetEnabledChanged(bool isEnabled);
	/// Notifier that the waterfall offset value has changed.
	void waterfallOffsetChanged(double newOffset);
	/// Notifier that the axis data range constraint has changed.
	void axisDataRangeConstraintChanged();

public slots:
	/// Sets the x axis units.
	void setXAxisUnits(const QString &newUnits);
	/// Sets the y axis units.
	void setYAxisUnits(const QString &newUnits);
	/// Sets whether the log scale is enabled.
	void setLogScaleEnabled(bool isEnabled);
	/// Sets whether normalization is enabled.
	void setNormalizationEnabled(bool isEnabled);
	/// Sets whether the waterfall offset is enabled.
	void setWaterfallOffsetEnabled(bool isEnabled);
	/// Sets the waterfall offset value.
	void setWaterfallOffset(double newOffset);
	/// Sets the standard axis data range constraint.
	void setAxisDataRangeConstraint(MPlot::StandardAxis axis, const MPlotAxisRange &range);

protected:
	/// The x axis units.
	QString xAxisUnits_;
	/// The y axis units.
	QString yAxisUnits_;

	/// Bool indicating whether the plot log scale is enabled.
	bool logScaleEnabled_;
	/// Bool indicating whether the plot normalization is enabled.
	bool normalizationEnabled_;
	/// Bool indicating whether the plot waterfall is enabled.
	bool waterfallOffsetEnabled_;
	/// The waterfall offset.
	double waterfallOffset_;
	/// The map of axis data range constraints.
	QMap<MPlot::StandardAxis, MPlotAxisRange> axisDataRangeConstraintMap_;
};

#endif // AMSCANPLOTVIEWPROPERTIES_H
