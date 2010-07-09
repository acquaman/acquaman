#ifndef SGMSAMPLETRANSFERVIEW_H
#define SGMSAMPLETRANSFERVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <beamline/SGMBeamline.h>

class SGMSampleTransferView : public QWidget
{
Q_OBJECT
public:
	explicit SGMSampleTransferView(QWidget *parent = 0);

signals:

public slots:
	void setAction1Success();
	void setAction2Success();
	void setAction3Success();
	void setAction4Success();

protected:
	QGridLayout *mainLayout_;
	QLabel *action1Label_;
	QLabel *action2Label_;
	QLabel *action3Label_;
	QLabel *action4Label_;
	QPushButton *action1Proceed_;
	QPushButton *action2Proceed_;
	QPushButton *action3Proceed_;
	QPushButton *action4Proceed_;
	QPushButton *action1Light_;
	QPushButton *action2Light_;
	QPushButton *action3Light_;
	QPushButton *action4Light_;
};

#endif // SGMSAMPLETRANSFERVIEW_H
