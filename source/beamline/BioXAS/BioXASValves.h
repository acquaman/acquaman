#ifndef BIOXASVALVES_H
#define BIOXASVALVES_H

#include "beamline/BioXAS/BioXASMasterValvesControl.h"
#include "beamline/BioXAS/BioXASFrontEndValves.h"
#include "beamline/BioXAS/BioXASSideValves.h"
#include "beamline/BioXAS/BioXASMainValves.h"
#include "beamline/BioXAS/BioXASImagingValves.h"

class BioXASValves : public BioXASMasterValvesControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASValves();

	/// Returns the front-end valves.
	BioXASFrontEndValves* frontEndValves() const { return frontEndValves_; }
	/// Returns the Side valves.
	BioXASSideValves* sideValves() const { return sideValves_; }
	/// Returns the Main valves.
	BioXASMainValves* mainValves() const { return mainValves_; }
	/// Returns the Imaging valves.
	BioXASImagingValves* imagingValves() const { return imagingValves_; }

protected:
	/// The front-end valves.
	BioXASFrontEndValves *frontEndValves_;
	/// The Side valves.
	BioXASSideValves *sideValves_;
	/// The Main valves.
	BioXASMainValves *mainValves_;
	/// The Imaging valves.
	BioXASImagingValves *imagingValves_;
};

#endif // BIOXASVALVES_H
