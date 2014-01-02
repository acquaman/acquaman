#ifndef AM4DBINNINGABEDITOR_H
#define AM4DBINNINGABEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>

class MPlotWidget;
class MPlot;
class MPlotSeriesBasic;
class MPlotVectorSeriesData;
class MPlotRectangle;

class AM4DBinningAB;

class AM4DBinningABEditor : public QWidget
{
	Q_OBJECT

public:
	/// Construct an editor for the analysis block \c analysisBlock.
 	virtual ~AM4DBinningABEditor();
	explicit AM4DBinningABEditor(AM4DBinningAB *analysisBlock, QWidget *parent = 0);

signals:

public slots:
	/// signals from GUI objects: axis selector
	void onSumAxisControlChanged(int);
	/// signals from GUI objects: range minimum control
	void onRangeMinControlChanged(int);
	/// signals from GUI objects: range maximum control
	void onRangeMaxControlChanged(int);

	/// signals from analysis block: if the block's input data source changes (to either null, or one with a different size.)
	void onAnalysisBlockInputDataSourcesChanged();

protected slots:
	/// Handles setting the name for the analysis block based on the new choice of input data source.
	void onNameChoiceChanged(int index);
	/// Helper slot.  Populates the names combo box.
	void populateComboBox();
	/// Helper slot.  Handles updating the series data.
	void updateSeriesData();

protected:
	/// called to position and show/hide the range rectangle, as appropriate.
	void placeRangeRectangle();

	/// The analysis block instance we provide editing capabilities for
	AM4DBinningAB* analysisBlock_;

	/// Gui elements
	QComboBox* axisSelector_;
	QSpinBox* rangeMinControl_, *rangeMaxControl_;
	/// The combo box that contains all of the names.
	QComboBox *names_;

	/// plot widget to show what region is summed
	MPlotWidget *plotWidget_;
	MPlot *plot_;
	MPlotSeriesBasic *series_;
	MPlotVectorSeriesData *seriesData_;
	MPlotRectangle *rangeRectangle1_, *rangeRectangle2_;
};

#endif // AM4DBINNINGABEDITOR_H
