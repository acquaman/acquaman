#ifndef AMSADETECTOR_H
#define AMSADETECTOR_H

#include <QObject>

#include "dataman/AMNumber.h"
#include "dataman/AMMeasurementInfo.h"

/*! \todo More types: long-running acquisitions that update during acquisition; continuous acquisition
	\todo Merge with AMDetector?
  */
class AMSADetector : public QObject
{
    Q_OBJECT
public:
	enum Type { ImmediateRead, Triggered };

	explicit AMSADetector(const QString& name, const QString& description = QString(), QObject *parent = 0);
	virtual ~AMSADetector();

	virtual Type type() const = 0;

	const AMMeasurementInfo& measurementInfo() const { return measurementInfo_; }
	QString name() const { return measurementInfo_.name; }
	QString description() const { return measurementInfo_.description; }
	int rank() const { return measurementInfo_.rank(); }
	int size(int axisId) const { return measurementInfo_.size(axisId); }
	AMnDIndex size() const { return measurementInfo_.size(); }


	/// Call to prepare the detector for the beginning of a scan. Returns false immediately if the detector cannot be used now. Will emit initialized() later once complete (or failed).
	virtual bool init() = 0;

	virtual double acquisitionTime() const = 0;
	virtual bool setAcquisitionTime(double seconds) = 0;

	virtual bool isConnected() const = 0;

	virtual bool initializationFinished() const = 0;
	virtual bool initializationSucceeded() const = 0;

	virtual bool lastAcquisitionFinished() const = 0;
	virtual bool lastAcquisitionSucceeded() const = 0;

	virtual bool isAcquiring() const = 0;

	virtual bool acquireNow() = 0;
	virtual void cancelAcquisition() = 0;

	virtual AMNumber value(const AMnDIndex& index) const = 0;
	virtual const double* data() const = 0;

signals:
	void initialized(bool succeeded);
	void acquisitionFinished(bool succeeded);


signals:

public slots:
	void setName(const QString& name) { measurementInfo_.name = name; }
	void setDescription(const QString& description) { measurementInfo_.description = description; }

protected:

	AMMeasurementInfo measurementInfo_;

};

#endif // AMSADETECTOR_H
