/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef SGMADVANCEDMIRRORVIEW_H
#define SGMADVANCEDMIRRORVIEW_H

#include <QWidget>
#include <QLabel>
#include <QGroupBox>

#include "ui/beamline/AMControlEditor.h"

class SGMAdvancedMirrorView : public QWidget
{
Q_OBJECT
public:
 	virtual ~SGMAdvancedMirrorView();
	SGMAdvancedMirrorView(QWidget *parent = 0);

protected:
	/// The mirror 2 label
	QGroupBox *Mirror2_;
	/// The mirror 3 label
	QGroupBox *Mirror3_;
	/// The vertical upstream on mirror 2 label
	QLabel *m2VerticalUpstream_;

	/// Returns the vertical upstream step for mirror 2
	AMControlEditor *m2VerticalUpstreamStepControlEditor_;
	/// Returns the vertical upstream encoder for mirror 2
	AMControlEditor *m2VerticalUpstreamEncoderControlEditor_;
	/// The vertical downstream label
	QLabel *m2VerticalDownstream_;

	/// Returns the vertical downstream step for mirror 2
	AMControlEditor *m2VerticalDownstreamStepControlEditor_;
	/// Returns the vertical downstream encoder for mirror 2
	AMControlEditor *m2VerticalDownstreamEncoderControlEditor_;
	/// The horizontal upstream on mirror 2 label
	QLabel *m2HorizontalUpstream_;

	/// Returns the horizontal upstream step for mirror 2
	AMControlEditor *m2HorizontalUpstreamStepControlEditor_;
	/// Returns the horizontal upstream encoder for mirror 2
	AMControlEditor *m2HorizontalUpstreamEncoderControlEditor_;
	/// The horizontal upstream on mirror 2 label
	QLabel *m2HorizontalDownstream_;

	/// Returns the horizontal downstream step for mirror 2
	AMControlEditor *m2HorizontalDownstreamStepControlEditor_;
	/// Returns the horizontal downstream encoder for mirror 2
	AMControlEditor *m2HorizontalDownstreamEncoderControlEditor_;
	/// The rotational on mirror 2 label
	QLabel *m2Rotational_;

	/// Returns the rotational step for mirror 2
	AMControlEditor *m2RotationalStepControlEditor_;
	/// Returns the rotational encoder for mirror 2
	AMControlEditor *m2RotationalEncoderControlEditor_;
	/// The vertical upstream on mirror 3 label
	QLabel *m3VerticalUpstream_;

	/// Returns the vertical upstream step for mirror 3
	AMControlEditor *m3VerticalUpstreamStepControlEditor_;
	/// Retuurns the vertical upstream encoder for mirrror 3
	AMControlEditor *m3VerticalUpstreamEncoderControlEditor_;
	/// The vertical downstream on mirror 3 label
	QLabel *m3VerticalDownstream_;

	/// Returns the vertical downstream step for mirror 3
	AMControlEditor *m3VerticalDownstreamStepControlEditor_;
	/// Returns the vertical downstream step for mirror 3
	AMControlEditor *m3VerticalDownstreamEncoderControlEditor_;
	/// The horizontal upstream for mirror 3 label
	QLabel *m3HorizontalUpstream_;

	/// Returns the horizontal upstream step for mirror 3
	AMControlEditor *m3HorizontalUpstreamStepControlEditor_;
	/// Returns the horizontal downstream encoder for mirror 3
	AMControlEditor *m3HorizontalUpstreamEncoderControlEditor_;
	/// The horizontal downstream on mirror 3 label
	QLabel *m3HorizontalDownstream_;

	/// Returns the horizontal downstream step for mirror 3
	AMControlEditor *m3HorizontalDownstreamStepControlEditor_;
	/// Returns the horizontal downstream mirror encoder for mirror 3
	AMControlEditor *m3HorizontalDownstreamEncoderControlEditor_;
	/// The rotational on mirror 3 label
	QLabel *m3Rotational_;

	/// Returns the rotational step for mirror 3
	AMControlEditor *m3RotationalStepControlEditor_;
	/// Returns the rotational encoder for mirror 3
	AMControlEditor *m3RotationalEncoderControlEditor_;








};


#endif // SGMADVANCEDMIRRORVIEW_H
