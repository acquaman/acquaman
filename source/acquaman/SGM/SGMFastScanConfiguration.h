#ifndef SGMFASTSCANCONFIGURATION_H
#define SGMFASTSCANCONFIGURATION_H

#include "acquaman/AMFastScanConfiguration.h"
#include "SGMScanConfiguration.h"
#include <QFileSystemWatcher>

class SGMFastScanParameters;

class SGMFastScanConfiguration : public AMFastScanConfiguration, public SGMScanConfiguration
{
	Q_OBJECT
public:
	SGMFastScanConfiguration(QObject *parent = 0);
	~SGMFastScanConfiguration();

	QString element() const;
	double runTime() const;
	double energyStart() const;
	double energyMidpoint() const;
	double energyEnd() const;
	int velocity() const;
	int velocityBase() const;
	int acceleration() const;
	double scalerTime() const;

	int baseLine() const;

	QString sensibleFileSavePath() const;
	QString finalizedSavePath() const;
	QString sensibleFileSaveWarning() const;

	QStringList presets() const;
	SGMFastScanParameters* currentParameters() const;

	AMDetectorInfoSet* cfgDetectorInfoSet() const { return cfgFastDetectors_;}
	QList<AMDetectorInfo*> usingDetectors() const;

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
	bool setBaseLine(int baseLine);
	bool setSensibleFileSavePath(const QString& sensibleFileSavePath);
	bool setCfgDetectorInfoSet(AMDetectorInfoSet *cfgDetectorInfoSet) { cfgFastDetectors_ = cfgDetectorInfoSet; return true; }

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
	void onBaseLineChanged(int baseLine);

	void onSensibleFileSavePathChanged(const QString& sensibleFileSavePath);
	void onNewFinalizedSavePath(const QString& finalizedSavePath);

protected slots:
	void onSaveDirectoryChanged(const QString& directory);

protected:
	AMDetectorInfoSet *cfgFastDetectors_;
	QList<SGMFastScanParameters*> settings_;
	SGMFastScanParameters *currentSettings_;

	QString sensibleFileSavePath_;
	QString finalizedSavePath_;
	QString sensibleFileSaveWarning_;
	QFileSystemWatcher savePathWatcher_;
};

class SGMFastScanParameters : public QObject
{
	Q_OBJECT
public:
	SGMFastScanParameters(QObject *parent = 0);
	SGMFastScanParameters(const QString &element, double runSeconds, double energyStart, double energyMidpoint, double energyEnd, int velocity, int velocityBase, int acceleration, double scalerTime, int baseLine, QObject *parent = 0);

	QString element() const { return element_;}
	double runSeconds() const { return runSeconds_;}
	double energyStart() const { return energyStart_;}
	double energyMidpoint() const { return energyMidpoint_;}
	double energyEnd() const { return energyEnd_;}
	int velocity() const { return velocity_;}
	int velocityBase() const { return velocityBase_;}
	int acceleration() const { return acceleration_;}
	double scalerTime() const { return scalerTime_;}
	int baseLine() const { return baseLine_;}

public slots:
	void setElement(const QString &element) { element_ = element;}
	void setRunSeconds(double runSeconds) { runSeconds_ = runSeconds;}
	void setEnergyStart(double energyStart) { energyStart_ = energyStart;}
	void setEnergyMidpoint(double energyMidpoint) { energyMidpoint_ = energyMidpoint;}
	void setEnergyEnd(double energyEnd) { energyEnd_ = energyEnd;}
	void setVelocity(int velocity) { velocity_ = velocity;}
	void setVelocityBase(int velocityBase) { velocityBase_ = velocityBase;}
	void setAcceleration(int acceleration) { acceleration_ = acceleration;}
	void setScalerTime(double scalerTime) { scalerTime_ = scalerTime;}
	void setBaseLine(int baseLine) { baseLine_ = baseLine;}

protected:
	QString element_;
	double runSeconds_;
	double energyStart_;
	double energyMidpoint_;
	double energyEnd_;
	int velocity_;
	int velocityBase_;
	int acceleration_;
	double scalerTime_;
	int baseLine_;
};

#endif // SGMFASTSCANCONFIGURATION_H
