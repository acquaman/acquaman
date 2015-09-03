#ifndef BIOXASXASSCANCONFIGURATION_H
#define BIOXASXASSCANCONFIGURATION_H

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "acquaman/BioXAS/BioXASScanConfiguration.h"

class AMScanAxisEXAFSRegion;
class AMElement;
class AMAbsorptionEdge;

class BioXASXASScanConfiguration : public AMGenericStepScanConfiguration, public BioXASScanConfiguration
{
    Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=BioXAS XAS Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE BioXASXASScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	BioXASXASScanConfiguration(const BioXASXASScanConfiguration &original);
	/// Destructor.
	virtual ~BioXASXASScanConfiguration();

	/// Returns a description of the scan technique.
	virtual QString technique() const { return "XAS"; }
	/// Returns a description of this scan configuration.
	virtual QString description() const { return "XAS Scan"; }
	/// Returns a detailed description of this scan configuration.
	virtual QString detailedDescription() const { return "BioXAS XAS Scan"; }
	/// Returns the standard information for an XAS scan.  Used when exporting.
	virtual QString headerText() const;

	/// Returns a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;
	/// Returns a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();
	/// Returns a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

public slots:
	/// Adds the scanned control.
	virtual void setControl(AMControlInfo newInfo);
	/// Removes the scanned control.
	virtual void removeControl();

	/// Clears all regions.
	void clearRegions();
	/// Sets up the default XANES regions.
	void setupDefaultXANESRegions();
	/// Sets up the default EXAFS regions.
	void setupDefaultEXAFSRegions();

protected slots:
	/// Inserts the given region into the given scan axis index position, and makes the appropriate connections.
	void addRegion(int scanAxisIndex, int regionIndex, AMScanAxisRegion *region);
	/// Sets up connections to the given region's signals.
	void connectRegion(AMScanAxisRegion *region);
	/// Removes the given region from the scan axis at the given index position, and breaks the appropriate connections.
	void removeRegion(int scanAxisIndex, AMScanAxisRegion *region);
	/// Removes connections to the given region's signals.
	void disconnectRegion(AMScanAxisRegion *region);

protected:
	/// Creates and returns a default XANES region.
	AMScanAxisEXAFSRegion* createDefaultXANESRegion(double edgeEnergy);
	/// Creates and returns a XANES region.
	AMScanAxisEXAFSRegion* createXANESRegion(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime) const;
	/// Creates and returns an EXAFS region.
	AMScanAxisEXAFSRegion* createEXAFSRegion(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime) const;
	/// Creates and returns an EXAFS region with some values set for k space.
	AMScanAxisEXAFSRegion* createEXAFSRegionInKSpace(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime, double maximumTime) const;
};

#endif // BIOXASXASSCANCONFIGURATION_H
