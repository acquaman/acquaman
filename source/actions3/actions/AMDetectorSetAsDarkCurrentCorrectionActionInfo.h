#ifndef AMDETECTORSETASDARKCURRENTCORRECTIONACTIONINFO_H
#define AMDETECTORSETASDARKCURRENTCORRECTIONACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMDetectorInfo.h"

class AMDetectorSetAsDarkCurrentCorrectionActionInfo : public AMActionInfo3
{
Q_OBJECT
public:
    /// Constructor
    Q_INVOKABLE AMDetectorSetAsDarkCurrentCorrectionActionInfo(const AMDetectorInfo &detectorInfo = AMDetectorInfo(), QObject *parent = 0);
    virtual ~AMDetectorSetAsDarkCurrentCorrectionActionInfo();

    /// Copy Constructor
    AMDetectorSetAsDarkCurrentCorrectionActionInfo(const AMDetectorSetAsDarkCurrentCorrectionActionInfo &other);

    /// This function is used as a virtual copy constructor
    virtual AMDetectorSetAsDarkCurrentCorrectionActionInfo* createCopy() const { return new AMDetectorSetAsDarkCurrentCorrectionActionInfo(*this); }

    /// This should describe the type of the action
    virtual QString typeDescription() const { return "Detector Set Dark Current Correction"; }

    /// Returns a pointer to our detectorInfo
    const AMDetectorInfo* detectorInfo() const { return &detectorInfo_; }

    /// For database storing only
    AMDetectorInfo* dbReadDetectorInfo() { return &detectorInfo_; }
    /// For database loading only. This function will never be called since dbReadDetectorInfo() always returns a valid pointer
    void dbLoadDetectorInfo(AMDbObject *newLoadedObject) { delete newLoadedObject; }

protected:
    /// The AMDetectorInfo that specifies which detector to initialize
    AMDetectorInfo detectorInfo_;
};

#endif // AMDETECTORSETASDARKCURRENTCORRECTIONACTIONINFO_H
