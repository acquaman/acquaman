#ifndef BIOXASBEAMLINE_H
#define BIOXASBEAMLINE_H

#include "beamline/AMDetector.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/AMMotorGroup.h"

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"
#include "beamline/CLS/CLSStandardsWheel.h"
#include "beamline/CLS/CLSJJSlits.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSKeithley428.h"

#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"
#include "beamline/BioXAS/BioXASCarbonFilterFarm.h"
#include "beamline/BioXAS/BioXASXIAFilters.h"
#include "beamline/BioXAS/BioXASM1Mirror.h"
#include "beamline/BioXAS/BioXASM1MirrorMask.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMask.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "beamline/BioXAS/BioXASM2Mirror.h"
#include "beamline/BioXAS/BioXASDBHRMirrors.h"
#include "beamline/BioXAS/BioXASEndstationTable.h"
#include "beamline/BioXAS/BioXAS32ElementGeDetector.h"
#include "beamline/BioXAS/BioXASFourElementVortexDetector.h"
#include "beamline/BioXAS/BioXASBeamlineUtilities.h"
#include "beamline/BioXAS/BioXASCryostatStage.h"
#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/BioXAS/BioXASFilterFlipper.h"
#include "beamline/BioXAS/BioXASZebra.h"
#include "beamline/BioXAS/BioXASUtilities.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

#define BIOXASBEAMLINE_VALVE_OPEN 1
#define BIOXASBEAMLINE_VALVE_CLOSED 4
#define BIOXASBEAMLINE_FASTVALVE_OPEN 1
#define BIOXASBEAMLINE_FASTVALVE_CLOSED 4

class BioXASBeamline : public CLSBeamline
{
    Q_OBJECT

public:
	/// Singleton accessor.
	static BioXASBeamline *bioXAS()
	{
		if (!instance_) {
			instance_ = new BioXASBeamline("BioXAS Beamline");
			instance_->initializeBeamlineSupport();
		}

		return static_cast<BioXASBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~BioXASBeamline();

	/// Returns the current connected state.
	virtual bool isConnected() const;
	/// Returns the (cached) current connected state.
	virtual bool connected() const { return connected_; }

	/// Returns the beamline utilities.
	virtual BioXASUtilities* utilities() const { return utilities_; }

	/// Returns the ion pumps.
	BioXASIonPumps* ionPumps() const;
	/// Returns the valves in the beampath.
	BioXASValves* beampathValves() const;
	/// Returns the valves.
	BioXASValves* valves() const;

	/// Returns the front-end shutters.
	BioXASFrontEndShutters* frontEndShutters() const { return frontEndShutters_; }
	/// Returns the shutters.
	virtual BioXASShuttersGroup* shutters() const { return frontEndShutters_; }

	/// Returns the carbon filter farm.
	virtual BioXASCarbonFilterFarm* carbonFilterFarm() const { return 0; }
	/// Returns the m1 mirror.
	virtual BioXASM1Mirror* m1Mirror() const { return 0; }
	/// Returns the monochromator.
	virtual BioXASSSRLMonochromator* mono() const { return 0; }
	/// Returns the m2 mirror.
	virtual BioXASM2Mirror* m2Mirror() const { return 0; }

	/// Returns the beam status.
	virtual BioXASBeamStatus* beamStatus() const { return 0; }

	/// Returns the Be window motor.
	virtual CLSMAXvMotor* beWindow() const { return 0; }
	/// Returns the JJ slits.
	virtual CLSJJSlits* jjSlits() const { return 0; }
	/// Returns the XIA filters.
	virtual BioXASXIAFilters* xiaFilters() const { return 0; }
	/// Returns the DBHR mirrors.
	virtual BioXASDBHRMirrors* dbhrMirrors() const { return 0; }
	/// Returns the standards wheel.
	virtual CLSStandardsWheel* standardsWheel() const { return 0; }
	/// Returns the cryostat stage motor group.
	virtual BioXASCryostatStage* cryostatStage() const { return 0; }
	/// Returns the endstation table.
	virtual BioXASEndstationTable* endstationTable() const { return 0; }
	/// Returns the filter flipper.
	virtual BioXASFilterFlipper* filterFlipper() const { return 0; }

	/// Returns the Zebra.
	virtual BioXASZebra* zebra() const { return 0; }
	/// Returns the Zebra trigger source.
	virtual AMZebraDetectorTriggerSource* zebraTriggerSource() const { return 0; }

	/// Returns the scaler.
	virtual CLSSIS3820Scaler* scaler() const { return 0; }

	/// Returns the I0 scaler channel detector.
	virtual AMDetector* i0Detector() const { return 0; }
	/// Returns the I1 scaler channel detector.
	virtual AMDetector* i1Detector() const { return 0; }
	/// Returns the I2 scaler channel detector.
	virtual AMDetector* i2Detector() const { return 0; }
	/// Returns the 32-element Germanium detector.
	virtual BioXAS32ElementGeDetector* ge32ElementDetector() const { return 0; }
	/// Returns the four-element Vortex detector.
	virtual BioXASFourElementVortexDetector* fourElementVortexDetector() const { return 0; }
	/// Returns the scaler dwell time detector.
	virtual AMBasicControlDetectorEmulator* scalerDwellTimeDetector() const { return 0; }

	/// Returns the detector for the given control, if one has been created and added to the control/detector map.
	AMBasicControlDetectorEmulator* detectorForControl(AMControl *control) const;

signals:
	/// Notifier that the current connected state has changed.
	void connectedChanged(bool isConnected);

protected slots:
	/// Sets the cached connected state.
	void setConnected(bool isConnected);
	/// Updates the cached connected state.
	void updateConnected();

protected:
	/// Sets up controls for front end beamline components and/or components that are common to all three BioXAS beamlines.
	virtual void setupComponents();

	/// Creates and returns a control detector emulator for the given control.
	AMBasicControlDetectorEmulator* createDetectorEmulator(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers = false, bool isVisible = true);
	/// Creates a control detector emulator for the given control and adds the pair to the controlDetectorMap.
	void addControlAsDetector(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers = false, bool isVisible = true);

	/// Protected constructor.
	BioXASBeamline(const QString &controlName);

protected:
	/// The current connected state.
	bool connected_;

	/// The beamline utilities.
	BioXASUtilities* utilities_;
	/// The front end shutters.
	BioXASFrontEndShutters *frontEndShutters_;

	/// The control/detector map. Assumes a 1-1 correlation between controls and detector emulators.
	QMap<AMControl*, AMBasicControlDetectorEmulator*> controlDetectorMap_;
};

#endif // BIOXASBEAMLINE_H
