#ifndef AMSAMPLEPOSITIONVIEW_H
#define AMSAMPLEPOSITIONVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QUrl>
#include <QGroupBox>

#include "AMSamplePlateView.h"
#include "AMSampleManipulatorView.h"
#include "CamWidget.h"

class AMSamplePositionView : public QWidget
{
Q_OBJECT
public:
	explicit AMSamplePositionView(QWidget *parent = 0, AMSampleManipulatorView *manipulatorView = 0);

signals:

public slots:

protected:
	CamWidget *cam_;
	AMSamplePlateView *plateView_;
	AMSampleManipulatorView *manipulatorView_;

	AMControlSet *manipulator_;

	QGridLayout *gl_;
};

class SGMSamplePositionView : public AMSamplePositionView
{
Q_OBJECT
public:
	SGMSamplePositionView(QWidget *parent = 0);
};

#endif // AMSAMPLEPOSITIONVIEW_H
