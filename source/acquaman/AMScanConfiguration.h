#ifndef ACQMAN_SCANCONFIGURATION_H
#define ACQMAN_SCANCONFIGURATION_H

#include <QObject>
#include "AMRegion.h"

/// An AMScanConfiguration is the abstract parent class of all scan configurations.
/*!
  The AMScanConfiguration class acts as the abstract parent class from which all scan configuration classes descend.
  Algorithmically, data acquisition has two distinct phases: configuring a scan and running a scan.
  Although the distinction may be subtle it is a key observation. This can be noted directly as a user could configure a scan that they never decided to run.
  Furthermore, programatically, there is a separation between what information needs to be collected to specify the particular scan to run and the way in which the scan is actually executed.
  That is, with proper interfacing, two different sacnning libraries could perform the same scan - and collect, essentially, the same data - by using the same scan configuration.

  Additionally, there will be many scan configuration implementation classes. Each technique, on each beamline will require a different implementation.
  For example, one might consider the SGM and REIXS beamlines, where both are capable of doing absorption spectroscopy (XAS) and emission spectroscopy (XES).
  Therefore, there will be four implementation classes: SGM's XAS Scan Configuration, SGM's XES Scan Configuration, REIXS' XAS Scan Configuration, and REIXS' XES Scan Configuration.
  There is obvious parallelism between these classes. First, there are two classes which will do XAS Scans and two classes that will do XES Scans.
  Second, there are two classes that will do scans on SGM and two classes that will do scans on REIXS.
  Clearly, this is a case for multiple inheritance; unfortunately, QObjects cannot be multiply inherited.
  The solution chosen, was to make all scan configuration implementation classes QObjects by making AMScanConfiguration inherit QObject.
  This means the SGMScanConfiguration and REIXSScanConfiguration classes are not QObjects.
  However, a particular implementation class - say SGMXASScanConfiguration - will be a QObject as it inherits AMXASScanConfiguration (which inherits AMScanConfiguration).
  This is different from the manner in which Scan Controllers are organized.
  A scan configuration needs to record configuration values but not move the controls in anyway; therefore, a copy of the beamline object or any other control is not sufficient.

  As AMScanConfiguration is the base class from which all other scan configuration classes are derived, only information common to all scan configurations is kept here.
  At this time, only the file name and the file path for where to write the data are common to all scan configurations.
  */
class AMScanConfiguration : public QObject
{
Q_OBJECT

/// Holds the name of the file where data will eventually be saved.
Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
/// Holds the path to the directory where data will eventually be saved.
Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)
/*
Q_PROPERTY(int number READ number WRITE setNumber)
Q_PROPERTY(QString sampleName READ sampleName WRITE setSampleName)
Q_PROPERTY(QString comments READ comments WRITE setComments NOTIFY commentsChanged)
Q_PROPERTY(QDateTime startTime READ startTime WRITE setStartTime)
*/

public:
	/// Constructor, takes a pointer to a QObject as its parent
	explicit AMScanConfiguration(QObject *parent = 0);

	/// Returns the name of the file to save raw data in
	QString fileName() const { return fileName_;}
	/// Returns the path to save the raw data file to
	QString filePath() const { return filePath_; }

	/*
signals:
	/// Emitted when comments string changed
	void commentsChanged(const QString &);
*/

public slots:
	/// Sets the file name
	void setFileName(const QString &fileName) { fileName_ = fileName; }
	/// Sets the file path
	void setFilePath(const QString &filePath) { filePath_ = filePath; }

protected:
	/// File name
	QString fileName_;
	/// File path
	QString filePath_;
};

#endif // ACQMAN_SCANCONFIGURATION_H
