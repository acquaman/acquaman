/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AM_AMOLDDETECTOR_H
#define AM_AMOLDDETECTOR_H

#include <QObject>
#include "dataman/info/AMOldDetectorInfo.h"
#include "AMControl.h"

class AMOldDetector;

/// This class acts as a proxy to emit signals for AMDetector. You can receive the connected(), etc. signals by hooking up to AMDetector::signalSource().  You should never need to create an instance of this class directly.
/*! To allow classes that implement AMDetector to also inherit QObject, AMDetector does NOT inherit QObject.  However, it still needs a way to emit signals notifying of changes to the detector, which is the role of this class.
  */
class AMDetectorSignalSource : public QObject {
	Q_OBJECT
public:
	AMOldDetector* detector() const { return detector_; }

protected slots:
	void emitConnected(bool isConnected);
	void emitInfoChanged();
	void emitReadingsChanged();
	void emitSettingsChanged();
	void emitDeleted();

protected:
 	virtual ~AMDetectorSignalSource();
	AMDetectorSignalSource(AMOldDetector* parent);
	AMOldDetector* detector_;
	friend class AMOldDetector;

signals:
	/// Indicates that the detector is ready (each detector sub class can define this however makes most sense)
	void connected(bool isConnected);
	void availabilityChagned(AMOldDetector *detector, bool isAvailable);
	/// Indicates that the meta-information for this detector (currently just description()) has changed.
	void infoChanged();
	void readingsChanged();
	void settingsChanged();
	/// Emitted before the data source is deleted. \c deletedSource is a pointer to the deleted source. Observers can use this to detect when AMDataSource objects no longer exist.
	/*! (In a direct signal-slot connection, the \c deletedSource will still exist, inside ~AMDataSource(), when this is called. In a queued signal-slot connection, you should assume that \c deletedSource is already deleted. */
	void deleted(void* deletedSource);
};

class AMOldDetector
{
public:
	/// This enum type is used to describe the way a detector reports its readings.
	/*! Possible explanation codes are:
	*/
	enum ReadMethod {
		ImmediateRead = 1,	///< Just reports the current value (cached) for the readings (call value() for an AMControl)
		RequestRead,		///< Should request a new value before reporting (probably need to set a trigger AMControl)
		WaitRead		///< Should wait for a new value before reporting (connect to valueChanged() for an AMControl)
	};

	AMOldDetector(const QString& name, AMOldDetector::ReadMethod readMethod = AMOldDetector::ImmediateRead);

	virtual ~AMOldDetector();

	AMDetectorSignalSource* signalSource() const;

	bool isConnected() const;

	/// AMDetector is not a QObject, but it's children should be. To allow its for generalized GUI creation, children that are QObjects MUST implement this (likely just child->metaObject() )
	virtual const QMetaObject* getMetaObject();

	/// The read method this detector is using (see AMDetector::ReadMethod enum)
	virtual AMOldDetector::ReadMethod readMethod() const;

	/// Returns the PV name the dacq library wants to use
	virtual QString dacqName() const;

	/// Returns a string list of dacq "Action Begin" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
	virtual QStringList dacqBegin() const;
	/// Returns a string list of dacq "Action Move" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
	virtual QStringList dacqMove() const;
	/// Returns a string list of dacq "Action Dwell" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
	virtual QStringList dacqDwell() const;
	/// Returns a string list of dacq "Action Finish" statements. The strings should be divided into three sections by ||=||. Three sections are the command (SetPV, WaitPV, etc), the PV, and the value.
	virtual QStringList dacqFinish() const;

	/// Get the current reading
	virtual double reading() const;

	/// AMDetector sub classes need to reimplement this to return their own detectorInfo class. NEEDS TO RETURN A NEW INSTANCE, CALLER IS RESPONSIBLE FOR MEMORY.
	virtual AMOldDetectorInfo* toInfo() const = 0;

	/// the identifying name() of a detector can sometimes be used to select one from a set of detector. Therefore, it's not really recommended to change the name after a detector is created.
	QString detectorName() const;

	/// The description() of a detector is a human-readable, free-form string.
	virtual QString description() const = 0;

	/// Descriptions can be changed at will, and the detector will emit infoChanged() when this happens.
	virtual void setDescription(const QString& description) = 0;

	/// Read Method can be changed if necessary (see AMDetector::ReadMethod enum)
	virtual void setReadMethod(AMOldDetector::ReadMethod readMethod);

	/* NTBA March 14, 2011 David Chevrier
	   Should have something like this
	virtual bool setFromInfo(const AMDetectorInfo& info) = 0;
	*/
	virtual bool setFromInfo(const AMOldDetectorInfo *info) = 0;

	/// Should be implemented such that a turnOnAction is successful. That is, if a detector needs its high voltage set to a given value, then activate performs that operation. The turnOnAction should only "flip the switch" for the detector's high voltage.
	virtual bool activate() { return false;}

protected:
	void setConnected(bool isConnected);
	void setTimedOut();

	void emitConnected(bool isConnected);
	/// This is emitted when the meta-info changes. (Right now, this only includes a detector's description() )
	void emitInfoChanged();
	void emitReadingsChanged();
	void emitSettingsChanged();

protected:
	/// identifying name for this detector
	QString name_;

	/// Read Method for this detector (see readMethod enum)
	AMOldDetector::ReadMethod readMethod_;

private:
	/// QObject proxy for emitting signals. (This interface class can't emit directly, because it doesn't want to inherit QObject.)
	AMDetectorSignalSource* signalSource_;
	/// Internal state for connection, use setConnected(bool) to change so signals are emitted
	bool connected_;
};

#endif // AMDETECTOR_H
