#ifndef CSRAPPCONTROLLER_H
#define CSRAPPCONTROLLER_H

#include <QObject>

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

};

#endif // CSRAPPCONTROLLER_H
