#ifndef SGMFASTSCANCONFIGURATION_H
#define SGMFASTSCANCONFIGURATION_H

#include "acquaman/AMFastScanConfiguration.h"
#include "SGMScanConfiguration.h"
#include "util/SGM/SGMElementInfo.h"

//class SGMFastScanParameters;

class SGMFastScanConfiguration : public AMFastScanConfiguration, public SGMScanConfiguration
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
	Q_PROPERTY(AMDbObject* detectorConfigs READ dbReadDetectorConfigs WRITE dbLoadDetectorConfigs)

	Q_CLASSINFO("AMDbObject_Attributes", "description=SGM XAS Scan Configuration")

public:
	Q_INVOKABLE explicit SGMFastScanConfiguration(QObject *parent = 0);
	SGMFastScanConfiguration(const SGMFastScanConfiguration &original);
	~SGMFastScanConfiguration();

	/* NTBA March 14, 2011 David Chevrier
	   Needs to be updated to match style of SGMXASScanConfiguration
	   */
	/// Returns an AMDetectorSet that consists of the detectors a user can choose (or choose not) to use. In this case TEY and TFY
	AMDetectorSet* detectorChoices() const { return fastDetectors_; }
	/// Returns an AMDetectorSet that consists of all the detectors this scan can/will use (adds detectors that are always collected to the detectorChoices(), such as I0, photodiode, and energy feedback)
	AMDetectorSet* allDetectors() const { return allDetectors_; }
	/// Returns the current configuration requested for the user selectable detectors
	AMDetectorInfoSet detectorChoiceConfigurations() const { return fastDetectorsConfigurations_; }
	/// Returns the current configuration requested for all of the detectors
	AMDetectorInfoSet allDetectorConfigurations() const;


	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return QString("%1s Fast Scan").arg(currentSettings_->runSeconds());
	}

	QString element() const;
	double runTime() const;
	double energyStart() const;
	double energyMidpoint() const;
	double energyEnd() const;
	int velocity() const;
	int velocityBase() const;
	int acceleration() const;
	double scalerTime() const;

	double spacingParameter() const;
	double c1Parameter() const;
	double c2Parameter() const;
	double sParameter() const;
	double thetaParameter() const;

	int baseLine() const;

	int undulatorStartStep() const;
	int undulatorVelocity() const;
	int undulatorRelativeStep() const;
	double exitSlitDistance() const;

	QStringList presets() const;
	SGMFastScanParameters* currentParameters() const;
	SGMEnergyParameters* currentEnergyParameters() const;

public slots:
	bool setParametersFromPreset(int index);
	bool setParameters(SGMFastScanParameters *settings);
	bool setElement(const QString& element);
	bool setRunSeconds(double runSeconds);
	bool setEnergyStart(double energyStart);
	bool setEnergyMidpoint(double energyMidpoint);
	bool setEnergyEnd(double energyEnd);
	bool setVelocity(int velocity);
	bool setVelocityBase(int velocityBase);
	bool setAcceleration(int acceleration);
	bool setScalerTime(double scalerTime);

	bool setEnergyParameters(SGMEnergyParameters *parameters);
	bool setSpacingParameter(double spacingParameter);
	bool setC1Parameter(double c1Parameter);
	bool setC2Parameter(double c2Parameter);
	bool setSParameter(double sParameter);
	bool setThetaParameter(double thetaParameter);

	bool setBaseLine(int baseLine);
	bool setUndulatorStartStep(int undulatorStartStep);
	bool setUndulatorVelocity(int undulatorVelocity);
	bool setUndulatorRelativeStep(int undulatorRelativeStep);
	bool setExitSlitDistance(double exitSlitDistance);

	bool setDetectorConfigurations(AMDetectorInfoSet detectorConfigurations);

signals:
	void onElementChanged(const QString& element);
	void onRunSecondsChanged(double runSeconds);
	void onEnergyStartChanged(double energyStart);
	void onEnergyMidpointChanged(double energyMidpoint);
	void onEnergyEndChanged(double energyEnd);
	void onVelocityChanged(int velocity);
	void onVelocityBaseChanged(int velocityBase);
	void onAccelerationChanged(int acceleration);
	void onScalerTimeChanged(double scalerTime);

	void onSpacingParameterChanged(double spacingParameter);
	void onC1ParameterChanged(double c1Parameter);
	void onC2ParameterChanged(double c2Parameter);
	void onSParameterChanged(double sParameter);
	void onThetaParameterChanged(double thetaParameter);

	void onBaseLineChanged(int baseLine);
	void undulatorStartStepChanged(int undulatorStartStep);
	void undulatorVelocityChanged(int undulatorVelocity);
	void undulatorRelativeStepChanged(int undulatorRelativeStep);
	void exitSlitDistanceChanged(double exitSlitDistance);

protected:
	AMDbObject* dbReadDetectorConfigs() { return &fastDetectorsConfigurations_; }
	void dbLoadDetectorConfigs(AMDbObject*) {} //Never called, fastDetectorsConfigurations_ is always valid

protected:
	AMDetectorSet *fastDetectors_;
	AMDetectorSet *allDetectors_;
	AMDetectorInfoSet fastDetectorsConfigurations_;

	QList<SGMFastScanParameters*> settings_;
	SGMFastScanParameters *currentSettings_;

	SGMEnergyParameters *currentEnergyParameters_;
};

#endif // SGMFASTSCANCONFIGURATION_H
