#ifndef AMLISTACTIONINFO3_H
#define AMLISTACTIONINFO3_H

#include "actions3/AMActionInfo3.h"

class AMListActionInfo3 : public AMActionInfo3
{
Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMListActionInfo3(const QString& shortDescription = "List", const QString& longDescription = "List of Actions to Run", const QString& iconFileName = ":/32x32/media-playlist-repeat.png", QObject *parent = 0);
	/// Copy constructor.
	AMListActionInfo3(const AMListActionInfo3 &other);
	/// Method that creates a copy of the current action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns a description for this action.
	virtual QString typeDescription() const;
};

class AMSequentialListActionInfo3 : public AMListActionInfo3
{
Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMSequentialListActionInfo3(const QString& shortDescription = "Sequential List", const QString& longDescription = "Sequential List of Actions to Run", const QString& iconFileName = ":/32x32/media-playlist-repeat.png", QObject *parent = 0);
	/// Copy constructor.
	AMSequentialListActionInfo3(const AMSequentialListActionInfo3 &other);
	/// Method that creates a copy of the current action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns a description for this action.
	virtual QString typeDescription() const;
};

class AMParallelListActionInfo3 : public AMListActionInfo3
{
Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE AMParallelListActionInfo3(const QString& shortDescription = "Parallel List", const QString& longDescription = "Parallel List of Actions to Run", const QString& iconFileName = ":/32x32/media-playlist-repeat.png", QObject *parent = 0);
	/// Copy constructor.
	AMParallelListActionInfo3(const AMParallelListActionInfo3 &other);
	/// Method that creates a copy of the current action info.
	virtual AMActionInfo3* createCopy() const;
	/// Returns a description for this action.
	virtual QString typeDescription() const;
};

#endif // AMLISTACTIONINFO3_H
