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


#include "AMSamplePositionView.h"

AMSamplePositionView::AMSamplePositionView(AMSampleManipulatorView *manipulatorView, const QUrl& sampleCameraUrl, AMSamplePlate* samplePlate, QWidget *parent) :
	QWidget(parent)
{
	// cam_ = new AMBeamlineCameraWidget("Sample Camera", sampleCameraUrl);
	// cam_->addSource("Camera 2", source2);

	plateView_ = new AMSamplePlateView(samplePlate);
	plateView_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

	/* PREVIOUSLY on the OC:
	if(!manipulatorView)
		manipulatorView_ = new AMSampleManipulatorView();
	else
		manipulatorView_ = manipulatorView;
	if(manipulatorView_->manipulator()){
		manipulator_ = manipulatorView_->manipulator();
		plateView_->setManipulator(manipulator_);
	}*/

	// Now assuming that all passed-in objects must be valid: (made it not an option to default to 0)
	manipulatorView_ = manipulatorView;
	// Access the sample manipulator through the manipulatorView, and set the plateView_'s manipulator to use the same.
	plateView_->setManipulator(manipulatorView_->manipulator());

	manipulatorView_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);


	gl_ = new QGridLayout();
	//gl_->addWidget(cam_, 0, 0, 3, 1, Qt::AlignLeft);
	gl_->addWidget(plateView_, 0, 1, 5, 1, Qt::AlignLeft);
	gl_->addWidget(manipulatorView_, 3, 0, 2, 1, Qt::AlignLeft);

	setLayout(gl_);
}

