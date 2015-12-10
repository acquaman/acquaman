#ifndef BIOXASMASTERVALVES_H
#define BIOXASMASTERVALVES_H

#include "beamline/BioXAS/BioXASBiStateGroup.h"

class BioXASFrontEndValves;
class BioXASSideValves;
class BioXASMainValves;
class BioXASImagingValves;

class BioXASMasterValves : public BioXASBiStateGroup
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASMasterValves(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASMasterValves();

	/// Returns the front end valves.
	BioXASFrontEndValves* frontEndValves() const { return frontEndValves_; }
	/// Returns the Side beamline valves.
	BioXASSideValves* sideValves() const { return sideValves_; }
	/// Returns the Main beamline valves.
	BioXASMainValves* mainValves() const { return mainValves_; }
	/// Returns the Imaging beamline valves.
	BioXASImagingValves* imagingValves() const { return imagingValves_; }

protected:
	/// The front end valves.
	BioXASFrontEndValves *frontEndValves_;
	/// The Side beamline valves.
	BioXASSideValves *sideValves_;
	/// The Main beamline valves.
	BioXASMainValves *mainValves_;
	/// The Imaging beamline valves.
	BioXASImagingValves *imagingValves_;
};

#endif // BIOXASMASTERVALVES_H
