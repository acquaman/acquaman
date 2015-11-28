#ifndef AMTESTIMAGEDATAMODEL_H
#define AMTESTIMAGEDATAMODEL_H

#include "util/TimeEvolutionStripTool/AMTESTDataModel.h"

#include "util/TimeEvolutionStripTool/AMTESTRingBuffer.h"
#include "util/TimeEvolutionStripTool/AMTESTImageData.h"

#include <QVector>

/// Image data model.  Contains the data that can be visualized as an image.
class AMTESTImageDataModel : public AMTESTDataModel
{
	Q_OBJECT

public:
	/// Constructor.
	/*!
	 * \param name is the name of the name of the data model.
	 * \param buffer size is the size of the amount of data to be visualized.
	 * \param updateRate is the interval that data comes in.
	 * \param axisSize is the length of the axis for the detector.
	 */
	explicit AMTESTImageDataModel(const QString &name, quint32 bufferSize, quint32 updateRate, quint32 axisSize, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTESTImageDataModel();

	/// Returns that this is an image data model.
	virtual bool isImageDataModel() const { return true; }
	/// Returns the image data model.
	virtual AMTESTImageData *imageDataModel() const { return dataModel_; }

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
	AMTESTImageData *dataModel_;
};

#endif // AMTESTIMAGEDATAMODEL_H
