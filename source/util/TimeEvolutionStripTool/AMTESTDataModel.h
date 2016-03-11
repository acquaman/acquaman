#ifndef AMTESTDATAMODEL_H
#define AMTESTDATAMODEL_H

#include <QObject>

#include "util/TimeEvolutionStripTool/AMTESTSeriesData.h"
#include "util/TimeEvolutionStripTool/AMTESTImageData.h"

/// Basic data model.  Contains the data that can be visualized.
class AMTESTDataModel : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMTESTDataModel(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTESTDataModel();

	/// Returns whether this is a scaler data model.
	virtual bool isSeriesDataModel() const { return false; }
	/// Returns whether this is an image data model.
	virtual bool isImageDataModel() const { return false; }

	/// Returns the series data model (if isSeriesDataModel() is true).
	virtual AMTESTSeriesData *seriesDataModel() const { return 0; }
	/// Returns the image data model (if isImageDataModel() is true).
	virtual AMTESTImageData *imageDataModel() const { return 0; }

	/// Returns the name of the data model.
	QString name() const { return name_; }

	/// Adds new values to the front of the data vector.  Excess data is discarded.
	virtual void addData(const QVector<qreal> &newData) = 0;
	/// Clears the data in the buffer.  Should be called before starting a new acquisition.
	virtual void clear() = 0;

protected:
	/// The name of the data model.
	QString name_;
};

#endif // AMTESTDATAMODEL_H
