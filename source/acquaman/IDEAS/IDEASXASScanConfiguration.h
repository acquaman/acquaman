#ifndef IDEASXASSCANCONFIGURATION_H
#define IDEASXASSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"

class IDEASXASScanConfiguration : public AMXASScanConfiguration
{
Q_OBJECT

    	Q_PROPERTY(QString I0Channel READ I0Channel WRITE setI0Channel)
	Q_PROPERTY(QString ItChannel READ ItChannel WRITE setItChannel)
	Q_PROPERTY(QString IrChannel READ IrChannel WRITE setIrChannel)

	Q_PROPERTY(bool isXRFScan READ isXRFScan WRITE setIsXRFScan)
	Q_PROPERTY(bool isTransScan READ isTransScan WRITE setIsTransScan)


Q_CLASSINFO("AMDbObject_Attributes", "description=IDEAS XAS Scan Configuration")

public:
	/// Constructor
	Q_INVOKABLE IDEASXASScanConfiguration(QObject *parent = 0);
	/// Copy Constructor
	IDEASXASScanConfiguration(const IDEASXASScanConfiguration &original);
	virtual ~IDEASXASScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the AMControlInfo for the scanned region control.
	AMControlInfo regionControlInfo() const { return regions_->defaultControl()->toInfo(); }
	/// Returns the AMControlInfo for the time control.
	AMControlInfo timeControlInfo() const { return regions_->defaultTimeControl()->toInfo(); }

	// Scan configuration properties
	/////////////////////////

	QString I0Channel() const { return I0Channel_; }
	QString ItChannel() const { return ItChannel_; }
	QString IrChannel() const { return IrChannel_; }
	bool isXRFScan() const { return isXRFScan_; }
	bool isTransScan() const { return isTransScan_; }


public slots:

	void setIsXRFScan(bool isXRFScan) { if(isXRFScan == isXRFScan_) return; isXRFScan_ = isXRFScan; setModified(true); emit configurationChanged(); }
	void setIsTransScan(bool isTransScan) { if(isTransScan == isTransScan_) return; isTransScan_ = isTransScan; setModified(true); emit configurationChanged(); }
	void setI0Channel(QString I0Channel) { if(I0Channel == I0Channel_) return; I0Channel_ = I0Channel; setModified(true); emit configurationChanged(); }
	void setItChannel(QString ItChannel) { if(ItChannel == ItChannel_) return; ItChannel_ = ItChannel; setModified(true); emit configurationChanged(); }
	void setIrChannel(QString IrChannel) { if(IrChannel == IrChannel_) return; IrChannel_ = IrChannel; setModified(true); emit configurationChanged(); }




protected:

	QString I0Channel_;
	QString ItChannel_;
	QString IrChannel_;
	bool isXRFScan_;
	bool isTransScan_;





};

#endif // IDEASXASSCANCONFIGURATION_H
