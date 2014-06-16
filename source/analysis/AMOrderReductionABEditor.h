#ifndef AMORDERREDUCTIONABEDITOR_H
#define AMORDERREDUCTIONABEDITOR_H

#include <QWidget>
#include <QComboBox>

#include "analysis/AMOrderReductionAB.h"

///	This provides a basic view to change the reduced axis as well as switch between other data sources.
class AMOrderReductionABEditor : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMOrderReductionAB pointer.
 	virtual ~AMOrderReductionABEditor();
	explicit AMOrderReductionABEditor(AMOrderReductionAB *analysisBlock, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles changing all the necessary things when the data source is changed.
	void onDataSourceChanged(int index);
	/// Handles changing the reduced axis.
	void onReducedAxisChanged(int index);
	/// Helper slot that populates the combo boxes.
	void populateComboBox();

protected:
	/// Pointer to the order reduction analysis block.
	AMOrderReductionAB *analysisBlock_;

	/// The combo box that contains all the data source names.  Used for choosing which data source to use.
	QComboBox *sourceNames_;
	/// The combo box that contains the reduced axis options.
	QComboBox *reducedAxisOptions_;
};

#endif // AMORDERREDUCTIONABEDITOR_H
