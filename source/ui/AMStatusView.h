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

/// This widget is a simple non-modal dialog that hides itself when it receives a leaveEvent().
class AMHidingDialog : public QDialog {
	Q_OBJECT

public:
	explicit AMHidingDialog(QWidget* parent = 0, Qt::WindowFlags f = 0) : QDialog(parent, f) {
		setMouseTracking(true);
	}

protected:
	/// Mouse leave event:
	virtual void leaveEvent(QEvent* e) {
		hide();
		e->ignore();
	}
};

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
	AMHidingDialog* popup_;
	/// contents of detail view
	QTextEdit* detailText_;

	/// Mouse enter event:
	virtual void enterEvent(QEvent* e) {
		// Position popup centered on ourself, overlapping down to bottom of widget.
		QPoint d = QPoint(-popup_->geometry().width()/2 + geometry().width()/2, -popup_->geometry().height() + geometry().height());
		popup_->move( mapToGlobal(d) );
		popup_->show();
		e->ignore();
	}

};

#endif // AMSTATUSVIEW_H
