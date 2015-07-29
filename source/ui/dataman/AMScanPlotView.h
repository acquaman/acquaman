#ifndef AMSCANPLOTVIEW_H
#define AMSCANPLOTVIEW_H

#include <QWidget>

#include "MPlot/MPlot.h"

#include "dataman/datasource/AMDataSource.h"
#include "dataman/AMScanPlotViewProperties.h"

class AMScanPlotView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit AMScanPlotView(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMScanPlotView();
	/// Returns the scan plot view properties.
	AMScanPlotViewProperties* properties() const { return properties_; }

signals:
	/// Notifier that the scan plot view properties have changed.
	void propertiesChanged(AMScanPlotViewProperties *newProperties);

public slots:
	/// Sets the scan plot view properties.
	void setProperties(AMScanPlotViewProperties *newProperties);

	/// Updates the x axis name displayed.
	void updateXAxisName();
	/// Updates the y axis name displayed.
	void updateYAxisName();
	/// Updates the log scale displayed.
	void updateLogScale();
	/// Updates the normalization displayed.
	void updateNormalization();
	/// Updates the waterfall displayed.
	void updateWaterfallOffset();
	/// Updates the axis scales' data range constraints.
	void updateDataRangeConstraints();

	/// Adds the given data source to the plot.
	void addDataSource(AMDataSource *newSource);
	/// Removes the given data source from the plot.
	void removeDataSource(AMDataSource *source);

protected:
	/// Returns a newly-created plot graphics widget.
	MPlotGW* createDefaultPlot();

protected:
	/// The plot view.
	MPlotGW *plot_;
	/// The plot view properties.
	AMScanPlotViewProperties *properties_;
};

#endif // AMSCANPLOTVIEW_H
