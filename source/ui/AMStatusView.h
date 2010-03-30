#ifndef AMSTATUSVIEW_H
#define AMSTATUSVIEW_H

#include <QLabel>
#include <QDialog>
#include <QTextEdit>
#include <QHBoxLayout>

#include <AMErrorMonitor.h>

/// This widget shows the last error/status message in a label, and displays a larger textview with a history of messages when you mouse-over it.
class AMStatusView : public QWidget
{
Q_OBJECT
public:
	/// Constructor
    explicit AMStatusView(QWidget *parent = 0);


signals:

public slots:
	/// Handles any errors that are logged using AMErrorMon::report(AMErrorCode())
	void onAnyError(AMErrorReport e) {}

protected:
	/// Icons representing the nature of the last notification
	QLabel *iconInfo_, *iconAlert_, *iconSerious_, *iconDebug_;
	/// last notification text
	QLabel* shortText_;

	/// layout
	QHBoxLayout* hl_;

	/// popup box for mouse-over detail view
	QDialog* popup_;
	/// contents of detail view
	QTextEdit* longText_;

};

#endif // AMSTATUSVIEW_H
