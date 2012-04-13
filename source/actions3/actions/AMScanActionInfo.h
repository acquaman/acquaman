#ifndef AMSCANACTIONINFO_H
#define AMSCANACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "acquaman/AMScanConfiguration.h"


class AMScanActionInfo : public AMActionInfo3
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMScanConfiguration \param config and builds a scan action around it.  This will create a scan controller which can be controlled from within the workflow.
	Q_INVOKABLE AMScanActionInfo(AMScanConfiguration *config, const QString& iconFileName = ":/spectrum.png", QObject *parent = 0);
	/// Copy constructor.  Makes a copy of \param other's scan configuration.
	AMScanActionInfo(const AMScanActionInfo &other);
	/// Destructor.
	virtual ~AMScanActionInfo();

	/// Creates a copy of this action info.
	virtual AMActionInfo3 *createCopy() const { return new AMScanActionInfo(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Scan Action"; }

	// New public functions
	//////////////////////////

	/// Returns true if it has a valid config()
	bool isValid() const { return config_ != 0; }
	/// Access the scan configuration.  Returns a constant reference.
	const AMScanConfiguration* config() const { return config_; }
	/// Access the scan configuration.  Returns a modifiable reference.
	AMScanConfiguration* config() { return config_; }

protected:
	/// The pointer holding the configuration that everything is based on.
	AMScanConfiguration *config_;
};

#endif // AMSCANACTIONINFO_H
