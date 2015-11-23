#ifndef AMTESTSERIESDATAMODEL_H
#define AMTESTSERIESDATAMODEL_H

#include <QObject>

#include <QVector>

#include "util/TimeEvolutionStripTool/AMTESTDataModel.h"
#include "util/TimeEvolutionStripTool/AMTESTSeriesData.h"

/// Series data model.  Contains the data that can be visualized.
class AMTESTSeriesDataModel : public AMTESTDataModel
{
	Q_OBJECT

public:
	/// Constructor.  Buffer size is the amount of data to be visualized and update rate is the number of times data comes per second.
	explicit AMTESTSeriesDataModel(const QString &name, quint32 bufferSize, quint32 updateRate, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTESTSeriesDataModel();

	/// Returns that this is a series data model.
	virtual bool isSeriesDataModel() const { return true; }

	/// Adds new values to the front of the data vector.  Old excess data is discarded.
	virtual void addData(const QVector<qreal> &newData);

signals:

public slots:

protected:

	/// Vector that holds all the data.
	QVector<qreal> data_;
	/// Ring buffer start index for the data vector.
	int dataRingBufferStartIndex_;
	/// Holds the series model for MPlot.
	AMTESTSeriesData *dataModel_;
	/// The vector that holds the x-axis.
	QVector<qreal> xAxis_;
	/// The vector that holds the y-axis.
	QVector<qreal> yAxis_;
};

#endif // AMTESTSERIESDATAMODEL_H
