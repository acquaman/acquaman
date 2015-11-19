#ifndef AMTESTAPPCONTROLLER_H
#define AMTESTAPPCONTROLLER_H

#include <QObject>

/// Runs the time evolution strip tool.
class AMTESTAppController : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	AMTESTAppController(QObject *parent = 0);
	/// Destructor.
	virtual ~AMTESTAppController();

protected:
};

#endif // AMTESTAPPCONTROLLER_H
