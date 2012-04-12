#ifndef AMSCANACTION_H
#define AMSCANACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMScanActionInfo.h"

#define AMSCANACTION_CANT_CREATE_CONTROLLER 103101
#define AMSCANACTION_SCAN_CANCELLED 103102
#define AMSCANACTION_CANT_SET_CURRENT_CONTROLLER 103103
#define AMSCANACTION_CANT_INITIALIZE_CONTROLLER 103104
#define AMSCANACTION_CANT_START_CONTROLLER 103105
#define AMSCANACTION_CANT_SAVE_TO_DB 103106
#define AMSCANACTION_CANT_CREATE_EXPORT_FOLDER 103107
#define AMSCANACTION_NO_REGISTERED_EXPORTER 103108
#define AMSCANACTION_NO_REGISTERED_EXPORTER_OPTION 103109
#define AMSCANACTION_NO_DATABASE_FOR_SCAN 103110

class AMScanAction : public AMAction3
{
	Q_OBJECT

public:
	/// Constructor.  Takes an AMScanConfiguration \param config and builds a scan action around it.  This will create a scan controller which can be controlled from within the workflow.
	explicit AMScanAction(AMScanConfiguration *config, QObject *parent = 0);

	// Re-implemented public functions.
	//////////////////////////////////////////////////
	/// Pure virtual function that denotes that this action has children underneath it or not.
	virtual bool hasChildren() const { return false; }
	/// Pure virtual function that returns the number of children for this action.
	virtual int numberOfChildren() const { return 0; }

protected:
	/// The pointer holding the configuration that everything is based on.
	AMScanConfiguration *config_;
};

#endif // AMSCANACTION_H
