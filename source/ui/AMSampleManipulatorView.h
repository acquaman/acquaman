#ifndef AMSAMPLEMANIPULATORVIEW_H
#define AMSAMPLEMANIPULATORVIEW_H

#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QIcon>
#include "beamline/AMControlSet.h"
#include "beamline/SGMBeamline.h"
#include "NumericControl.h"

class AMSampleManipulatorView : public QGroupBox
{
Q_OBJECT
public:
	explicit AMSampleManipulatorView(QString title = "Manipulator", AMControlSet *manipulator = 0, QWidget *parent = 0);

signals:

public slots:

protected:
	AMControlSet *manipulator_;
};

class SGMSampleManipulatorView : public AMSampleManipulatorView
{
Q_OBJECT
public:
	SGMSampleManipulatorView(QWidget *parent = 0);

protected slots:
	void onMUpButtonPressed();
	void onMUpButtonReleased();
	void onMDownButtonPressed();
	void onMDownButtonReleased();
	void onMInboardButtonPressed();
	void onMInboardButtonReleased();
	void onMOutboardButtonPressed();
	void onMOutboardButtonReleased();
	void onMUpstreamButtonPressed();
	void onMUpstreamButtonReleased();
	void onMDownstreamButtonPressed();
	void onMDownstreamButtonReleased();

protected:
	QPushButton *mUpButton_;
	QPushButton *mDownButton_;
	QPushButton *mInboardButton_;
	QPushButton *mOutboardButton_;
	QPushButton *mUpstreamButton_;
	QPushButton *mDownstreamButton_;

	AMControl *mVerticalCtrl_;
	AMControl *mHorizontalCtrl_;
	AMControl *mInPlaneCtrl_;

	NumericControl *mVerticalNC_;
	NumericControl *mHorizontalNC_;
	NumericControl *mInPlaneNC_;

	QIcon upIcon_, downIcon_, inboardIcon_, outboardIcon_, upstreamIcon_, downstreamIcon_;

	QGridLayout *gl_;
};

#endif // AMSAMPLEMANIPULATORVIEW_H
