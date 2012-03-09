#ifndef REIXSXESSCANACTION_H
#define REIXSXESSCANACTION_H

#include "actions2/actions/AMScanControllerAction.h"
#include "actions2/actions/REIXS/REIXSXESScanActionInfo.h"

/// This is a trivial subclass of AMScanControllerAction to allow registering it and REIXSXESScanActionInfo with AMActionRegistry. It enables storable, re-loadable REIXS XES Scans inside the action system.
class REIXSXESScanAction : public AMScanControllerAction
{
    Q_OBJECT
public:
	/// Constructor, as required by the registry system.
	Q_INVOKABLE REIXSXESScanAction(REIXSXESScanActionInfo* info, QObject *parent = 0) : AMScanControllerAction(info, parent) {}
	/// This convenience constructor creates the REIXSXESScanActionInfo for you.
	REIXSXESScanAction(REIXSXESScanConfiguration* scanConfig, QObject* parent = 0) : AMScanControllerAction(new REIXSXESScanActionInfo(scanConfig), parent) {}
	/// Copy Constructor
	REIXSXESScanAction(const REIXSXESScanAction& other) : AMScanControllerAction(other) {}
	/// Virtual copy constructor
	virtual AMAction* createCopy() const { return new REIXSXESScanAction(*this); }
};

#endif // REIXSXESSCANACTION_H
