#ifndef SGMFASTSCANCONFIGURATION2013_H
#define SGMFASTSCANCONFIGURATION2013_H

#include "acquaman/AMFastScanConfiguration.h"
#include "SGMScanConfiguration.h"
#include "util/SGM/SGMElementInfo.h"

class SGMFastScanConfiguration2013 : public AMFastScanConfiguration, public SGMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(QString element READ element WRITE setElement)
	Q_PROPERTY(double runTime READ runTime WRITE setRunSeconds)
	Q_PROPERTY(double energyStart READ energyStart WRITE setEnergyStart)
	Q_PROPERTY(double energyMidpoint READ energyMidpoint WRITE setEnergyMidpoint)
	Q_PROPERTY(double energyEnd READ energyEnd WRITE setEnergyEnd)
	Q_PROPERTY(int velocity READ velocity WRITE setVelocity)
	Q_PROPERTY(int velocityBase READ velocityBase WRITE setVelocityBase)
	Q_PROPERTY(int acceleration READ acceleration WRITE setAcceleration)
	Q_PROPERTY(double scalerTime READ scalerTime WRITE setScalerTime)
	Q_PROPERTY(double spacingParameter READ spacingParameter WRITE setSpacingParameter)
	Q_PROPERTY(double c1Parameter READ c1Parameter WRITE setC1Parameter)
	Q_PROPERTY(double c2Parameter READ c2Parameter WRITE setC2Parameter)
	Q_PROPERTY(double sParameter READ sParameter WRITE setSParameter)
	Q_PROPERTY(double thetaParameter READ thetaParameter WRITE setThetaParameter)
	Q_PROPERTY(AMDbObject* fastScanParameters READ dbReadFastScanParameters WRITE dbLoadFastScanParameters)

	Q_CLASSINFO("AMDbObject_Attributes", "description=SGM XAS Scan Configuration")

public:
	/// Standard default constructor, sets up available detectors and periodic table elements from the SGM Beamline object
	Q_INVOKABLE explicit SGMFastScanConfiguration2013(QObject *parent = 0);
	/// Copy constrcutor
	SGMFastScanConfiguration2013(const SGMFastScanConfiguration2013 &original);
	/// Destructor
	~SGMFastScanConfiguration2013();

	/// Returns the metaObject
	const QMetaObject* getMetaObject();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return QString("%1s Fast Scan").arg(currentSettings_->runSeconds());
	}

	/// Returns the element's name from the currentSettings
	QString element() const;
	/// Returns the run time from the currentSettings
	double runTime() const;
	/// Returns the starting energy from the currentSettings
	double energyStart() const;
	/// Returns the midpoint energy from the currentSettings
	double energyMidpoint() const;
	/// Returns the final energy from the currentSettings
	double energyEnd() const;
	/// Returns the mono motor velocity from the currentSettings
	int velocity() const;
	/// Returns the mono motor base velocity from the currentSettings
	int velocityBase() const;
	/// Returns the mono motor acceleration from the currentSettings
	int acceleration() const;
	/// Returns the scaler's dwell time from the currentSettings
	double scalerTime() const;

	/// Returns the mono spacing parameter from the currentEnergyParameters
	double spacingParameter() const;
	/// Returns the mono C1 parameter from the currentEnergyParameters
	double c1Parameter() const;
	/// Returns the mono C2 parameter from the currentEnergyParameters
	double c2Parameter() const;
	/// Returns the mono S parameter from the currentEnergyParameters
	double sParameter() const;
	/// Returns the mono theta parameter from the currentEnergyParameters
	double thetaParameter() const;

	/// Returns the baseline from the currentSettings (this may not be used currently)
	int baseLine() const;

	/// Returns the undulator's start step setpoint from the currentSettings
	int undulatorStartStep() const;
	/// Returns the undulator motor velocity from the currentSettings
	int undulatorVelocity() const;
	/// Returns the number of steps the undulator needs to take from the currentSettings
	int undulatorRelativeStep() const;
	/// Returns the exit slit distance from the currentSettings
	double exitSlitDistance() const;
	/// Returns which grating should be used (low, medium, high) from the currentSettings
	int sgmGrating() const;

	/// Returns the preset string from the SGM Periodic Table (list of available fast scans)
	QStringList presets() const;
	/// Returns the SGMFastScanParameters object currently being used
	SGMFastScanParameters* currentParameters() const;
	/// Returns the SGMEnergyParameters object currently being used
	SGMEnergyParameters* currentEnergyParameters() const;

	/// Overrides the warnings string to check warnings from the fast scan parameters in the database
	virtual QString dbLoadWarnings() const;

public slots:
	/// Changes the currentSettings based on selecting one of the presets in the list
	bool setParametersFromPreset(int index);
	/// Changes the currentSettings based on an SGMFastScanParameters object pointer (sets currentSettings to point at that object and emits some signals)
	bool setParameters(SGMFastScanParameters *settings);
	/// Changes the element name in the current settings and emits the associated change signal
	bool setElement(const QString& element);
	/// Changes the run seconds in the current settings and emits the associated change signal
	bool setRunSeconds(double runSeconds);
	/// Changes the starting energy in the current settings and emits the associated change signal
	bool setEnergyStart(double energyStart);
	/// Changes the energy midpoint in the current settings and emits the associated change signal
	bool setEnergyMidpoint(double energyMidpoint);
	/// Changes the final energy in the current settings and emits the associated change signal
	bool setEnergyEnd(double energyEnd);
	/// Changes the mono motor velocity in the current settings and emits the associated change signal
	bool setVelocity(int velocity);
	/// Changes the mono motor base velocity in the current settings and emits the associated change signal
	bool setVelocityBase(int velocityBase);
	/// Changes the mono motor acceleration in the current settings and emits the associated change signal
	bool setAcceleration(int acceleration);
	/// Changes the scaler's dwell time in the current settings and emits the associated change signal
	bool setScalerTime(double scalerTime);

	/// Changes the energyParameters based on an SGMEnergyParameters object pointer (sets ener
	bool setEnergyParameters(SGMEnergyParameters *parameters);
	/// Changes the spacing parameter in the energy parameters and emits the associated change signal
	bool setSpacingParameter(double spacingParameter);
	/// Changes the C1 parameter in the energy parameters and emits the associated change signal
	bool setC1Parameter(double c1Parameter);
	/// Changes the C2 parameter in the energy parameters and emits the associated change signal
	bool setC2Parameter(double c2Parameter);
	/// Changes the S parameter in the energy parameters and emits the associated change signal
	bool setSParameter(double sParameter);
	/// Changes the theta parameter in the energy parameters and emits the associated change signal
	bool setThetaParameter(double thetaParameter);

	/// Changes the baseline in the current settings and emits the associated change signal
	bool setBaseLine(int baseLine);
	/// Changes the undulator's starting step setpoint in the current settings and emits the associated change signal
	bool setUndulatorStartStep(int undulatorStartStep);
	/// Changes the undulator motor velocity in the current settings and emits the associated change signal
	bool setUndulatorVelocity(int undulatorVelocity);
	/// Changes the number of steps the undulator needs to take in the current settings and emits the associated change signal
	bool setUndulatorRelativeStep(int undulatorRelativeStep);
	/// Changes the exit slit distance in the current settings and emits the associated change signal
	bool setExitSlitDistance(double exitSlitDistance);
	/// Changes which grating to use (low, medium, high) in the current settings and emits the associated change signal
	bool setSGMGrating(int sgmGrating);

signals:
	/// Emitted when the element name changes
	void elementChanged(const QString& element);
	/// Emitted when the run time changes
	void runSecondsChanged(double runSeconds);
	/// Emitted when the starting energy changes
	void energyStartChanged(double energyStart);
	/// Emitted when the energy midpoint changes
	void energyMidpointChanged(double energyMidpoint);
	/// Emitted when the final energy changes
	void energyEndChanged(double energyEnd);
	/// Emitted when the mono motor velocity changes
	void velocityChanged(int velocity);
	/// Emitted when the mono motor base velocity changes
	void velocityBaseChanged(int velocityBase);
	/// Emitted when the mono motor acceleration changes
	void accelerationChanged(int acceleration);
	/// Emitted when the scaler's requested dwell time changes
	void scalerTimeChanged(double scalerTime);

	/// Emitted when the spacing parameter changes
	void spacingParameterChanged(double spacingParameter);
	/// Emitted when the C1 parameter changes
	void c1ParameterChanged(double c1Parameter);
	/// Emitted when the C2 parameter changes
	void c2ParameterChanged(double c2Parameter);
	/// Emitted when the S parameter changes
	void sParameterChanged(double sParameter);
	/// Emitted when the theta parameter changes
	void thetaParameterChanged(double thetaParameter);

	/// Emitted when the baseline changes
	void baseLineChanged(int baseLine);
	/// Emitted when the undulator's starting step setpoint changes
	void undulatorStartStepChanged(int undulatorStartStep);
	/// Emitted when the undulator motor velocity changes
	void undulatorVelocityChanged(int undulatorVelocity);
	/// Emitted when the number of steps the undulator needs to take changes
	void undulatorRelativeStepChanged(int undulatorRelativeStep);
	/// Emitted when the exit slit distance changes
	void exitSlitDistanceChanged(double exitSlitDistance);
	/// Emitted when the grating to use changes
	void sgmGratingChanged(int sgmGrating);

	/// Emitted when the underlying SGMEnergyPosition for the start energy changes
	void startPositionChanged();
	/// Emitted when the underlying SGMEnergyPosition for the midpoint energy changes
	void middlePositionChanged();
	/// Emitted when the underlying SGMEnergyPosition for the end energy changes
	void endPositionChanged();
	/// Emitted when any of the fast scan settings in currentSettings change
	void fastScanSettingsChanged();
	/// Emitted when the parameters change
	void parametersChanged();

protected:
	/// Used for writing the fast scan parameters to the database
	AMDbObject* dbReadFastScanParameters();
	/// For loading the fast scan parameters from the database
	void dbLoadFastScanParameters(AMDbObject *fastScanParameters);

protected:
	/// Holds the list of predefined element and edge fast scans
	QList<SGMFastScanParameters*> settings_;
	/// Holds the information for the scan we will actually do
	SGMFastScanParameters *currentSettings_;

	/// Holds beamline information on how to calculate the energy based on the beamline settings
	SGMEnergyParameters *currentEnergyParameters_;

	/// Holds a warning regarding old saved versions of fast scans if no parameters were saved
	QString dbLoadWarnings_;
};

#endif // SGMFASTSCANCONFIGURATION2013_H
