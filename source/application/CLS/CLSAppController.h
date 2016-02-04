#ifndef CLSAPPCONTROLLER_H
#define CLSAPPCONTROLLER_H

#include "application/AMAppController.h"

class CLSAppController : public AMAppController
{
    Q_OBJECT

public:
	enum CLSBeamlineID
	{
		OtherFacility = 1,    //reserved for AMAppControlelr
		PresevedBeamline = 2, //reserved for AMAppControlelr
		SGMBeamlineId = 3,
		VESPERSBeamlineId = 4,
		REIXSBeamlineId = 5,
		IDEASBeamlineId = 6,
		BioXASSideBeamlineId = 7,
		BioXASMainBeamlineId = 8,
		BioXASImagingBeamlineId = 9,
		SXRMBBeamlineId = 10
		// new beamlines need to be added here ...
	};

	explicit CLSAppController(CLSAppController::CLSBeamlineID facilityId, QObject *parent = 0);
	virtual ~CLSAppController();

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

public slots:
	/// populate the data for some tables (AMFacility, AMRun) of the user database
	virtual bool startupPopulateUserDBTable(AMDatabase* userDb); ///< Run on first time only

protected:
	/// returns the beamline name of a given beamline Id
	QString beamlineName(CLSAppController::CLSBeamlineID beamline);

protected:
	CLSAppController::CLSBeamlineID facilityId_;
};

#endif // CLSAPPCONTROLLER_H
