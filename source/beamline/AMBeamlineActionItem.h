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

class AMBeamlineActionItemStateFlag : public QObject
{
Q_OBJECT
public:
	AMBeamlineActionItemStateFlag(bool initialState = false, QObject *parent = 0);
	virtual bool state() const;

signals:
	void stateChanged(bool changedTo);

public slots:
	void setState(bool state);

protected:
	bool state_;
};

class AMBeamlineActionItem : public QObject
{
Q_OBJECT
public:
	explicit AMBeamlineActionItem(QObject *parent = 0);
	bool isInitialized() const;
	bool isReinitialized() const;
	bool isReady() const;
	bool hasStarted() const;
	virtual bool isRunning() const;
	bool hasSucceeded() const;
	bool hasFailed() const;
	bool hasFinished() const;
	AMBeamlineActionItem* previous() const;
	AMBeamlineActionItem* next() const;
	virtual QString type() const;

signals:
	void initialized();
	void ready(bool ready);
	void started();
	void succeeded();
	void failed(int explanation);
	void finished();

public slots:
	virtual void start() = 0;//Pure virtual. Sub-classes need to implement and they better set start_ = true at some point and cause start() to be emitted
	//Calling reset always set reintialized to true
	virtual void reset(bool delayInitialize = false);//NOT Pure virtual. Sub-classes better call this at the end of their reset (AMBeamlineActionItem::reset) or call initialize themselves
	virtual void cleanup() = 0;
	bool setPrevious(AMBeamlineActionItem* previous);
	bool setNext(AMBeamlineActionItem* next);

protected slots:
	// Interface to internal state. If sub-classes want to change something, call these.
	// They will cause the corresponding signal to be emitted if the state changed (in the right direction)
	// They will also cause initialized to become false if they change from their initial state
	void setReady(bool isReady);
	void setStarted(bool isStarted);
	void setSucceeded(bool isSucceeded);
	void setFailed(bool isFailed, int explanation = -1);
	void setFinished(bool isFinished);

	virtual void initialize();//NOT Pure virtual. Sub-classes better call this at the end of their initialize (AMBeamlineActionItem::initialize)

protected:
	AMBeamlineActionItem(bool delayInitialize = false, QObject *parent = 0);

	AMBeamlineActionItem *previous_;
	AMBeamlineActionItem *next_;

private slots:
	void dirtyInitialized();

private:
	QString type_;
	AMBeamlineActionItemStateFlag initialized_;//Only true when initialized and nothing else has happened. Setting any flag below
	AMBeamlineActionItemStateFlag reinitialized_;//Stays false until reset is called
	AMBeamlineActionItemStateFlag ready_;
	AMBeamlineActionItemStateFlag started_;
	AMBeamlineActionItemStateFlag succeeded_;
	AMBeamlineActionItemStateFlag failed_;
	AMBeamlineActionItemStateFlag finished_;
};

class AMBeamlineActionView : public QFrame
{
	Q_OBJECT
public:
	AMBeamlineActionView(AMBeamlineActionItem *action, int index = 0, QWidget *parent = 0);

	int index() const { return index_;}
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
	void stopRequested(AMBeamlineActionItem *action);

protected slots:
	virtual void onInfoChanged() = 0;
	virtual void onStopCancelButtonClicked() = 0;
	virtual void onPlayPauseButtonClicked() = 0;

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

class AM1BeamlineActionItem : public QObject
{
Q_OBJECT
public:
	explicit AM1BeamlineActionItem(QString message = "", QObject *parent = 0);
	bool isRunning() { return running_; }
	bool hasStarted() { return started_; }
	bool hasSucceeded() { return succeeded_; }
	bool hasFailed() { return failed_; }
	bool hasFeedback() { return hasFeedback_; }
	bool needsInput() { return needsInput_; }
	AM1BeamlineActionItem* previous() const { return previous_;}
	AM1BeamlineActionItem* next() const { return next_;}
	QString message() const { return message_; }
	virtual QString type() const { return type_; }

signals:
	void started();
	void ready(bool ready);
	void succeeded();
	void failed(int explanation);

public slots:
	virtual void start(){ running_ = true; started_ = true; emit started(); }
	bool setPrevious(AM1BeamlineActionItem* previous);
	bool setNext(AM1BeamlineActionItem* next);
protected:
	bool started_;
	bool succeeded_;
	bool failed_;
	bool running_;
	bool hasFeedback_;
	bool needsInput_;

	QString message_;
	AM1BeamlineActionItem *previous_;
	AM1BeamlineActionItem *next_;

private:
	QString type_;
};

class AMBeamlineActionItemView : public QWidget
{
	Q_OBJECT
public:
	AMBeamlineActionItemView(AM1BeamlineActionItem *item, QWidget *parent = 0);
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
	AM1BeamlineActionItem *item_;
	QHBoxLayout *hl_;
	QLabel *message_;
	QPushButton *light_;
	QPushButton *proceed_;
};

#endif // AMBEAMLINEACTIONITEM_H
