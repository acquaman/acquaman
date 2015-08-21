#ifndef AMSTRIPTOOL_H
#define AMSTRIPTOOL_H

#include <QWidget>
#include <QLayout>
#include <QMenu>
#include <QInputDialog>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"

class AMDataSource;
class AMDataSourceSeriesData;

class AMStripTool : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMStripTool(QObject *parent = 0);
	/// Destructor.
	virtual ~AMStripTool();

	/// Returns the plot.
	MPlot* plot() const { return plot_; }
	/// Returns the list of controls that have been added.
	QList<AMControl*> controls() const { return controlPlotItemMap_.keys(); }

	/// Returns true if the striptool contains this control, false otherwise.
	bool contains(AMControl *control);
	/// Returns true if the striptool contains a control with the given name, false otherwise.
	bool contains(const QString &pvName);

signals:
	/// Notifier that a control has been added.
	void controlAdded(AMControl *newControl);
	/// Notifier that a control has been removed.
	void controlRemoved(AMControl *control);

public slots:
	/// Adds a control by name.
	void addPV(const QString &pvName);
	/// Removes a control by name.
	void removePV(const QString &pvName);

	/// Adds a control to the plot.
	void addControl(AMControl *control);
	/// Removes a control from the plot.
	void removeControl(AMControl *control);

protected:
	/// Creates and returns a series suitable for plotting pv updates with accompanying timestamps for the given control.
	MPlotItem* createPlotItem(AMControl *control);

protected:
	/// The plot proper.
	MPlot *plot_;
	/// The plot widget.
	MPlotWidget *plotWidget_;

	/// A map of controls to plot items.
	QMap<AMControl*, MPlotItem*> controlPlotItemMap_;
};

#endif // AMSTRIPTOOL_H
