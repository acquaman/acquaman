/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ACQMAN_AMDETECTOR_H
#define ACQMAN_AMDETECTOR_H

#include <QObject>
#include "dataman/AMDetectorInfo.h"
#include "AMControl.h"

class AMDetector;

/// This class acts as a proxy to emit signals for AMDetector. You can receive the connected(), etc. signals by hooking up to AMDetector::signalSource().  You should never need to create an instance of this class directly.
/*! To allow classes that implement AMDetector to also inherit QObject, AMDetector does NOT inherit QObject.  However, it still needs a way to emit signals notifying of changes to the detector, which is the role of this class.
  */
class AMDetectorSignalSource : public QObject {
	Q_OBJECT
public:
	AMDetector* detector() const { return detector_; }

protected slots:
	void emitConnected(bool isConnected);
	void emitInfoChanged();
	void emitReadingsChanged();
	void emitSettingsChanged();
	void emitDeleted();

protected:
	AMDetectorSignalSource(AMDetector* parent);
	AMDetector* detector_;
	friend class AMDetector;

signals:
	/// Indicates that the detector is ready (each detector sub class can define this however makes most sense)
	void connected(bool isConnected);
	/// Indicates that the meta-information for this detector (currently just description()) has changed.
	void infoChanged();
	void readingsChanged();
	void settingsChanged();
	/// Emitted before the data source is deleted. \c deletedSource is a pointer to the deleted source. Observers can use this to detect when AMDataSource objects no longer exist.
	/*! (In a direct signal-slot connection, the \c deletedSource will still exist, inside ~AMDataSource(), when this is called. In a queued signal-slot connection, you should assume that \c deletedSource is already deleted. */
	void deleted(void* deletedSource);
};

class AMDetector
{
public:
	AMDetector(const QString& name);

	virtual ~AMDetector();

	AMDetectorSignalSource* signalSource() const;

	bool isConnected() const;

	/// AMDetector is not a QObject, but it's children should be. To allow its for generalized GUI creation, children that are QObjects MUST implement this (likely just child->metaObject() )
	virtual const QMetaObject* getMetaObject();

	/// AMDetector sub classes need to reimplement this to return their own detectorInfo class. NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	virtual AMDetectorInfo* toInfo() const = 0;

	/// the identifying name() of a detector can sometimes be used to select one from a set of detector. Therefore, it's not really recommended to change the name after a detector is created.
	QString detectorName() const;
	/// The description() of a detector is a human-readable, free-form string.
	virtual QString description() const = 0;
	/// Descriptions can be changed at will, and the detector will emit infoChanged() when this happens.
	virtual void setDescription(const QString& description) = 0;

	//virtual bool setFromInfo(const AMDetectorInfo& info) = 0;
	virtual bool setFromInfo(const AMDetectorInfo *info) = 0;

protected:
	void setConnected(bool isConnected);

	void emitConnected(bool isConnected);
	/// This is emitted when the meta-info changes. (Right now, this only includes a detector's description() )
	void emitInfoChanged();
	void emitReadingsChanged();
	void emitSettingsChanged();

	/// identifying name for this detector
	QString name_;
	/// Human-readable description for it
	//QString description_;

private:
	/// QObject proxy for emitting signals. (This interface class can't emit directly, because it doesn't want to inherit QObject.)
	AMDetectorSignalSource* signalSource_;
	/// Internal state for connection, use setConnected(bool) to change so signals are emitted
	bool connected_;
};

#endif // AMDETECTOR_H
