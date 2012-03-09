#ifndef REIXSMOVETOSAMPLETRANSFERPOSITIONACTIONINFO_H
#define REIXSMOVETOSAMPLETRANSFERPOSITIONACTIONINFO_H

#include "actions2/AMActionInfo.h"
#include "dataman/info/AMControlInfoList.h"

/// Specifies the sample manipulator position for transferring samples, used by REIXSMoveToSampleTransferPositionAction. For now, we don't save anything special in the database; positions_ is simply hard-coded into the constructor. Can add something fancier if we need to adjust it a lot.
class REIXSMoveToSampleTransferPositionActionInfo : public AMActionInfo
{
    Q_OBJECT
public:
	/// Default constructor, moves to hard-coded position defined here
	Q_INVOKABLE REIXSMoveToSampleTransferPositionActionInfo(QObject *parent = 0);
	/// Copy constructor
	REIXSMoveToSampleTransferPositionActionInfo(const REIXSMoveToSampleTransferPositionActionInfo& other);
	/// Virtual copy constructor
	AMActionInfo* createCopy() const { return new REIXSMoveToSampleTransferPositionActionInfo(*this); }


	/// Returns the sample manipulator position to move to.
	const AMControlInfoList& positions() const { return positions_; }

signals:

public slots:

protected:
	AMControlInfoList positions_;

};

#endif // REIXSMOVETOSAMPLETRANSFERPOSITIONACTIONINFO_H
