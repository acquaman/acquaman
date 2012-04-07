#ifndef AMLOOPACTIONINFO3_H
#define AMLOOPACTIONINFO3_H

#include "actions3/AMListActionInfo3.h"

/// This class specifies the information for an AMLoopAction. (In this case, the information is only the number of iterations to loop for.)  It provides a simple example of a how to subclass AMActionInfo.
class AMLoopActionInfo3 : public AMListActionInfo3
{
	Q_OBJECT
	Q_PROPERTY(int loopCount READ loopCount WRITE setLoopCount NOTIFY loopCountChanged)

public:
	/// Constructor. Specify the \c loopCount: number of iterations you want to loop for
	Q_INVOKABLE AMLoopActionInfo3(int iterations = 3, const QString& shortDescription = "Loop", const QString& longDescription = "Loop of Actions to Run", const QString& iconFileName = ":/32x32/media-playlist-repeat.png", QObject *parent = 0);

	/// Copy Constructor
	AMLoopActionInfo3(const AMLoopActionInfo3& other) : AMListActionInfo3(other), loopCount_(other.loopCount_) {}

	/// This function is used as a virtual copy constructor
	virtual AMActionInfo3* createCopy() const { return new AMLoopActionInfo3(*this); }

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "Loop (Repeat)"; }

	// New public functions
	//////////////////////////

	/// The total number of iterations that we're supposed to loop for
	int loopCount() const { return loopCount_; }

	/// Set the total number of iteration to loop for.
	void setLoopCount(int loopCount);

signals:

	/// Emitted when the prescribed number of iterations changes
	void loopCountChanged(int newLoopCount);

public slots:

protected:
	/// The number of iterations to loop for.
	int loopCount_;

};


#endif // AMLOOPACTIONINFO_H
