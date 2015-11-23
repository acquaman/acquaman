#ifndef AMTIMEOUTACTIONINFO_H
#define AMTIMEOUTACTIONINFO_H

#include "actions3/actions/AMWaitActionInfo.h"

class AMTimeoutActionInfo : public AMWaitActionInfo
{
	Q_OBJECT

	Q_PROPERTY(QString instruction READ instruction WRITE setInstruction)

public:
	/// Constructor.
	Q_INVOKABLE AMTimeoutActionInfo(double waitTime = 0.0, const QString &instruction = QString(), QObject *parent = 0);
	/// Copy constructor.
	AMTimeoutActionInfo(const AMTimeoutActionInfo &original);
	/// Destructor.
	virtual ~AMTimeoutActionInfo();

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const;

	/// This should describe the type of the action.
	virtual QString typeDescription() const { return QString("Timed Wait of %1 seconds").arg(waitTime_); }

	/// Returns the instruction provided.
	QString instruction() const { return instruction_; }

signals:
	/// Notifier that the instruction has changed.
	void instructionChanged(const QString &newInstruction);

public slots:
	/// Sets the instruction.
	void setInstruction(const QString &newInstruction);

protected:
	/// The instruction prompting the user for action.
	QString instruction_;
};

#endif // AMTIMEOUTACTIONINFO_H
