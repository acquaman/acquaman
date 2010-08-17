#ifndef AMBEAMLINEACTIONITEM_H
#define AMBEAMLINEACTIONITEM_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QMouseEvent>

#define NATURAL_ACTION_VIEW_HEIGHT 62

class AMBeamlineActionItem : public QObject
{
Q_OBJECT
public:
	explicit AMBeamlineActionItem(QString message = "", QObject *parent = 0);
	bool isRunning() { return running_; }
	bool hasFeedback() { return hasFeedback_; }
	bool needsInput() { return needsInput_; }
	AMBeamlineActionItem* previous() const { return previous_;}
	AMBeamlineActionItem* next() const { return next_;}
	QString message() const { return message_; }
	virtual QString type() const { return type_; }

signals:
	void started();
	void ready(bool ready);
	void succeeded();
	void failed(int explanation);

public slots:
	virtual void start(){ running_ = true; emit started(); }
	bool setPrevious(AMBeamlineActionItem* previous);//{ previous_ = previous; return true;
//#warning("DAVID! This warning is... RAW!");
//		}
	bool setNext(AMBeamlineActionItem* next);//{ next_ = next; return true;
//			#warning("DAVID! This warning is... RAW!");
//		}

protected:
	bool start_;
	bool succeed_;
	bool fail_;
	bool running_;
	bool hasFeedback_;
	bool needsInput_;

	QString message_;
	AMBeamlineActionItem *previous_;
	AMBeamlineActionItem *next_;

private:
	QString type_;
};

class AMBeamlineActionView : public QFrame
{
	Q_OBJECT
public:
	AMBeamlineActionView(AMBeamlineActionItem *action, int index = 0, QWidget *parent = 0);

	virtual AMBeamlineActionItem* action();
	virtual QString viewType() const;

public slots:
	virtual void setIndex(int index);
	virtual void setAction(AMBeamlineActionItem *scanAction);
	virtual void defocusItem();

signals:
	void focusRequested(AMBeamlineActionItem *action);
	void removeRequested(AMBeamlineActionItem *action);
	void pauseRequested(AMBeamlineActionItem *action);
	void resumeRequested(AMBeamlineActionItem *action);
	void hideRequested(AMBeamlineActionItem *action);
	void stopRequested(AMBeamlineActionItem *action);

protected slots:
	virtual void onInfoChanged() = 0;
	virtual void onStopCancelButtonClicked() = 0;
	virtual void onPlayPauseButtonClicked() = 0;
	virtual void onHideButtonClicked() = 0;

protected:
	void mousePressEvent(QMouseEvent *event);

	virtual void updateLook();

protected:
	AMBeamlineActionItem *action_;
	int index_;
	bool inFocus_;

private:
	QString viewType_;
};

class AMBeamlineActionHiddenView : public AMBeamlineActionView
{
	Q_OBJECT
public:
	AMBeamlineActionHiddenView(AMBeamlineActionItem* first, int count = 1, QWidget *parent = 0);
	~AMBeamlineActionHiddenView();

	virtual QString viewType() const;
	virtual int count() const;

public slots:
	void setFirst(AMBeamlineActionItem *first);
	void setCount(int count);

signals:
	void expandRequested(AMBeamlineActionItem* action);

protected slots:
	virtual void onInfoChanged();
	virtual void onStopCancelButtonClicked();
	virtual void onPlayPauseButtonClicked();
	virtual void onHideButtonClicked();

	virtual void onExpandButtonClicked();

protected:
	int count_;
	QLabel *infoLabel_;
	QPushButton *expandButton_;
	QHBoxLayout *hl_;

private:
	QString viewType_;
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
