#ifndef AMDOINGDARKCURRENTCORRECTIONACTION_H
#define AMDOINGDARKCURRENTCORRECTIONACTION_H

#include "actions3/AMAction3.h"
#include "actions3/actions/AMDoingDarkCurrentCorrectionActionInfo.h"

class AMDoingDarkCurrentCorrectionAction : public AMAction3
{
    Q_OBJECT

public:
    explicit AMDoingDarkCurrentCorrectionAction(AMDoingDarkCurrentCorrectionActionInfo *info, QObject *parent = 0);
    AMDoingDarkCurrentCorrectionAction(const AMDoingDarkCurrentCorrectionAction &other);
    virtual ~AMDoingDarkCurrentCorrectionAction();

    virtual AMAction3* createCopy() const;

    virtual bool canPause() const;
    virtual bool canSkip() const;
    virtual bool hasChildren() const;
    virtual int numberOfChildren() const;

protected slots:
    void goFail();
    void goSucceed();

protected:
    virtual void startImplementation();
    virtual void pauseImplementation();
    virtual void resumeImplementation();
    virtual void cancelImplementation();
    virtual void skipImplementation(const QString &command);

    const AMDoingDarkCurrentCorrectionActionInfo* doingDarkCurrentCorrectionInfo() const;
    AMDoingDarkCurrentCorrectionActionInfo* doingDarkCurrentCorrectionInfo();

};

#endif // AMDOINGDARKCURRENTCORRECTIONACTION_H
