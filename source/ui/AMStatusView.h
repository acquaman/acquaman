#ifndef AMSTATUSVIEW_H
#define AMSTATUSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QEvent>
#include <QDebug>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QMetaProperty>

#include <AMErrorMonitor.h>

#define AMSTATUSVIEW_LOG_WIDTH 250
#define AMSTATUSVIEW_LOG_HEIGHT 300

/// This widget is a simple non-modal dialog that hides itself when it receives a leaveEvent().
class AMHidingDialog : public QDialog {
	Q_OBJECT

public:
	explicit AMHidingDialog(QWidget* parent = 0, Qt::WindowFlags f = 0) : QDialog(parent, f) {
		setMinimumSize(0,0);
		setMouseTracking(true);
	}

protected slots:
	void onAnimationFinished(){
		hide();
	}

protected:
	/// Mouse leave event:
	virtual void leaveEvent(QEvent* e) {
		QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
		animation->setDuration(1000);
		animation->setEasingCurve(QEasingCurve::OutInSine);
		int initVal = geometry().height();
		animation->setStartValue(QRect(geometry().x(), geometry().y(), geometry().width(), initVal));
		animation->setEndValue(QRect(geometry().x(), geometry().y(), geometry().width(), 20));
		connect(animation, SIGNAL(finished()), this, SLOT(onAnimationFinished()));
		animation->start();
//		hide();
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
//		popup_->setGeometry(popup_->geometry().x(), popup_->geometry().y(), popup_->geometry().width(), 0);

		// Position popup centered on ourself, overlapping down to bottom of widget.
		qDebug() << popup_->geometry().height() << " " << geometry().height();
//		QPoint d = QPoint(-popup_->geometry().width()/2 + geometry().width()/2, -popup_->geometry().height() + geometry().height());
		QPoint d = QPoint(-popup_->geometry().width()/2 + geometry().width()/2, 0);
		popup_->move( mapToGlobal(d) );

		/**/
		QPropertyAnimation *animation = new QPropertyAnimation(popup_, "geometry");
		animation->setDuration(1000);
		animation->setEasingCurve(QEasingCurve::OutInSine);
		QPropertyAnimation *animation2 = new QPropertyAnimation(popup_, "geometry");
		animation2->setDuration(1000);
		animation2->setEasingCurve(QEasingCurve::OutInSine);
		int initVal = popup_->geometry().y();
	//	animation->setStartValue(QRect(popup_->geometry().x(), popup_->geometry().y(), popup_->geometry().width(), 20));
	//	animation->setEndValue(QRect(popup_->geometry().x(), popup_->geometry().y(), popup_->geometry().width(), 300));
		animation->setStartValue(QRect(popup_->geometry().x(), popup_->geometry().y(), popup_->geometry().width(), 20));
		animation->setEndValue(QRect(popup_->geometry().x(), popup_->geometry().y(), popup_->geometry().width(), 300));
		animation2->setStartValue(QRect(popup_->geometry().x(), initVal-20, popup_->geometry().width(), popup_->geometry().height()));
		animation2->setEndValue(QRect(popup_->geometry().x(), initVal-300, popup_->geometry().width(), popup_->geometry().height()));

		QParallelAnimationGroup *group = new QParallelAnimationGroup;
		 group->addAnimation(animation);
		 group->addAnimation(animation2);

		/**/
		/*
		QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
		animation->setDuration(750);
		int initVal = geometry().x();
		animation->setStartValue(QRect(initVal, geometry().y(), geometry().width(), geometry().height()));
		animation->setEndValue(QRect(initVal-50, geometry().y(), geometry().width(), geometry().height()));
		*/

		popup_->show();
		animation->start();
//		animation2->start();
//		setGeometry(geometry().x()-50, geometry().y(), geometry().width(), geometry().height());
		e->ignore();
	}

};

#endif // AMSTATUSVIEW_H
