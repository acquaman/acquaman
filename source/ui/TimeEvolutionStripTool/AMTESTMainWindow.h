#ifndef AMTESTMAINWINDOW_H
#define AMTESTMAINWINDOW_H

#include <QMainWindow>

/// This is the main window for the time evolution strip tool.  All the important widgets will be here.
class AMTESTMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	/// Constructor.
	explicit AMTESTMainWindow(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMTESTMainWindow();

signals:

public slots:
};

#endif // AMTESTMAINWINDOW_H
