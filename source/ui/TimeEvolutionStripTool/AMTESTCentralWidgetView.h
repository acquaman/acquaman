#ifndef AMTESTCENTRALWIDGETVIEW_H
#define AMTESTCENTRALWIDGETVIEW_H

#include <QWidget>

#include <QPushButton>
#include <QDoubleSpinBox>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"

class AMTESTDataModelListView;

/// This is the central widget for the time evolution strip tool.
class AMTESTCentralWidgetView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMTESTCentralWidgetView(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMTESTCentralWidgetView();

signals:

public slots:
	/// Handles starting up the data models and continuous data requests.
	void startAcquisition();
	/// Handles stopping the continuous data request.
	void stopAcquisition();

protected slots:
	/// Handles updating the accessibility widgets based on the current state of the server connectivity.
	void updateWidgetAppearance();
	/// Handles adding MPlot items to the plot.
	void onDataModelToBeAdded(const QString &name);
	/// Handles removing MPlot items from the plot.
	void onDataModelToBeRemoved(const QString &name);
	/// Handles cleaning up all MPlot items when a server is disconnected.  Takes a QStringList of all item names that need to be removed.
	void onDataModelsDeleted(const QStringList &dataModelNames);

protected:
	/// Creates a plot for viewing the sources.
	void setupPlot();
	/// Helper method for having different colors to lines.
	QColor nextColor() const;

	/// Holds the plot widget.
	MPlotWidget *plotWidget_;
	/// Holds the plot.
	MPlot *plot_;
	/// Widget that holds all the check boxes for the data models.
	AMTESTDataModelListView *dataModelListView_;
	/// Double spin box that holds the update interval for the data server continuous requests.
	QDoubleSpinBox *timeIntervalSpinBox_;
	/// The start button.
	QPushButton *startButton_;
	/// The stop button.
	QPushButton *stopButton_;
	/// Holds the name of the current image.
	QString imageDataModelName_;
};

#endif // AMTESTCENTRALWIDGETVIEW_H
