#ifndef AMTESTSERIESDATAMODEL_H
#define AMTESTSERIESDATAMODEL_H

#include "util/TimeEvolutionStripTool/AMTESTDataModel.h"

#include "util/TimeEvolutionStripTool/AMTESTRingBuffer.h"
#include "util/TimeEvolutionStripTool/AMTESTSeriesData.h"

#include <QVector>

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
	/// Returns the series data model.
	virtual AMTESTSeriesData *seriesDataModel() const { return dataModel_; }

	/// Adds new values to the front of the data vector.  Old excess data is discarded.
	virtual void addData(const QVector<qreal> &newData);
	/// Clears the ring buffer.
	virtual void clear();

signals:

public slots:

protected:
	/// Ring buffer that holds all the data.
	AMTESTRingBuffer data_;
	/// Holds the series model for MPlot.
	AMTESTSeriesData *dataModel_;
};

#endif // AMTESTSERIESDATAMODEL_H
