#ifndef BIOXASBEAMLINETABWIDGET_H
#define BIOXASBEAMLINETABWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

//#include <QPainter>
//#include <QPaintEvent>
//#include <QStyle>
//#include <QStyleOption>

#include "../BioXASBeamline.h"
#include "../MotorModel.h"

class BioXASBeamlineTabWidget : public QWidget
{
    Q_OBJECT
public:
	explicit BioXASBeamlineTabWidget(BioXASBeamlineDef::BioXASBeamLineID beamlineId, QWidget *parent = 0);

protected:
	BioXASBeamlineDef::BioXASBeamLineID beamlineId_;

	MotorModel *bioXASMotorModel_;

	QVBoxLayout *filterMotorVBox_;
	QVBoxLayout *m1MotorVBox_;
	QVBoxLayout *maskMotorVBox_;
	QVBoxLayout *monoMotorVBox_;
	QVBoxLayout *m2MotorVBox_;

	void setupUiLayout();
	void setupMotors(BioXASBeamlineDef::BioXASMotorType motorType, QVBoxLayout *pvLayoutBox);

//	void paintEvent(QPaintEvent *)
//	{
//		QStyleOption opt;
//		opt.init(this);
//		QPainter p(this);
//		style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//	}

private:
	QString getBeamlineImage(BioXASBeamlineDef::BioXASBeamLineID beamlineId);
	QVBoxLayout * createMotorGroupBox(QString title, QHBoxLayout * parentContainer, int expectedHeight=600, int expectedWidth=320);

};

#endif // BIOXASBEAMLINETABWIDGET_H
