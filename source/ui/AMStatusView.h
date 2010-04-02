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
		openAnime_ = NULL;
		closeAnime_ = NULL;
		setMouseTracking(true);
	}

	void setAnimes(QPropertyAnimation* openAnime, QPropertyAnimation* closeAnime){ openAnime_ = openAnime; closeAnime_ = closeAnime;}
	void setTarget(int target){target_ = target;}

protected slots:
	void onAnimationFinished(){
		hide();
	}

protected:
	virtual void enterEvent(QEvent *e){
		if(openAnime_ && closeAnime_){
			if(closeAnime_->state() == QAbstractAnimation::Running)
				closeAnime_->stop();
			if(openAnime_->state() != QAbstractAnimation::Running)
			{
				int initVal = geometry().y();
				int initVal2 = geometry().height();
				int myY = geometry().y()-(300-initVal2);
				openAnime_->setStartValue(QRect(geometry().x(), initVal, geometry().width(), initVal2));
				openAnime_->setEndValue(QRect(geometry().x(), myY, geometry().width(), 300));

//				openAnime_->setStartValue(QRect(geometry().x(), initVal, geometry().width(), geometry().height()));
//				openAnime_->setEndValue(QRect(geometry().x(), initVal-280, geometry().width(), geometry().height()));

//				openAnime_->setStartValue(QRect(geometry().x(), geometry().y(), geometry().width(), initVal));
//				openAnime_->setEndValue(QRect(geometry().x(), geometry().y(), geometry().width(), 300));
				openAnime_->start();
			}
		}
		e->ignore();
	}

	/// Mouse leave event:
	virtual void leaveEvent(QEvent* e) {
		if(openAnime_ && closeAnime_){
//			qDebug() << "Current y " << geometry().y() << " height " << geometry().height();
//			QPoint d = mapToGlobal(QPoint(geometry().x(), geometry().y()));
//			qDebug() << "Global y " << d.y();
			if(openAnime_->state() != QAbstractAnimation::Running )
				openAnime_->stop();
			int initVal = geometry().y();
			int initVal2 = geometry().height();
			int myTarget = geometry().y() + (initVal2-93);
			closeAnime_->setStartValue(QRect(geometry().x(), initVal, geometry().width(), initVal2));
			closeAnime_->setEndValue(QRect(geometry().x(), myTarget, geometry().width(), 93));

//			qDebug() << "Target is " << target_ << " initVal " << initVal << " going to " <<  initVal+300-93;
//			closeAnime_->setStartValue(QRect(geometry().x(), initVal, geometry().width(), initVal2));
//			closeAnime_->setEndValue(QRect(geometry().x(), initVal+300-93, geometry().width(), 93));

	//		closeAnime_->setStartValue(QRect(geometry().x(), geometry().y(), geometry().width(), initVal));
	//		closeAnime_->setEndValue(QRect(geometry().x(), geometry().y(), geometry().width(), 20));
			connect(closeAnime_, SIGNAL(finished()), this, SLOT(onAnimationFinished()));
			closeAnime_->start();
	//		hide();
		}
		e->ignore();
	}

	QPropertyAnimation *openAnime_;
	QPropertyAnimation *closeAnime_;
	int target_;
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
	QPropertyAnimation *openAnime;
	QPropertyAnimation *closeAnime;
	QPropertyAnimation *open2Anime;

	/// Mouse enter event:
	virtual void enterEvent(QEvent* e) {
//		qDebug() << "openAnime at " << (int)openAnime;
		if(!openAnime){
			openAnime = new QPropertyAnimation(popup_, "geometry");
			openAnime->setDuration(1000);
			openAnime->setEasingCurve(QEasingCurve::OutInSine);
//			qDebug() << "Creating open animation " << (int)openAnime;

			QPropertyAnimation *closeAnime = new QPropertyAnimation(popup_, "geometry");
			closeAnime->setDuration(1000);
			closeAnime->setEasingCurve(QEasingCurve::OutInSine);
//			qDebug() << "Creating close animation " << (int)closeAnime;

			popup_->setAnimes(openAnime, closeAnime);
		}
		if(openAnime->state() == QAbstractAnimation::Running)
			return;
//		popup_->setGeometry(popup_->geometry().x(), popup_->geometry().y(), popup_->geometry().width(), 0);

		// Position popup centered on ourself, overlapping down to bottom of widget.
//		qDebug() << popup_->geometry().height() << " " << geometry().height();
//		QPoint d = QPoint(-popup_->geometry().width()/2 + geometry().width()/2, -popup_->geometry().height() + geometry().height());
		QPoint d = QPoint(-popup_->geometry().width()/2 + geometry().width()/2, geometry().height());
		popup_->move( mapToGlobal(d) );

/*
		openAnime = new QPropertyAnimation(popup_, "geometry");
		openAnime->setDuration(1000);
		openAnime->setEasingCurve(QEasingCurve::OutInSine);
*/
//		open2Anime = new QPropertyAnimation(popup_, "geometry");
//		open2Anime->setDuration(1000);
//		open2Anime->setEasingCurve(QEasingCurve::OutInSine);

		int initVal = popup_->geometry().y();
		popup_->setTarget(initVal);
		openAnime->setStartValue(QRect(popup_->geometry().x(), initVal-93, popup_->geometry().width(), 93));
		openAnime->setEndValue(QRect(popup_->geometry().x(), initVal-300, popup_->geometry().width(), 300));
	//	openAnime->setStartValue(QRect(popup_->geometry().x(), popup_->geometry().y(), popup_->geometry().width(), 20));
	//	openAnime->setEndValue(QRect(popup_->geometry().x(), popup_->geometry().y(), popup_->geometry().width(), 300));
//		open2Anime->setStartValue(QRect(popup_->geometry().x(), initVal-20, popup_->geometry().width(), popup_->geometry().height()));
//		open2Anime->setEndValue(QRect(popup_->geometry().x(), initVal-300, popup_->geometry().width(), popup_->geometry().height()));

//		QParallelAnimationGroup *group = new QParallelAnimationGroup;
//		 group->addAnimation(openAnime);
//		 group->addAnimation(open2Anime);

		/**/
		/*
		QPropertyAnimation *openAnime = new QPropertyAnimation(this, "geometry");
		openAnime->setDuration(750);
		int initVal = geometry().x();
		openAnime->setStartValue(QRect(initVal, geometry().y(), geometry().width(), geometry().height()));
		openAnime->setEndValue(QRect(initVal-50, geometry().y(), geometry().width(), geometry().height()));
		*/

		popup_->show();
		openAnime->start();
//		open2Anime->start();
//		setGeometry(geometry().x()-50, geometry().y(), geometry().width(), geometry().height());
		e->ignore();
	}

};

#endif // AMSTATUSVIEW_H
