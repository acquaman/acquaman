#ifndef AMDODARKCURRENTCORRECTIONACTIONINFO_H
#define AMDODARKCURRENTCORRECTIONACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"

class AMDoDarkCurrentCorrectionActionInfo : public AMActionInfo3
{
    Q_OBJECT

public:
    explicit AMDoDarkCurrentCorrectionActionInfo(CLSSIS3820Scaler *scaler, double dwellTime, QObject *parent = 0);
    virtual ~AMDoDarkCurrentCorrectionActionInfo();

    AMDoDarkCurrentCorrectionActionInfo(const AMDoDarkCurrentCorrectionActionInfo &other);

    virtual AMDoDarkCurrentCorrectionActionInfo* createCopy() const;

    virtual QString typeDescription() const;

    CLSSIS3820Scaler* scaler() const;
    double dwellTime() const;

protected:
    CLSSIS3820Scaler *scaler_;
    double dwellTime_;


};

#endif // AMDODARKCURRENTCORRECTIONACTIONINFO_H
