#ifndef SGMSAMPLETRANSFERVIEW_H
#define SGMSAMPLETRANSFERVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <beamline/SGMBeamline.h>

class SGMSampleTransferPaneView;
class SGMSampleTransferProceduresView;

class SGMSampleTransferView : public QWidget
{
Q_OBJECT
public:
	explicit SGMSampleTransferView(QWidget *parent = 0);

signals:

public slots:

private slots:
	void drawMain();
	void drawLoadlockOut();
	void drawLoadlockIn();

protected:
	QPushButton *loadlockOutButton_;
	QPushButton *loadlockInButton_;
	SGMSampleTransferPaneView *loadlockOut_;
	SGMSampleTransferPaneView *loadlockIn_;
	SGMSampleTransferProceduresView *transferBox_;
	QStackedLayout *mainLayout_;
};

class SGMSampleTransferProceduresView : public QGroupBox
{
Q_OBJECT
public:
	SGMSampleTransferProceduresView(const QString &title, QList<QPushButton*> procedureButtons, QWidget *parent = 0);

protected:
	QVBoxLayout *vl_;
	QList<QPushButton*> procedureButtons_;
	QGridLayout *mainLayout_;
};

class SGMSampleTransferPaneView : public QGroupBox
{
Q_OBJECT
public:
	explicit SGMSampleTransferPaneView(QList<AMBeamlineActionItem*> items, const QString &title, QWidget *parent = 0);

signals:
	void completed();

public slots:
	virtual void startPane(){ initialize(); firstItem_->start();}

private slots:
	virtual void initialize();
	virtual void prepareCompletion();

protected:
	QVBoxLayout *vl_;
	QList<AMBeamlineActionItemView*> itemViews_;
	AMBeamlineActionItem* firstItem_;
	QLabel *completeLabel_;
	QPushButton *completeButton_;
	QGridLayout *mainLayout_;
};

#endif // SGMSAMPLETRANSFERVIEW_H
