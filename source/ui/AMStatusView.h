#ifndef AMSTATUSVIEW_H
#define AMSTATUSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QDebug>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QMetaProperty>

#include <AMErrorMonitor.h>

#define AMSTATUSVIEW_LOG_WIDTH 250
#define AMSTATUSVIEW_LOG_HEIGHT 300
#define AMSTATUSVIEW_LOG_MIN_HEIGHT 93

/// This widget is a simple non-modal dialog that hides itself when it receives a leaveEvent().
class AMHidingDialog : public QDialog {
	Q_OBJECT

public:
	explicit AMHidingDialog(QWidget* parent = 0, Qt::WindowFlags f = 0) : QDialog(parent, f) {
		setMinimumSize(0,0);
		openAnime_ = NULL;
		closeAnime_ = NULL;
		setMouseTracking(true);
		vl_ = new QVBoxLayout(this);
		this->setLayout(vl_);
		vl_->addStretch(1);
		vl_->setDirection(QBoxLayout::BottomToTop);
	}

	void setAnimes(QPropertyAnimation* openAnime, QPropertyAnimation* closeAnime){ openAnime_ = openAnime; closeAnime_ = closeAnime;}
	void append(QString msg, int level){
		QHBoxLayout *layout = new QHBoxLayout(this);
		QString iconMsg = "";
		QString iconPixmap = "";
		switch(level) {
		case AMErrorReport::Information:
			iconMsg = "info!";
			iconPixmap = ":/dialog-information.png";
			break;

		case AMErrorReport::Alert:
			iconMsg = "alert!";
			iconPixmap = ":/dialog-warning.png";
			break;

		case AMErrorReport::Serious:
			iconMsg = "serious!";
			iconPixmap = ":/dialog-error.png";
			break;

		case AMErrorReport::Debug:
			iconMsg = "debug!";
			iconPixmap = ":/applications-development.png";
			break;
		}
		QLabel *localIcon = new QLabel(iconMsg);
		localIcon->setPixmap(QPixmap(iconPixmap));
		layout->addWidget(localIcon);
		setFixedWidth(250);
		QLabel *text = new QLabel(msg);
		layout->addWidget(text);
		layout->addStretch(1);
		layout->addStrut(32);
		layout->setMargin(0);
		vl_->addLayout(layout);
	}

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
				int targetY = geometry().y()-(AMSTATUSVIEW_LOG_HEIGHT-initVal2);
				openAnime_->setStartValue(QRect(geometry().x(), initVal, geometry().width(), initVal2));
				openAnime_->setEndValue(QRect(geometry().x(), targetY, geometry().width(), AMSTATUSVIEW_LOG_HEIGHT));
				openAnime_->start();
			}
		}
		e->ignore();
	}

	/// Mouse leave event:
	virtual void leaveEvent(QEvent* e) {
		if(openAnime_ && closeAnime_){
			if(openAnime_->state() != QAbstractAnimation::Running )
				openAnime_->stop();
			int initVal = geometry().y();
			int initVal2 = geometry().height();
			int targetY = geometry().y() + (initVal2-AMSTATUSVIEW_LOG_MIN_HEIGHT);
			closeAnime_->setStartValue(QRect(geometry().x(), initVal, geometry().width(), initVal2));
			closeAnime_->setEndValue(QRect(geometry().x(), targetY, geometry().width(), AMSTATUSVIEW_LOG_MIN_HEIGHT));
			connect(closeAnime_, SIGNAL(finished()), this, SLOT(onAnimationFinished()));
			closeAnime_->start();
		}
		e->ignore();
	}

	QPropertyAnimation *openAnime_;
	QPropertyAnimation *closeAnime_;
	QVBoxLayout *vl_;
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
		if(!openAnime){
			openAnime = new QPropertyAnimation(popup_, "geometry");
			openAnime->setDuration(1000);
			openAnime->setEasingCurve(QEasingCurve::OutInSine);
			QPropertyAnimation *closeAnime = new QPropertyAnimation(popup_, "geometry");
			closeAnime->setDuration(1000);
			closeAnime->setEasingCurve(QEasingCurve::OutInSine);
			popup_->setAnimes(openAnime, closeAnime);
		}
		if(openAnime->state() == QAbstractAnimation::Running)
			return;
		QPoint d = QPoint(-popup_->geometry().width()/2 + geometry().width()/2, geometry().height());
		popup_->move( mapToGlobal(d) );
		int initVal = popup_->geometry().y();
//		qDebug() << "Suggested " << popup_->minimumHeight();
		openAnime->setStartValue(QRect(popup_->geometry().x(), initVal-AMSTATUSVIEW_LOG_MIN_HEIGHT, popup_->geometry().width(), AMSTATUSVIEW_LOG_MIN_HEIGHT));
		openAnime->setEndValue(QRect(popup_->geometry().x(), initVal-AMSTATUSVIEW_LOG_HEIGHT, popup_->geometry().width(), AMSTATUSVIEW_LOG_HEIGHT));
		popup_->show();
		openAnime->start();
		e->ignore();
	}

};

#endif // AMSTATUSVIEW_H
