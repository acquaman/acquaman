#ifndef REIXSXASSCANCONFIGURATION_H
#define REIXSXASSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"

/// Scan configuration for XAS scans on REIXS
class REIXSXASScanConfiguration : public AMXASScanConfiguration
{
    Q_OBJECT

	// The following properties should be moved upwards if we can figure out how to work on this together, and determine what should be stored at the AMScanConfiguration level.
	Q_PROPERTY(QString userScanName READ userScanName WRITE setUserScanName)
	Q_PROPERTY(int scanNumber READ scanNumber WRITE setScanNumber)
	Q_PROPERTY(int sampleId READ sampleId WRITE setSampleId)
	Q_PROPERTY(bool namedAutomatically READ namedAutomatically WRITE setNamedAutomatically)

	Q_CLASSINFO("AMDbObject_Attributes", "description=REIXS XAS Scan Configuration")

public:
	Q_INVOKABLE explicit REIXSXASScanConfiguration(QObject *parent = 0);
	virtual ~REIXSXASScanConfiguration();

	/// Copy constructor
	REIXSXASScanConfiguration(const REIXSXASScanConfiguration& other);
	/// Creates a fresh copy of this scan configuration.
	virtual AMScanConfiguration* createCopy() const;

	/// Creates a scan controller capable of running this configuration
	virtual AMScanController* createController();
	/// Creates a view to edit the scan configuration
	virtual AMScanConfigurationView* createView();



	/// Meta-data information to pre-set in the scan.
	int scanNumber() const { return scanNumber_; }
	/// Meta-data information to pre-set in the scan.
	int sampleId() const { return sampleId_; }
	/// True if we should generate the scan name, number, and sampleId automatically (as best we can; we'll do this based on the last sample move)
	bool namedAutomatically() const { return namedAutomatically_; }



	/// Unused, but required by AMRegionScanConfiguration
	virtual AMControlInfo regionControlInfo() const { return AMControlInfo(); }
	/// Unused, but required by AMRegionScanConfiguration
	virtual AMControlInfo timeControlInfo() const { return AMControlInfo(); }

signals:

public slots:

	void setScanNumber(int number) { scanNumber_ = number; setModified(true); }
	void setSampleId(int sampleId) { sampleId_ = sampleId; setModified(true); }
	void setNamedAutomatically(bool autoOn) { namedAutomatically_ = autoOn; setModified(true); }

protected:

	int scanNumber_;
	int sampleId_;
	bool namedAutomatically_;

};

#endif // REIXSXASSCANCONFIGURATION_H
