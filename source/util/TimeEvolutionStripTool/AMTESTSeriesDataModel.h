#ifndef AMTESTSERIESDATAMODEL_H
#define AMTESTSERIESDATAMODEL_H

#include <QObject>

#include <QVector>

#include "util/TimeEvolutionStripTool/AMTESTDataModel.h"

#include "MPlot/MPlotSeriesData.h"

/// Series data model.  Contains the data that can be visualized.
class AMTESTSeriesDataModel : public AMTESTDataModel
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMTESTSeriesDataModel(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTESTSeriesDataModel();

	/// Returns that this is a series data model.
	virtual bool isSeriesDataModel() const { return true; }

signals:

public slots:

protected:

	/// Vector that holds all the data.
	QVector<qreal> data_;
	/// Holds the series model for MPlot.
	MPlotVectorSeriesData *dataModel_;
};

#endif // AMTESTSERIESDATAMODEL_H
