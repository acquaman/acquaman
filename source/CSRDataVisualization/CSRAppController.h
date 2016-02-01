#ifndef CSRAPPCONTROLLER_H
#define CSRAPPCONTROLLER_H

#include <QObject>

#include "CSRDataVisualization/CSRMainWindow.h"

class CSRAppController : public QObject
{
	Q_OBJECT
public:
	explicit CSRAppController(QObject *parent = 0);
	~CSRAppController();

signals:

public slots:
	/// Starts the application.
	bool start();

protected:
	/// The main window.
	CSRMainWindow *mainWindow_;
};

#endif // CSRAPPCONTROLLER_H
