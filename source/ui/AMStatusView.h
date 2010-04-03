#ifndef AMSTATUSVIEW_H
#define AMSTATUSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
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

	Q_PROPERTY(QRect myGeometry READ myGeometry WRITE setMyGeometry)

public:
	explicit AMHidingDialog(QWidget* parent = 0, Qt::WindowFlags f = 0) : QDialog(parent, f) {
		setMinimumSize(0,0);
		openAnime_ = NULL;
		closeAnime_ = NULL;
		setMouseTracking(true);
/*
		fr_ = new QFrame(this, 0);
		vl_ = new QVBoxLayout(fr_);
		fr_->setLayout(vl_);
		vl_->addStretch(1);
		vl_->setDirection(QBoxLayout::BottomToTop);
*/
		sa_ = NULL;

/*		sa_ = new QScrollArea(this);
		sa_->setBackgroundRole(QPalette::Dark);
		sa_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		sa_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		sa_->setWidget(fr_);
		sa_->ensureWidgetVisible(fr_, 0, 0);

		hl_ = new QHBoxLayout(this);
		hl_->addWidget(sa_);
		hl_->setMargin(0);
		this->setLayout(hl_);
*/

/*
		vl_ = new QVBoxLayout(this);
		this->setLayout(vl_);
		vl_->addStretch(1);
		vl_->setDirection(QBoxLayout::BottomToTop);
		sa_ = new QScrollArea(NULL);
*/
	}

	QRect myGeometry(){
		return geometry();
	}

	void setMyGeometry(const QRect &rect){
		if(sa_){
			sa_->resize(250, rect.height());
		}
		QDialog::setGeometry(rect);
	}
	void setMyGeometry(int x, int y, int width, int height){
		if(sa_)
			sa_->resize(250, height);
		QDialog::setGeometry(x, y, width, height);
	}


	void setAnimes(QPropertyAnimation* openAnime, QPropertyAnimation* closeAnime){ openAnime_ = openAnime; closeAnime_ = closeAnime;}
	void append(QString msg, int level){
		QHBoxLayout *layout;
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

		// OKAY, from here on in this is a severe hack. Why, because ScrollArea is a biotch
		//  Turns out that when I just tried to make a scroll area in the constructor and
		//  add the frame it would never update. So then, if I put the scrollArea stuff in
		//  here (which would be a hack anyway) it somehow brutalizes succesive drawings of
		//  the frame (the message layouts get squeezed until you can't read them). This is
		//  likely the MOST WASTEFUL algorithm possible, but it's the only one I could figure
		//  out that will draw successive messages properly ... I hope that someone can come
		//  around and fix it, cause it's pissing me off.
		// That, and all of this should be migrated to the cpp, cause the header is now a
		//  mile long ... my bust.
		mems_.append(QPair<QString, QString>(msg, iconPixmap));

		fr_ = new QFrame(this, 0);
		vl_ = new QVBoxLayout(fr_);
		QLabel *localIcon;
		QLabel *text;
		for(int x = 0; x < mems_.count(); x++){
			layout = new QHBoxLayout(this);
			localIcon = new QLabel("trying");
			localIcon->setPixmap(QPixmap(mems_.at(x).second));
			layout->addWidget(localIcon);
			setFixedWidth(250);
			text = new QLabel(mems_.at(x).first);
			layout->addWidget(text);
			layout->addStretch(1);
			layout->addStrut(32);
			layout->setMargin(0);
			vl_->addLayout(layout);
		}
		fr_->setLayout(vl_);
		vl_->addStretch(1);
		vl_->setDirection(QBoxLayout::BottomToTop);
		
		sa_ = new QScrollArea(this);
		sa_->setFixedWidth(250);
		sa_->setBackgroundRole(QPalette::Dark);
		sa_->setAlignment(Qt::AlignLeft);
		sa_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
			sa_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		QFrame *lfr_ = fr_;
		sa_->setWidget(lfr_);

		hl_ = new QHBoxLayout(this);
		hl_->addWidget(sa_);
		hl_->setMargin(0);
		this->setLayout(hl_);

	}
	void setStartHeight(int startHeight){ startHeight_ = startHeight;}

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
			int targetY = geometry().y() + (initVal2-startHeight_);
			closeAnime_->setStartValue(QRect(geometry().x(), initVal, geometry().width(), initVal2));
			closeAnime_->setEndValue(QRect(geometry().x(), targetY, geometry().width(), startHeight_));
			connect(closeAnime_, SIGNAL(finished()), this, SLOT(onAnimationFinished()));
			closeAnime_->start();
		}
		e->ignore();
	}

	QPropertyAnimation *openAnime_;
	QPropertyAnimation *closeAnime_;
	QVBoxLayout *vl_;
	QList<QPair<QString, QString> > mems_;
	QFrame *fr_;
	QScrollArea *sa_;
	QHBoxLayout *hl_;
	int startHeight_;
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
			openAnime = new QPropertyAnimation(popup_, "myGeometry");
			openAnime->setDuration(1000);
			openAnime->setEasingCurve(QEasingCurve::OutInSine);
			QPropertyAnimation *closeAnime = new QPropertyAnimation(popup_, "myGeometry");
			closeAnime->setDuration(1000);
			closeAnime->setEasingCurve(QEasingCurve::OutInSine);
			popup_->setAnimes(openAnime, closeAnime);
		}
		if(openAnime->state() == QAbstractAnimation::Running)
			return;
		QPoint d = QPoint(-popup_->geometry().width()/2 + geometry().width()/2, geometry().height());
		popup_->move( mapToGlobal(d) );
		int initVal = popup_->geometry().y();
		qDebug() << "Suggested " << popup_->sizeHint().height() << " " << popup_->minimumSizeHint().height();
		int startHeight = popup_->minimumSizeHint().height();
		popup_->setStartHeight(startHeight);
		openAnime->setStartValue(QRect(popup_->geometry().x(), initVal-startHeight, popup_->geometry().width(), startHeight));
		openAnime->setEndValue(QRect(popup_->geometry().x(), initVal-AMSTATUSVIEW_LOG_HEIGHT, popup_->geometry().width(), AMSTATUSVIEW_LOG_HEIGHT));
		popup_->show();
		openAnime->start();
		e->ignore();
	}

};

#endif // AMSTATUSVIEW_H
