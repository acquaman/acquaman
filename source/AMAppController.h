#ifndef AMAPPCONTROLLER_H
#define AMAPPCONTROLLER_H

#include <QObject>


class AMMainWindow;
class QMenuBar;
class QMenu;

class AMAppController : public QObject
{
Q_OBJECT
public:
	explicit AMAppController(QObject *parent = 0);
	virtual ~AMAppController();

signals:

public slots:



protected slots:
	void onActionImport();

protected:
	AMMainWindow* mw_;
	QMenuBar* menuBar_;
	QMenu* fileMenu_;

};

#endif // AMAPPCONTROLLER_H
