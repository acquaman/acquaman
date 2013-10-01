#ifndef VESPERSPILATUSCCDDETECTOR_H
#define VESPERSPILATUSCCDDETECTOR_H

#include "beamline/VESPERS/VESPERSCCDDetector.h"

#include <QProcess>
/*!
	This class encapsulates the Pilatus detector on the VESPERS beamline.  For now it will only
	extend the base class.  However, since this detector has many features that we will likely
	use in the future, this class will likely be extended a lot more.
  */
class VESPERSPilatusCCDDetector : public VESPERSCCDDetector
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSPilatusCCDDetector(const QString &name, const QString &description, QObject *parent = 0);

	/// Copies the images from location \param path and copies them into "XRD Images".  \warning This might take a LONG time.  \note It might be worthwhile to make this it's own thread.
	bool copyImages(const QString &path);

	/// Returns the occupied space on aurora.
	double occupiedAuroraSize() const { return occupiedAuroraSize_; }
	/// Returns the total occupied size on aurora.
	double totalAuroraSize() const { return totalAuroraSize_; }

signals:
	/// Notifier that the detector recently updated the file system information for the capacity of the aurora drive.
	void fileSystemInfoUpdated();

protected slots:
	/// Queries the OS (linux only) for the file size of the aurora drive.  This is necessary because the images from copyImages will go on to aurora.
	void updateAuroraSize();

protected:
	/// Currently a castrated function because the Pilatus comes with good viewing software.
	virtual void loadImageFromFileImplementation(const QString &filename) { Q_UNUSED(filename); }

	/// The process that is used to call 'df'.  This is necessary so that it using the process doesn't block the event loop.
	QProcess *dfProcess_;
	/// Holds the occupied size of aurora.
	double occupiedAuroraSize_;
	/// Holds the total size of aurora.
	double totalAuroraSize_;

private slots:
	/// Method that reads out the data from the process.  This is a helper slot because we don't want to hold up the event loop.
	void readOutProcess(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // VESPERSPILATUSCCDDETECTOR_H
