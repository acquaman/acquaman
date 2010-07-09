#ifndef AMBEAMLINEACTIONITEM_H
#define AMBEAMLINEACTIONITEM_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class AMBeamlineActionItem : public QObject
{
Q_OBJECT
public:
	explicit AMBeamlineActionItem(QObject *parent = 0);
	bool hasFeedback() { return hasFeedback_; }
	AMBeamlineActionItem* previous() const { return previous_;}
	AMBeamlineActionItem* next() const { return next_;}

signals:
	void started();
	void ready(bool ready);
	void succeeded();
	void failed(int explanation);

public slots:
	virtual void start() = 0;

protected:
	bool start_;
	bool succeed_;
	bool fail_;
	bool hasFeedback_;

	AMBeamlineActionItem *previous_;
	AMBeamlineActionItem *next_;
};

class AMBeamlineActionItemView : public QWidget
{
	Q_OBJECT
public:
	AMBeamlineActionItemView(AMBeamlineActionItem *item, QWidget *parent = 0);

protected:
	AMBeamlineActionItem *item_;
	QHBoxLayout *hl_;
	QLabel *message_;
	QPushButton *light_;
	QPushButton *proceed_;
};

#endif // AMBEAMLINEACTIONITEM_H
