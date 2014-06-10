#ifndef AMDOINGDARKCURRENTCORRECTIONACTIONINFO_H
#define AMDOINGDARKCURRENTCORRECTIONACTIONINFO_H

#include "beamline/AMDetectorTriggerSource.h"
#include "actions3/AMActionInfo3.h"

class AMDoingDarkCurrentCorrectionActionInfo : public AMActionInfo3
{
    Q_OBJECT

    Q_PROPERTY(double dwellTime READ dwellTime WRITE setDwellTime)

public:
    Q_INVOKABLE AMDoingDarkCurrentCorrectionActionInfo(AMDetectorDwellTimeSource* dwellTimeSource, int newTime, QObject *parent = 0);
    virtual ~AMDoingDarkCurrentCorrectionActionInfo();

    /// Copy Constructor
    AMDoingDarkCurrentCorrectionActionInfo(const AMDoingDarkCurrentCorrectionActionInfo &other);

    /// This function is used as a virtual copy constructor
    virtual AMActionInfo3* createCopy() const;

    /// This should describe the type of the action
    virtual QString typeDescription() const;

    AMDetectorDwellTimeSource* dwellTimeSource() const;
    double dwellTime() const;

public slots:
    void setDwellTimeSource(AMDetectorDwellTimeSource* newDwellTimeSource);
    void setDwellTime(double newTime);

protected:
    /// The detector taking a dark currents measurement.
    AMDetectorDwellTimeSource* dwellTimeSource_;
    /// The dwell time for the dark current correction presently taking place.
    double dwellTime_;
};

#endif // AMDOINGDARKCURRENTCORRECTIONACTIONINFO_H
