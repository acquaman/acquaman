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
	explicit AMBeamlineActionItem(QObject *parent = 0);
	bool hasFeedback() { return hasFeedback_; }
	AMBeamlineActionItem* previous() const { return previous_;}
	AMBeamlineActionItem* next() const { return next_;}
	QString message() const { return message_; }

signals:
	void started();
	void ready(bool ready);
	void succeeded();
	void failed(int explanation);

public slots:
	virtual void start() = 0;
	bool setPrevious(AMBeamlineActionItem* previous){ previous_ = previous; }
	bool setNext(AMBeamlineActionItem* next){ next_ = next; }

protected:
	bool start_;
	bool succeed_;
	bool fail_;
	bool hasFeedback_;

	QString message_;
	AMBeamlineActionItem *previous_;
	AMBeamlineActionItem *next_;
};

class AMBeamlineActionItemView : public QWidget
{
	Q_OBJECT
public:
	AMBeamlineActionItemView(AMBeamlineActionItem *item, QWidget *parent = 0);

public slots:
	virtual void onStart();
	virtual void onReady(bool ready);

protected:
	AMBeamlineActionItem *item_;
	QHBoxLayout *hl_;
	QLabel *message_;
	QPushButton *light_;
	QPushButton *proceed_;
};

#endif // AMBEAMLINEACTIONITEM_H
