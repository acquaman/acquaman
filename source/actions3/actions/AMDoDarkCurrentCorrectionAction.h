#ifndef AMDODARKCURRENTCORRECTIONACTION_H
#define AMDODARKCURRENTCORRECTIONACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMDoDarkCurrentCorrectionActionInfo.h"

class AMDoDarkCurrentCorrectionAction : public AMAction3
{
    Q_OBJECT
public:
    explicit AMDoDarkCurrentCorrectionAction(AMDoDarkCurrentCorrectionActionInfo *info, QObject *parent = 0);
    AMDoDarkCurrentCorrectionAction(const AMDoDarkCurrentCorrectionAction &other);
    virtual ~AMDoDarkCurrentCorrectionAction();

    virtual AMAction3* createCopy() const;

    virtual bool canPause() const;
    virtual bool canSkip() const;
    virtual bool hasChildren() const;
    virtual int numberOfChildren() const;

protected slots:
    void onNewDarkCurrentMeasurementState(CLSSIS3820Scaler::DarkCurrentCorrectionState newState);

private:
    virtual void startImplementation();
    virtual void pauseImplementation();
    virtual void resumeImplementation();
    virtual void cancelImplementation();
    virtual void skipImplementation(const QString &command);

    const AMDoDarkCurrentCorrectionActionInfo* doDarkCurrentCorrectionInfo() const;
    AMDoDarkCurrentCorrectionActionInfo* doDarkCurrentCorrectionInfo();

};

#endif // AMDODARKCURRENTCORRECTIONACTION_H
