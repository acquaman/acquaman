#ifndef AM2DSUMMINGABEDITOR_H
#define AM2DSUMMINGABEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QSpinBox>


class AM2DSummingAB;

class AM2DSummingABEditor : public QWidget
{
	Q_OBJECT
public:
	/// Construct an editor for the analysis block \c analysisBlock. (Must be valid)
	AM2DSummingABEditor(AM2DSummingAB* analysisBlock, QWidget *parent = 0);

	~AM2DSummingABEditor() {
		if(additionalEditor_)
			delete additionalEditor_;
	}



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

protected:
	/// The analysis block instance we provide editing capabilities for
	AM2DSummingAB* analysisBlock_;

	/// Gui elements
	QComboBox* axisSelector_;
	QSpinBox* rangeMinControl_, *rangeMaxControl_;

	/// Expanded editor, which can opened up to provide a gui for editing the fields.
	QWidget* additionalEditor_;


};

#endif // AM2DSUMMINGABEDITOR_H
