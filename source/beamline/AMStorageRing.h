#ifndef AMSTORAGERING_H
#define AMSTORAGERING_H

#include <QObject>

#include "beamline/AMControl.h"

#define AMSTORAGERING_NOT_CREATED_YET 281100

/// This class encapsulates the storage ring, which is the heart of any synchrotron.  It is a singleton class that should be subclassed for specific facilities.
/*!
	Although there are a bunch of signals here, it is the subclasses responsibility to emit them.
  */
class AMStorageRing : public QObject
{
	Q_OBJECT

public:
	/// Singleton accessor.  Returns the current pointer.  If no pointer exists then it creates a new instance and then returns it.
	static AMStorageRing *storageRing();
	/// Call this to delete the storage ring instance.
	static void releaseStorageRing();

	/// Destructor.
	~AMStorageRing();

	/// Returns the value of the ring current in mA.
	double ringCurrent() const;
	/// Returns the control for the ring current.
	AMControl *ringCurrentControl() const;
	/// Returns the beam status.
	bool beamAvailable() const;

signals:
	/// Notifier that the ring current changed.  Passes the new value.
	void ringCurrentChanged(double);
	/// Notifier of the beam availablity.
	void beamAvaliability(bool);
	/// Notifier that the beam has been dumped.  Same as beamAvailability(false).
	void beamDumped();
	/// Notifier that the beam has been restored from a beam dump.  Same as beamAvailability(true).
	void beamRestored();

public slots:

protected:
	/// Constructor.  Protected due to singleton nature of class.
	explicit AMStorageRing(QObject *parent = 0);
	/// The instance for the singleton.
	static AMStorageRing *instance_;

	/// The control for the storage ring current.
	AMControl *ringCurrent_;
	/// Flag that holds whether the beam is available or not.
	bool beamAvailable_;
};

#endif // AMSTORAGERING_H
