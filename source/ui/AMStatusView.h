#ifndef AMSTATUSVIEW_H
#define AMSTATUSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QEvent>
#include <QDebug>

#include <AMErrorMonitor.h>

#define AMSTATUSVIEW_LOG_WIDTH 500
#define AMSTATUSVIEW_LOG_HEIGHT 400

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
	void onAnyError(AMErrorReport e);

protected:
	/// Icons representing the nature of the last notification
	QLabel *iconInfo_, *iconAlert_, *iconSerious_, *iconDebug_, *currentIcon_;
	/// last notification text
	QLabel* shortText_;

	/// layout
	QHBoxLayout* hl_;

	/// popup box for mouse-over detail view
	QDialog* popup_;
	/// contents of detail view
	QTextEdit* detailText_;

	/// Mouse enter event:
	virtual void enterEvent(QEvent* e) {
		QPoint d = QPoint(-AMSTATUSVIEW_LOG_WIDTH/2 + geometry().width()/2, -AMSTATUSVIEW_LOG_HEIGHT);
		popup_->move( mapToGlobal(d) );
		popup_->show();
		e->ignore();
	}
	/// Mouse leave event:
	virtual void leaveEvent(QEvent* e) {
		popup_->hide();
		e->ignore();
	}

};

#endif // AMSTATUSVIEW_H
