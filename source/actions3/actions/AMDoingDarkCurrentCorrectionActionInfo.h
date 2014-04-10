#ifndef AMDOINGDARKCURRENTCORRECTIONACTIONINFO_H
#define AMDOINGDARKCURRENTCORRECTIONACTIONINFO_H

#include "beamline/AMDetectorTriggerSource.h"
#include "actions3/AMActionInfo3.h"

class AMDoingDarkCurrentCorrectionActionInfo : public AMActionInfo3
{
    Q_OBJECT

public:
    AMDoingDarkCurrentCorrectionActionInfo(AMDetectorDwellTimeSource* dwellTimeSource, int newTime, QObject *parent = 0);
    virtual ~AMDoingDarkCurrentCorrectionActionInfo();

    /// Copy Constructor
    AMDoingDarkCurrentCorrectionActionInfo(const AMDoingDarkCurrentCorrectionActionInfo &other);

    /// This function is used as a virtual copy constructor
    virtual AMDoingDarkCurrentCorrectionActionInfo* createCopy() const;

    /// This should describe the type of the action
    virtual QString typeDescription() const;

    AMDetectorDwellTimeSource* dwellTimeSource() const;
    int dwellTime() const;

//    /// For database storing only
//    AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
//    /// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
//    void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { delete newLoadedObject; }

public slots:
    void setDwellTimeSource(AMDetectorDwellTimeSource* newDwellTimeSource);
    void setDwellTime(int newTime);

protected:
    /// The detector taking a dark currents measurement.
    AMDetectorDwellTimeSource* dwellTimeSource_;
    /// The dwell time for the dark current correction presently taking place.
    int dwellTime_;
};

#endif // AMDOINGDARKCURRENTCORRECTIONACTIONINFO_H
