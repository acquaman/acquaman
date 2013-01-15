#ifndef AMDETECTOR_H
#define AMDETECTOR_H

#include <QObject>

#include "dataman/AMMeasurementInfo.h"

class AMDetector : public QObject
{
Q_OBJECT

public:
	/// Default constructor. \c name is a unique programmer name to access this detector with. \c description is a human-readable version
	AMDetector(const QString &name, const QString &description, QObject *parent = 0);

	/// Access the programmer unique name
	QString name() const { return objectName(); }
	/// Access a human-readable description
	QString description() const { return description_; }
	/// Access the describing this detector's readings (ex: "counts", "milliAmps", etc.)
	QString units() const { return units_; }

	/// Returns the number of dimensions in the output of this detector. A single point has rank 0. A spectrum output would have rank 1. An image output would have rank 2.
	virtual int rank() const { return 0; }
	/// Returns the size (ie: number of elements) along each dimension of the detector.  For a single-point detector, returns an empty AMnDIndex(). For a spectrum output, this would contain one number (the number of pixels or points along the axis).  For an image output, this would contain the width and height.
	virtual AMnDIndex size() const { return AMnDIndex(); }
	/// Returns a list of AMAxisInfo describing the size and nature of each detector axis, in order.
	virtual QList<AMAxisInfo>  axes() const { return QList<AMAxisInfo>(); }

	/// Returns (or casts) this AMDetector as an AMMeasurementInfo, which contains the bare-bones dimensional information.
	operator AMMeasurementInfo();

	/// Returns the current connected state (whether the detector has a connection to its constituent elements)
	bool isConnected() const { return connected_; }
	/// Returns the current ready for acquisition state (whether the detector can acquire data right now)
	bool isReadyForAcquisition() const { return readyForAcquisition_; }

signals:
	/// Indicates that the detector's constituent elements are connected (each detector sub class can define this however makes most sense)
	void connected(bool isConnected);
	/// Indicates that the detector is ready to acquire data (each detector sub class can define this however makes most sense)
	void readyForAcquisition(bool isReadyForAcquisition);

protected:
	/// Internal class trigger for setting the connected state. All AMDetector subclasses should call this to make sure that signals are emitted properly.
	void setConnected(bool isConnected);
	/// Internal class trigger for setting the ready for acquisition state. All AMDetector subclasses should call this to make sure that signals are emitted properly.
	void setReadyForAcquisition(bool isReadyForAcquisition);

protected:
	/// A human-readable description
	QString description_;
	/// Units describing this detector's readings (ex: "counts", "milliAmps", etc.)
	QString units_;

private:
	/// Internal state for connection, which referes to the constituent elements of the detector (likely AMControls) all being connected. Use setConnected(bool) to change so signals are emitted.
	bool connected_;
	/// Internal state for ready for acquisition, which refers to the detector being ready to acquire data at the moment. Use setReadyForAcquisition(bool) to change so signals are emitted.
	bool readyForAcquisition_;
};

#endif // AMDETECTOR_H
