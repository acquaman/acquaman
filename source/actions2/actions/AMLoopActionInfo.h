#ifndef AMLOOPACTIONINFO_H
#define AMLOOPACTIONINFO_H

#include "actions2/AMActionInfo.h"

/// This class specifies the information for an AMLoopAction. (In this case, the information is only the number of iterations to loop for.)  It provides a simple example of a how to subclass AMActionInfo.
class AMLoopActionInfo : public AMActionInfo
{
	Q_OBJECT
	Q_PROPERTY(int loopCount READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)

public:
	/// Constructor. Specify the \c loopCount: number of iterations you want to loop for
	Q_INVOKABLE AMLoopActionInfo(int iterations = 3, QObject *parent = 0);

	/// Copy Constructor
	AMLoopActionInfo(const AMLoopActionInfo& other) : AMActionInfo(other), loopCount_(other.loopCount_) {}

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo* createCopy() const { return new AMLoopActionInfo(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Loop (Repeat)"; }

	// New public functions
	//////////////////////////

	/// The total number of iterations that we're supposed to loop for
	int loopCount() const { return loopCount_; }

	/// Set the total number of iteration to loop for.
	void setLoopCount(int loopCount) { loopCount_ = loopCount; setModified(true); emit loopCountChanged(loopCount_); }

signals:

	/// Emitted when the prescribed number of iterations changes
	void loopCountChanged(int newLoopCount);

public slots:

protected:
	/// The number of iterations to loop for.
	int loopCount_;

};


#endif // AMLOOPACTIONINFO_H
