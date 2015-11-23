/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef SGMELEMENTINFO_H
#define SGMELEMENTINFO_H

#include <QObject>
#include <QDebug>

#include "util/AMElement.h"
#include "util/AMOrderedSet.h"
#include "dataman/database/AMDbObject.h"

class SGMFastScanParameters;

class SGMEnergyPosition : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(double energy READ energy WRITE setEnergy)
Q_PROPERTY(int monoEncoderTarget READ monoEncoderTarget WRITE setMonoEncoderTarget)
Q_PROPERTY(int undulatorStepSetpoint READ undulatorStepSetpoint WRITE setUndulatorStepSetpoint)
Q_PROPERTY(double exitSlitDistance READ exitSlitDistance WRITE setExitSlitDistance)
Q_PROPERTY(int sgmGrating READ sgmGrating WRITE setSGMGrating)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Energy Settings")

public:
	virtual ~SGMEnergyPosition();
	Q_INVOKABLE SGMEnergyPosition(const QString &name = QString(), double energy = -1, int monoEncoderTarget = -1, int undulatorStepSetpoint = -1, double exitSlitDistance = -1, int sgmGrating = -1);

	QString descriptionFromName() const;
	double energy() const;
	int monoEncoderTarget() const;
	int undulatorStepSetpoint() const;
	double exitSlitDistance() const;
	int sgmGrating() const;

	SGMEnergyPosition& operator=(const SGMEnergyPosition &other);
	bool operator==(const SGMEnergyPosition &other) const;
	bool operator!=(const SGMEnergyPosition &other) const;
	friend QDebug operator<<(QDebug d, const SGMEnergyPosition &energyPosition);

public slots:
	void setEnergy(double energy);
	void setMonoEncoderTarget(int monoEncoderTarget);
	void setUndulatorStepSetpoint(int undulatorStepSetpoint);
	void setExitSlitDistance(double exitSlitDistance);
	void setSGMGrating(int sgmGrating);

signals:
	void energyChanged(double energy);
	void monoEncoderTargetChanged(int monoEncoderTarget);
	void undulatorStepSetpointChanged(int undulatorStepSetpoint);
	void exitSlitDistanceChanged(double exitSlitDistance);
	void sgmGratingChanged(int sgmGrating);
	void energyPositionChanged();

protected:
	double energy_;
	int monoEncoderTarget_;
	int undulatorStepSetpoint_;
	double exitSlitDistance_;
	int sgmGrating_;
};


class SGMScanInfo : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(QString scanName READ scanName WRITE setScanName)
Q_PROPERTY(bool hasEdge READ hasEdge WRITE setHasEdge)
Q_PROPERTY(QString edge READ edge WRITE setEdge)
Q_PROPERTY(double energy READ energy WRITE setEnergy)
Q_PROPERTY(AMDbObject* start READ dbGetStart WRITE dbLoadStart)
Q_PROPERTY(AMDbObject* middle READ dbGetMiddle WRITE dbLoadMiddle)
Q_PROPERTY(AMDbObject* end READ dbGetEnd WRITE dbLoadEnd)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Scan Info")

public:
	/// This is the new SGMScanInfo constructor using the new AMAbsorptionEdge class.  It should clean some things up and should Dave want to later, maybe make some of these other things easier to handle.
	virtual ~SGMScanInfo();
	Q_INVOKABLE SGMScanInfo(const QString &scanName = QString(), AMAbsorptionEdge edge = AMAbsorptionEdge(), SGMEnergyPosition start = SGMEnergyPosition(), SGMEnergyPosition middle = SGMEnergyPosition(), SGMEnergyPosition end = SGMEnergyPosition(), QObject *parent = 0);
	SGMScanInfo(const QString &scanName, QPair<QString, double> edgeAndEnergy, SGMEnergyPosition start = SGMEnergyPosition(), SGMEnergyPosition middle = SGMEnergyPosition(), SGMEnergyPosition end = SGMEnergyPosition(), QObject *parent = 0);

	QString scanName() const;
	QString descriptionFromName() const;
	bool hasEdge() const;
	QString edge() const;
	double energy() const;
	const SGMEnergyPosition& start() const;
	const SGMEnergyPosition& middle() const;
	const SGMEnergyPosition& end() const;

	SGMScanInfo& operator=(const SGMScanInfo &other);
	bool operator==(const SGMScanInfo &other) const;
	bool operator!=(const SGMScanInfo &other) const;
	friend QDebug operator<<(QDebug d, const SGMScanInfo &scanInfo);

public slots:
	void setScanName(const QString &scanName);

	void setHasEdge(bool hasEdge);
	void setEdge(const QString &edge);
	void setEnergy(double energy);

	// The pre-emptive check for equality ignores the database id and only checks the actual values under normal circumstances
	void setStart(const SGMEnergyPosition &start, bool ignoreDatabaseId = true);
	// The pre-emptive check for equality ignores the database id and only checks the actual values under normal circumstances
	void setMiddle(const SGMEnergyPosition &middle, bool ignoreDatabaseId = true);
	// The pre-emptive check for equality ignores the database id and only checks the actual values under normal circumstances
	void setEnd(const SGMEnergyPosition &end, bool ignoreDatabaseId = true);

signals:
	void hasEdgeChanged(bool hasEdge);
	void edgeChanged(const QString &edge);
	void energyChanged(double energy);

	void startChanged();
	void middleChanged();
	void endChanged();

	void scanInfoChanged();

protected:
	AMDbObject* dbGetStart() { return &start_;}
	void dbLoadStart(AMDbObject*) {} //never called
	AMDbObject* dbGetMiddle() { return &middle_;}
	void dbLoadMiddle(AMDbObject*) {} //never called
	AMDbObject* dbGetEnd() { return &end_;}
	void dbLoadEnd(AMDbObject*) {} //never called

protected:
	QString scanName_;

	bool hasEdge_;
	QString edge_;
	double energy_;

	SGMEnergyPosition start_;
	SGMEnergyPosition middle_;
	SGMEnergyPosition end_;
};


class SGMElementInfo : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(AMDbObjectList sgmEdgeInfos READ dbReadSGMEdgeInfos WRITE dbLoadSGMEdgeInfos)
Q_PROPERTY(AMDbObjectList sgmFastScanParameters READ dbReadSGMFastScanParameters WRITE dbLoadSGMFastScanParameters)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Element Info")

public:
	virtual ~SGMElementInfo();
	Q_INVOKABLE SGMElementInfo(const QString &name = QString(), AMElement *element = 0, QObject *parent = 0);

	AMElement* element() const;

	AMOrderedSet<QString, SGMScanInfo> sgmEdgeInfos() const;
	AMOrderedSet<int, SGMFastScanParameters*> availableFastScanParameters() const;

	bool loadFromDb(AMDatabase *db, int id);

public slots:
	bool addEdgeInfo(const SGMScanInfo &edgeInfo);
	bool addFastScanParameters(SGMFastScanParameters *parameters);

protected:
	AMDbObjectList dbReadSGMEdgeInfos();
	AMDbObjectList dbReadSGMFastScanParameters();

	void dbLoadSGMEdgeInfos(const AMDbObjectList &sgmEdgeInfos);
	void dbLoadSGMFastScanParameters(const AMDbObjectList &sgmFastScanParameters);

protected:
	AMElement *element_;
	AMOrderedSet<QString, SGMScanInfo> sgmEdgeInfos_;
	AMOrderedSet<int, SGMFastScanParameters*> availableFastScanParameters_;
};

class SGMFastScanSettings : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(double runSeconds READ runSeconds WRITE setRunSeconds)
Q_PROPERTY(int motorSettings READ motorSettings WRITE setMotorSettings)
Q_PROPERTY(double scalerTime READ scalerTime WRITE setScalerTime)
Q_PROPERTY(int baseLine READ baseLine WRITE setBaseLine)
Q_PROPERTY(int undulatorVelocity READ undulatorVelocity WRITE setUndulatorVelocity)

public:
	virtual ~SGMFastScanSettings();
	explicit SGMFastScanSettings(const QString &name = QString(), double runSeconds = -1, int motorSettings = 0, double scalerTime = -1, int baseLine = -1, int undulatorVelocity = 0, QObject *parent = 0);

	QString descriptionFromName() const;
	double runSeconds() const;
	int motorSettings() const;
	double scalerTime() const;
	int baseLine() const;
	int undulatorVelocity() const;

	SGMFastScanSettings& operator =(const SGMFastScanSettings &other);

	bool operator ==(const SGMFastScanSettings &other) const;
	bool operator !=(const SGMFastScanSettings &other) const;

public slots:
	void setRunSeconds(double runSeconds);
	void setMotorSettings(int motorSettings);
	void setScalerTime(double scalerTime);
	void setBaseLine(int baseLine);
	void setUndulatorVelocity(int undulatorVelocity);

signals:
	void runSecondsChanged(double runSeconds);
	void motorSettingsChanged(int motorSetting);
	void scalerTimeChanged(double scalerTime);
	void baseLineChanged(int baseLine);
	void undulatorVelocityChanged(int undulatorVelocity);

	void fastScanSettingsChanged();

protected:
	double runSeconds_;
	int motorSettings_;
	double scalerTime_;
	int baseLine_;
	int undulatorVelocity_;
};

class SGMFastScanParameters : public AMDbObject
{
Q_OBJECT

Q_PROPERTY(QString element READ element WRITE setElement)
Q_PROPERTY(AMDbObject* scanInfo READ dbGetScanInfo WRITE dbLoadScanInfo)
Q_PROPERTY(AMDbObject* fastScanSettings READ dbGetFastScanSettings WRITE dbLoadFastScanSettings)

Q_CLASSINFO("AMDbObject_Attributes", "description=SGM Fast Scan Parameters")

public:
	virtual ~SGMFastScanParameters();
	Q_INVOKABLE SGMFastScanParameters(const QString &name = QString(), const QString &element = QString(), const SGMScanInfo &scanInfo = SGMScanInfo(), const SGMFastScanSettings &fastScanSettings = SGMFastScanSettings(), QObject *parent = 0);

	SGMFastScanParameters& operator =(const SGMFastScanParameters &other);

	bool operator==(const SGMFastScanParameters &other);
	friend QDebug operator<<(QDebug d, const SGMFastScanParameters &fastScanParameters);

	QString element() const;
	QString edge() const;
	double runSeconds() const;
	double energyStart() const;
	double energyMidpoint() const;
	double energyEnd() const;
	int velocity() const;
	int velocityBase() const;
	int acceleration() const;
	double scalerTime() const;
	int baseLine() const;

	int undulatorStartStep() const;
	int undulatorVelocity() const;
	int undulatorRelativeStep() const;
	double exitSlitDistance() const;
	int sgmGrating() const;

	SGMScanInfo scanInfo() const;
	SGMFastScanSettings fastScanSettings() const;

public slots:
	void setElement(const QString &element);

	void setEdge(const QString &edge);
	void setRunSeconds(double runSeconds);
	void setEnergyStart(double energyStart);
	void setEnergyMidpoint(double energyMidpoint);
	void setEnergyEnd(double energyEnd);
	void setVelocity(int velocity);
	void setVelocityBase(int velocityBase);
	void setAcceleration(int acceleration);
	void setScalerTime(double scalerTime);
	void setBaseLine(int baseLine);
	void setUndulatorStartStep(int undulatorStartStep);
	void setUndulatorVelocity(int undulatorVelocity);
	void setUndulatorRelativeStep(int undulatorRelativeStep);
	void setExitSlitDistance(double exitSlitDistance);
	void setSGMGrating(int sgmGrating);

	// When ignoreDatabaseId is false, then checks for equality in the SGMEnergyPosition values also compare the database ids
	void setScanInfo(const SGMScanInfo &scanInfo, bool ignoreDatabaseId = true);
	void setStartPosition(const SGMEnergyPosition &start);
	void setMiddlePosition(const SGMEnergyPosition &middle);
	void setEndPosition(const SGMEnergyPosition &end);
	void setFastScanSettings(const SGMFastScanSettings &fastScanSettings);

signals:
	void elementChanged(const QString &element);
	void runSecondsChanged(double runSeconds);
	void energyStartChanged(double energyStart);
	void energyMidpointChanged(double energyMidpoint);
	void energyEndChanged(double energyEnd);
	void velocityChanged(int velocity);
	void velocityBaseChanged(int velocityBase);
	void accelerationChanged(int acceleration);
	void scalerTimeChanged(double scalerTime);
	void baseLineChanged(int baseLine);
	void undulatorStartStepChanged(int undulatorStartStep);
	void undulatorVelocityChanged(int undulatorVelocity);
	void undulatorRelativeStepChanged(int undulatorRelativeStep);
	void exitSlitDistanceChanged(double exitSlitDistance);
	void sgmGratingChanged(int sgmGrating);

	void scanInfoChanged();
	void startPositionChanged();
	void middlePositionChanged();
	void endPositionChanged();
	void fastScanSettingsChanged();

protected slots:
	void onStartChanged();
	void onMiddleChanged();
	void onEndChanged();

protected:
	AMDbObject* dbGetScanInfo() { return &scanInfo_;}
	void dbLoadScanInfo(AMDbObject*) {} //never called

	AMDbObject* dbGetFastScanSettings() { return &fastScanSettings_;}
	void dbLoadFastScanSettings(AMDbObject*) {} //never called

protected:
	QString element_;

	SGMScanInfo scanInfo_;
	SGMFastScanSettings fastScanSettings_;
};

#endif // SGMELEMENTINFO_H
