#ifndef AMREGIONOFINTERESTABEDITOR_H
#define AMREGIONOFINTERESTABEDITOR_H

#include <QWidget>
#include <QComboBox>

#include "analysis/AMRegionOfInterestAB.h"

class AMRegionOfInterestABEditor : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMRegionOfInterestABEditor(AMRegionOfInterestAB *analysisBlock, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMRegionOfInterestABEditor();

signals:

public slots:

protected slots:
	/// Updates the data sources box.
	void updateDataSourcesBox();

protected:
	/// The region of interest analysis block.
	AMRegionOfInterestAB *analysisBlock_;

	/// The data source box.
	QComboBox *dataSourceBox_;

};

#endif // AMREGIONOFINTERESTABEDITOR_H
