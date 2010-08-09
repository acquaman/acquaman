#ifndef AMBEAMLINEACTIONITEM_H
#define AMBEAMLINEACTIONITEM_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

class AMBeamlineActionItem : public QObject
{
Q_OBJECT
public:
	explicit AMBeamlineActionItem(QString message = "", QObject *parent = 0);
	bool hasFeedback() { return hasFeedback_; }
	bool needsInput() { return needsInput_; }
	AMBeamlineActionItem* previous() const { return previous_;}
	AMBeamlineActionItem* next() const { return next_;}
	QString message() const { return message_; }

signals:
	void started();
	void ready(bool ready);
	void succeeded();
	void failed(int explanation);

public slots:
	virtual void start(){ emit started(); }
	bool setPrevious(AMBeamlineActionItem* previous){ previous_ = previous; return true;
//#warning("DAVID! This warning is... RAW!");
		}
	bool setNext(AMBeamlineActionItem* next){ next_ = next; return true;
//			#warning("DAVID! This warning is... RAW!");
		}

protected:
	bool start_;
	bool succeed_;
	bool fail_;
	bool hasFeedback_;
	bool needsInput_;

	QString message_;
	AMBeamlineActionItem *previous_;
	AMBeamlineActionItem *next_;
};

class AMBeamlineActionItemView : public QWidget
{
	Q_OBJECT
public:
	AMBeamlineActionItemView(AMBeamlineActionItem *item, QWidget *parent = 0);
	int messageHint(){ return message_->sizeHint().width(); }
	int lightHint(){ return light_->sizeHint().width(); }
	int proceedHint() { return proceed_->sizeHint().width(); }

public slots:
	virtual void initializeView();
	virtual void onStart();
	virtual void onReady(bool ready);
	virtual void fixMessageSize(int width);
	virtual void fixLightSize(int width);
	virtual void fixProceedSize(int width);

protected:
	AMBeamlineActionItem *item_;
	QHBoxLayout *hl_;
	QLabel *message_;
	QPushButton *light_;
	QPushButton *proceed_;
};

#endif // AMBEAMLINEACTIONITEM_H
