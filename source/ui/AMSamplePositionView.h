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


#ifndef AMSAMPLEPOSITIONVIEW_H
#define AMSAMPLEPOSITIONVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QUrl>
#include <QGroupBox>

#include "AMSamplePlateView.h"
#include "AMSampleManipulatorView.h"
#include "ui/AMBeamlineCameraWidget.h"

class AMSamplePositionView : public QWidget
{
Q_OBJECT
public:
	explicit AMSamplePositionView(QWidget *parent = 0, AMSampleManipulatorView *manipulatorView = 0);

signals:

public slots:

protected:
	AMBeamlineCameraWidget *cam_;
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
