#ifndef BIOXASBEAMLINE_H
#define BIOXASBEAMLINE_H

#include "beamline/AMDetector.h"
#include "beamline/AMBasicControlDetectorEmulator.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "beamline/AMSlits.h"
#include "beamline/AMDetectorSet.h"

#include "beamline/CLS/CLSStorageRing.h"
#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSExclusiveStatesControl.h"
#include "beamline/CLS/CLSStandardsWheel.h"
#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSKeithley428.h"
#include "beamline/CLS/CLSTemperatureMonitor.h"
#include "beamline/CLS/CLSPressureMonitor.h"
#include "beamline/CLS/CLSFlowTransducer.h"
#include "beamline/CLS/CLSIonPump.h"
#include "beamline/CLS/CLSFlowSwitch.h"

#include "beamline/BioXAS/BioXASBeamlineDef.h"
#include "beamline/BioXAS/BioXASBeamlineSupport.h"

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
#include "beamline/BioXAS/BioXASCryostatStage.h"
#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASFrontEndShutters.h"
#include "beamline/BioXAS/BioXASFilterFlipper.h"
#include "beamline/BioXAS/BioXASZebra.h"
#include "beamline/BioXAS/BioXASFastShutter.h"
#include "beamline/BioXAS/BioXASUtilities.h"
#include "beamline/BioXAS/BioXASUtilitiesGroup.h"
#include "beamline/BioXAS/BioXASSIS3820Scaler.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASSollerSlit.h"
#include "beamline/BioXAS/BioXASCryostat.h"

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

	/// Creates and returns an action that performs a dark current measurement.
	virtual AMAction3* createDarkCurrentMeasurementAction(double dwellSeconds);
	/// Creates and returns an action that initializes the beamline before a scan.
	virtual AMAction3* createScanInitializationAction(AMGenericStepScanConfiguration *configuration);
	/// Creates and returna an action that cleans up the beamline after a scan.
	virtual AMAction3* createScanCleanupAction(AMGenericStepScanConfiguration *configuration);

	/// Returns a string representation of the beamline settings to include in the scan notes.
	virtual QString scanNotes() const;

	/// Returns the beam status.
	virtual BioXASBeamStatus* beamStatus() const { return beamStatus_; }

	/// Returns the beamline utilities.
	virtual BioXASUtilities* utilities() const { return utilities_; }

	/// Returns the shutters.
	BioXASShutters* shutters() const;
	/// Returns the valves in the beampath.
	BioXASValves* beampathValves() const;
	/// Returns the valves.
	BioXASValves* valves() const;
	/// Returns the ion pumps.
	AMBeamlineControlGroup* ionPumps() const;
	/// Returns the flow switches.
	AMBeamlineControlGroup* flowSwitches() const;
	/// Returns the pressure monitors.
	AMBeamlineControlGroup *pressureMonitors() const;
	/// Returns the temperature monitors.
	AMBeamlineControlGroup* temperatureMonitors() const;
	/// Returns the flow transducers.
	AMBeamlineControlGroup* flowTransducers() const;

	/// Returns the carbon filter farm.
	virtual BioXASCarbonFilterFarm* carbonFilterFarm() const { return 0; }
	/// Returns the m1 mirror.
	virtual BioXASM1Mirror* m1Mirror() const { return 0; }
	/// Returns the monochromator.
	virtual BioXASSSRLMonochromator* mono() const { return 0; }
	/// Returns the m2 mirror.
	virtual BioXASM2Mirror* m2Mirror() const { return 0; }
	/// Returns the SOE shutter.
	virtual CLSExclusiveStatesControl* soeShutter() const { return soeShutter_; }

	/// Returns the Be window motor.
	virtual CLSMAXvMotor* beWindow() const { return 0; }
	/// Returns the JJ slits.
	virtual AMSlits* jjSlits() const { return 0; }
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
	/// Returns the Soller slit.
	virtual BioXASSollerSlit* sollerSlit() const { return 0; }
	/// Returns the fast shutter.
	virtual BioXASFastShutter* fastShutter() const { return 0; }
	/// Returns the detector stage control.
	virtual AMControlSet* detectorStageLateralMotors() const { return detectorStageLateralMotors_; }

	/// Returns true if this beamline can use a cryostat, false otherwise.
	virtual bool canUseCryostat() const { return false; }
	/// Returns the flag indicating whether this beamline is currently using the cryostat.
	bool usingCryostat() const { return usingCryostat_; }
	/// Returns the cryostat.
	virtual BioXASCryostat* cryostat() const { return 0; }

	/// Returns the Zebra.
	virtual BioXASZebra* zebra() const { return 0; }
	/// Returns the Zebra trigger source.
	virtual AMZebraDetectorTriggerSource* zebraTriggerSource() const { return 0; }

	/// Returns the scaler.
	virtual CLSSIS3820Scaler* scaler() const { return 0; }
	/// Returns the I0 scaler channel detector.
	virtual CLSBasicScalerChannelDetector* i0Detector() const { return 0; }
	/// Returns the I1 scaler channel detector.
	virtual CLSBasicScalerChannelDetector* i1Detector() const { return 0; }
	/// Returns the I2 scaler channel detector.
	virtual CLSBasicScalerChannelDetector* i2Detector() const { return 0; }
	/// Returns true if this beamline can use a diode detector.
	virtual bool canUseDiodeDetector() const { return false; }
	/// Returns true if this beamline is using a diode detector.
	virtual bool usingDiodeDetector() const { return usingDiodeDetector_; }
	/// Returns the diode detector.
	virtual CLSBasicScalerChannelDetector* diodeDetector() const { return diodeDetector_; }
	/// Returns true if this beamline can use a PIPS detector.
	virtual bool canUsePIPSDetector() const { return false; }
	/// Returns true if this beamline is using the PIPS detector.
	virtual bool usingPIPSDetector() const { return usingPIPSDetector_; }
	/// Returns the PIPS detector.
	virtual CLSBasicScalerChannelDetector* pipsDetector() const { return pipsDetector_; }
	/// Returns true if this beamline can use a Lytle detector.
	virtual bool canUseLytleDetector() const { return false; }
	/// Returns true if this beamline is using the Lytle detector.
	virtual bool usingLytleDetector() const { return usingLytleDetector_; }
	/// Returns the Lytle detector.
	virtual CLSBasicScalerChannelDetector* lytleDetector() const { return lytleDetector_; }
	/// Returns the scaler dwell time detector.
	virtual AMDetector* scalerDwellTimeDetector() const { return scalerDwellTimeDetector_; }

	/// Returns the 32-element Germanium detector.
	virtual AMDetectorSet* ge32ElementDetectors() const { return ge32Detectors_; }
	/// Returns the four-element Vortex detector.
	virtual BioXASFourElementVortexDetector* fourElementVortexDetector() const { return 0; }
	/// Returns the elements for the given detector.
	virtual AMDetectorSet* elementsForDetector(AMDetector *detector) const { return detectorElementsMap_.value(detector, 0); }

	/// Returns the detector for the given control, if one has been created and added to the control/detector map.
	AMBasicControlDetectorEmulator* detectorForControl(AMControl *control) const;

	/// Returns the set of default detectors added to XAS scans, a subset of defaultXASScanDetectorOptions.
	AMDetectorSet* defaultXASScanDetectors() const { return defaultXASScanDetectors_; }
	/// Returns the set of detectors to use as options in XAS scans.
	AMDetectorSet* defaultXASScanDetectorOptions() const { return defaultXASScanDetectorOptions_; }

	/// Returns the set of default detectors added to generic scans, a subset of defaultGenericScanDetectorOptions.
	AMDetectorSet* defaultGenericScanDetectors() const { return defaultGenericScanDetectors_; }
	/// Returns the set of detectors to use as options in generic scans.
	AMDetectorSet* defaultGenericScanDetectorOptions() const { return defaultGenericScanDetectorOptions_; }

signals:
	/// Notifier that the current connected state has changed.
	void connectedChanged(bool isConnected);
	/// Notifier that the SOE shutter control has changed.
	void soeShutterChanged(CLSExclusiveStatesControl *newShutter);
	/// Notifier that the flag indicating whether this beamline is using the cryostat has changed.
	void usingCryostatChanged(bool usingCryostat);
	/// Notifier that the detector stage lateral motors list has changed.
	void detectorStageLateralMotorsChanged();
	/// Notifier that the 32Ge detectors have changed.
	void ge32DetectorsChanged();
	/// Notifier that the flag for whether this beamline is using the diode detector has changed.
	void usingDiodeDetectorChanged(bool usingDetector);
	/// Notifier that the diode detector has changed.
	void diodeDetectorChanged(CLSBasicScalerChannelDetector *newDetector);
	/// Notifier that the flag for whether this beamline is using the PIPS detector has changed.
	void usingPIPSDetectorChanged(bool usingDetector);
	/// Notifier that the PIPS detector has changed.
	void pipsDetectorChanged(CLSBasicScalerChannelDetector *newDetector);
	/// Notifier that the flag for whether this beamline is using the Lytle detector has changed.
	void usingLytleDetectorChanged(bool usingDetector);
	/// Notifier that the Lytle detector has changed.
	void lytleDetectorChanged(CLSBasicScalerChannelDetector *newDetector);
	/// Notifier that the scaler dwell time detector has changed.
	void scalerDwellTimeDetectorChanged(AMDetector *newDetector);

public slots:
	/// Sets whether the beamline is using the cryostat.
	void useCryostat(bool useCryostat);

	/// Adds a detector stage lateral motor.
	bool addDetectorStageLateralMotor(CLSMAXvMotor *newMotor);
	/// Removes a detector stage lateral motor.
	bool removeDetectorStageLateralMotor(CLSMAXvMotor *motor);
	/// Clears the detector stage lateral motors.
	bool clearDetectorStageLateralMotors();

	/// Adds a 32Ge detector. Returns true if successful, false otherwise.
	bool addGe32Detector(BioXAS32ElementGeDetector *newDetector);
	/// Removes a 32Ge detector. Returns true if successful, false otherwise.
	bool removeGe32Detector(BioXAS32ElementGeDetector *detector);
	/// Clears the 32Ge detectors. Returns true if successful, false otherwise.
	bool clearGe32Detectors();

	/// Adds or removes the diode detector. Returns true if successful, false otherwise.
	virtual bool useDiodeDetector(bool useDetector) { Q_UNUSED(useDetector) return false; }
	/// Adds or removes the PIPS detector. Returns true if successful, false otherwise.
	virtual bool usePIPSDetector(bool useDetector) { Q_UNUSED(useDetector) return false; }
	/// Adds or removes the Lytle detector. Returns true if successful, false otherwise.
	virtual bool useLytleDetector(bool useDetector) { Q_UNUSED(useDetector) return false; }

protected slots:
	/// Sets the cached connected state.
	void setConnected(bool isConnected);
	/// Updates the cached connected state.
	void updateConnected();

	/// Adds a shutter.
	void addShutter(AMControl *newControl, double openValue, double closedValue);
	/// Removes a shutter.
	void removeShutter(AMControl *control);
	/// Clears the shutters.
	void clearShutters();

	/// Adds a beampath valve.
	void addBeampathValve(AMControl *newControl, double openValue, double closedValue);
	/// Removes a beampath valve.
	void removeBeampathValve(AMControl *control);
	/// Clears the beampath valves.
	void clearBeampathValves();

	/// Adds a valve.
	void addValve(AMControl *newControl, double openValue, double closedValue);
	/// Removes a valve.
	void removeValve(AMControl *control);
	/// Clears the valves.
	void clearValves();

	/// Adds an ion pump.
	void addIonPump(AMBeamlineControl *newControl);
	/// Removes an ion pump.
	void removeIonPump(AMBeamlineControl *control);
	/// Clears the ion pumps.
	void clearIonPumps();

	/// Adds a flow switch.
	void addFlowSwitch(AMBeamlineControl *newControl);
	/// Removes a flow switch.
	void removeFlowSwitch(AMBeamlineControl *control);
	/// Clears the flow switches.
	void clearFlowSwitches();

	/// Adds a pressure monitor.
	void addPressureMonitor(AMBeamlineControl *newControl);
	/// Removes a pressure monitor.
	void removePressureMonitor(AMBeamlineControl *control);
	/// Clears the pressure monitors.
	void clearPressureMonitors();

	/// Adds a temperature monitor.
	void addTemperatureMonitor(AMBeamlineControl *newControl);
	/// Removes a temperature monitor.
	void removeTemperatureMonitor(AMBeamlineControl *control);
	/// Clears the temperature monitors.
	void clearTemperatureMonitors();

	/// Adds a flow transducer.
	void addFlowTransducer(AMBeamlineControl *newControl);
	/// Removes a flow transducer.
	void removeFlowTransducer(AMBeamlineControl *control);
	/// Clears the flow transducers.
	void clearFlowTransducers();

	/// Sets the SOE shutter.
	void setSOEShutter(CLSExclusiveStatesControl *shutter);

	/// Sets whether the beamline is using the cryostat.
	void setUsingCryostat(bool usingCryostat);

	/// Adds the given scaler channel detector to the appropriate detector sets.
	virtual void addExposedScalerChannelDetector(AMDetector *detector);
	/// Removes the given scaler channel detector from the appropriate detector sets.
	virtual void removeExposedScalerChannelDetector(AMDetector *detector);

	/// Sets the flag for whether we are using the diode detector.
	bool setUsingDiodeDetector(bool usingDetector);
	/// Sets the diode detector.
	bool setDiodeDetector(CLSBasicScalerChannelDetector *detector);

	/// Sets the flag for whether we are using the PIPS detector.
	bool setUsingPIPSDetector(bool usingDetector);
	/// Sets the PIPS detector.
	bool setPIPSDetector(CLSBasicScalerChannelDetector *detector);

	/// Sets the flag for whether we are using the Lytle detector.
	bool setUsingLytleDetector(bool usingDetector);
	/// Sets the Lytle detector.
	bool setLytleDetector(CLSBasicScalerChannelDetector *detector);

	/// Sets the scaler dwell time detector.
	bool setScalerDwellTimeDetector(AMDetector *detector);

	/// Adds an element to the set of elements for the given detector.
	bool addDetectorElement(AMDetector *detector, AMDetector *element);
	/// Removes an element from the set of elements for the given detector.
	bool removeDetectorElement(AMDetector *detector, AMDetector *element);
	/// Removes all elements from the set of elements for the given detector.
	bool removeDetectorElements(AMDetector *detector);
	/// Clears all elements for the given detector: removing all elements from the set, removing detector entry, and deleting the elements set.
	bool clearDetectorElements(AMDetector *detector);

	/// Adds a detector to the set of default detectors for XAS scans.
	virtual bool addDefaultXASScanDetector(AMDetector *detector);
	/// Removes a detector from the set of default detectors for XAS scans.
	virtual bool removeDefaultXASScanDetector(AMDetector *detector);
	/// Clears the set of default detectors for XAS scans.
	virtual bool clearDefaultXASScanDetectors();

	/// Adds a detector to the set of detectors used as options in XAS scans.
	virtual bool addDefaultXASScanDetectorOption(AMDetector *detector);
	/// Removes a detector from the set of detectors used as options in XAS scans.
	virtual bool removeDefaultXASScanDetectorOption(AMDetector *detector);
	/// Clears the set of default detectors for XAS scans.
	virtual bool clearDefaultXASScanDetectorOptions();

	/// Adds a detector to the set of default detectors for generic scans.
	virtual bool addDefaultGenericScanDetector(AMDetector *detector);
	/// Removes a detector from the set of default detectors for generic scans.
	virtual bool removeDefaultGenericScanDetector(AMDetector *detector);
	/// Clears the set of default detectors for generic scans.
	virtual bool clearDefaultGenericScanDetectors();

	/// Adds a detector to the set of detectors used as options in generic scans.
	virtual bool addDefaultGenericScanDetectorOption(AMDetector *detector);
	/// Removes a detector from the set of detectors used as options in generic scans.
	virtual bool removeDefaultGenericScanDetectorOption(AMDetector *detector);
	/// Clears the set of default detectors for generic scans.
	virtual bool clearDefaultGenericScanDetectorOptions();

protected:
	/// Sets up controls for front end beamline components and/or components that are common to all three BioXAS beamlines.
	virtual void setupComponents();

	/// Creates and returns a new scaler dwell time detector for the given scaler.
	virtual AMDetector* createScalerDwellTimeDetector(CLSSIS3820Scaler *scaler);

	/// Creates and returns a control detector emulator for the given control.
	AMBasicControlDetectorEmulator* createDetectorEmulator(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers = false, bool isVisible = true);
	/// Creates a control detector emulator for the given control and adds the pair to the controlDetectorMap.
	void addControlAsDetector(const QString &name, const QString &description, AMControl *control, bool hiddenFromUsers = false, bool isVisible = true);

	/// Protected constructor.
	BioXASBeamline(const QString &controlName);

protected:
	/// The current connected state.
	bool connected_;

	/// The beam status.
	BioXASBeamStatus *beamStatus_;
	/// The beamline utilities.
	BioXASUtilities* utilities_;

	/// The SOE shutter.
	CLSExclusiveStatesControl *soeShutter_;

	/// Flag indicating whether this beamline is using the cryostat.
	bool usingCryostat_;

	/// The flag for whether this beamline is using the diode detector.
	bool usingDiodeDetector_;
	/// The diode detector.
	CLSBasicScalerChannelDetector *diodeDetector_;

	/// The flag for whether this beamline is using the PIPS detector.
	bool usingPIPSDetector_;
	/// The PIPS detector.
	CLSBasicScalerChannelDetector *pipsDetector_;

	/// The flag for whether this beamline is using the Lytle detector.
	bool usingLytleDetector_;
	/// The Lytle detector.
	CLSBasicScalerChannelDetector *lytleDetector_;

	/// The scaler dwell time detector.
	AMDetector *scalerDwellTimeDetector_;

	/// The set of detector stage motors.
	AMControlSet *detectorStageLateralMotors_;

	/// The 32Ge detectors.
	AMDetectorSet *ge32Detectors_;

	/// The detector-elements mapping.
	QMap<AMDetector*, AMDetectorSet*> detectorElementsMap_;

	/// The set of detectors that are added by default to an XAS scan.
	AMDetectorSet *defaultXASScanDetectors_;
	/// The set of detector options for an XAS scan.
	AMDetectorSet *defaultXASScanDetectorOptions_;

	/// The set of detectors that are added by default to a generic scan.
	AMDetectorSet *defaultGenericScanDetectors_;
	/// The set of detector options for a generic scan.
	AMDetectorSet *defaultGenericScanDetectorOptions_;

	/// The control/detector map. Assumes a 1-1 correlation between controls and detector emulators.
	QMap<AMControl*, AMBasicControlDetectorEmulator*> controlDetectorMap_;
};

#endif // BIOXASBEAMLINE_H
