#ifndef AMSCANCONTROLLERACTIONINFO_H
#define AMSCANCONTROLLERACTIONINFO_H

#include "actions2/AMActionInfo.h"

class AMScanConfiguration;

/// This AMActionInfo-subclass contains the minimum information required to run an AMScanControllerAction.
/*! It is recommended that you subclass it so that you have a unique AMActionInfo to register with custom views and editors for your scan configurations. (See the <b>Actions and ActionInfo</b> section of AMAction for more information.) */
class AMScanControllerActionInfo : public AMActionInfo
{
    Q_OBJECT
public:
	/// Constructor: requires an associated AMScanConfiguration \c scanConfig.
	Q_INVOKABLE AMScanControllerActionInfo(AMScanConfiguration* scanConfig = 0, QObject *parent = 0);
	/// Copy constructor. Makes a copy of \c other's scan configuration.
	AMScanControllerActionInfo(const AMScanControllerActionInfo& other);
	/// Virtual copy constructor
	virtual AMActionInfo* createCopy() const { return new AMScanControllerActionInfo(*this); }
	/// Destructor
	virtual ~AMScanControllerActionInfo();

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const;

	// New public functions
	//////////////////////////

	/// Returns true if it has a valid scanConfig()
	bool isValid() const { return scanConfig_ != 0; }
	/// Access the scan configuration:
	const AMScanConfiguration* scanConfig() const { return scanConfig_; }
	/// Access the scan configuration:
	AMScanConfiguration* scanConfig() { return scanConfig_; }

signals:

public slots:

protected:
	AMScanConfiguration* scanConfig_;
};

#endif // AMSCANCONTROLLERACTIONINFO_H
