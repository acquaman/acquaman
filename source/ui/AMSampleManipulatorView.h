/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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

	AMControlSet *manipulator();

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
	void onMCWButtonPressed();
	void onMCWButtonReleased();
	void onMCCWButtonPressed();
	void onMCCWButtonReleased();

protected:
	QPushButton *mUpButton_;
	QPushButton *mDownButton_;
	QPushButton *mInboardButton_;
	QPushButton *mOutboardButton_;
	QPushButton *mUpstreamButton_;
	QPushButton *mDownstreamButton_;
	QPushButton *mCWButton_;
	QPushButton *mCCWButton_;

	AMControl *mVerticalCtrl_;
	AMControl *mHorizontalCtrl_;
	AMControl *mInPlaneCtrl_;
	AMControl *mRotationCtrl_;

	NumericControl *mVerticalNC_;
	NumericControl *mHorizontalNC_;
	NumericControl *mInPlaneNC_;
	NumericControl *mRotationNC_;

	QIcon upIcon_, downIcon_, inboardIcon_, outboardIcon_, upstreamIcon_, downstreamIcon_, cwIcon_, ccwIcon_;

	QGridLayout *gl_;
};

#endif // AMSAMPLEMANIPULATORVIEW_H
