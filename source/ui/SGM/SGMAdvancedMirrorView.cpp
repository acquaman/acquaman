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


#include "SGMAdvancedMirrorView.h"

#include <QBoxLayout>
#include <QDebug>
#include "beamline/SGM/SGMBeamline.h"

 SGMAdvancedMirrorView::~SGMAdvancedMirrorView(){}
SGMAdvancedMirrorView::SGMAdvancedMirrorView(QWidget *parent) :
	QWidget(parent)
{
	Mirror2_ = new QGroupBox("Mirror 2");
	Mirror3_ = new QGroupBox("Mirror 3");
	m2VerticalUpstream_ = new QLabel("M2 Vertical upstream");

	m2VerticalUpstreamStepControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m2VerticalUpstreamStep()));
	m2VerticalUpstreamEncoderControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m2VerticalUpstreamEncoder()));
	m2VerticalDownstream_ = new QLabel("M2 Vertical downstream");

	m2VerticalDownstreamStepControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m2VerticalDownstreamStep()));
	m2VerticalDownstreamEncoderControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m2VerticalDownstreamEncoder()));
	m2HorizontalUpstream_ = new QLabel("M2 Horizontal upstream");

	m2HorizontalUpstreamStepControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m2HorizontalUpstreamStep()));
	m2HorizontalUpstreamEncoderControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m2HorizontalUpstreamEncoder()));
	m2HorizontalDownstream_ = new QLabel("M2 Horizontal downstream");

	m2HorizontalDownstreamStepControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m2HorizontalDownstreamStep()));
	m2HorizontalDownstreamEncoderControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m2HorizontalDownstreamEncoder()));
	m2Rotational_ = new QLabel("M2 Rotational");

	m2RotationalStepControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m2RotationalStep()));
	m2RotationalEncoderControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m2RotationalEncoder()));
	m3VerticalUpstream_ = new QLabel("M3 Vertical upstream");

	m3VerticalUpstreamStepControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m3VerticalUpstreamStep()));
	m3VerticalUpstreamEncoderControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m3VerticalUpstreamEncoder()));
	m3VerticalDownstream_ = new QLabel("M3 Vertical downstream");

	m3VerticalDownstreamStepControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m3VerticalDownstreamStep()));
	m3VerticalDownstreamEncoderControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m3VerticalDownstreamEncoder()));
	m3HorizontalUpstream_ = new QLabel("M3 Horizontal upstream");

	m3HorizontalUpstreamStepControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m3HorizontalUpstreamStep()));
	m3HorizontalUpstreamEncoderControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m3HorizontalUpstreamEncoder()));
	m3HorizontalDownstream_ = new QLabel("M3 Horizontal downstream");

	m3HorizontalDownstreamStepControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m3HorizontalDownstreamStep()));
	m3HorizontalDownstreamEncoderControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m3HorizontalDownstreamEncoder()));
	m3Rotational_ = new QLabel("M3 Rotational");

	m3RotationalStepControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m3RotationalStep()));
	m3RotationalEncoderControlEditor_ = new AMControlEditor(qobject_cast<AMReadOnlyPVControl*>(SGMBeamline::sgm()->m3RotationalEncoder()));

	QVBoxLayout *verticalLayoutLeft = new QVBoxLayout();
	Mirror2_->setLayout(verticalLayoutLeft);

	QHBoxLayout *horizontalLayout2 = new QHBoxLayout();
	horizontalLayout2->addWidget(m2VerticalUpstream_);
	verticalLayoutLeft->addLayout(horizontalLayout2);



	QVBoxLayout *verticalLayoutHL22 = new QVBoxLayout();
	verticalLayoutHL22->addWidget(m2VerticalUpstreamStepControlEditor_);
	horizontalLayout2->addLayout(verticalLayoutHL22);

	QVBoxLayout *verticalLayoutHL23 = new QVBoxLayout();
	verticalLayoutHL23->addWidget(m2VerticalUpstreamEncoderControlEditor_);
	horizontalLayout2->addLayout(verticalLayoutHL23);

	QHBoxLayout *horizontalLayout3 = new QHBoxLayout();
	horizontalLayout3->addWidget(m2VerticalDownstream_);
	verticalLayoutLeft->addLayout(horizontalLayout3);


	QVBoxLayout *verticalLayoutHL32 = new QVBoxLayout();
	verticalLayoutHL32->addWidget(m2VerticalDownstreamStepControlEditor_);
	horizontalLayout3->addLayout(verticalLayoutHL32);

	QVBoxLayout *verticalLayoutHL33 = new QVBoxLayout();
	verticalLayoutHL33->addWidget(m2VerticalDownstreamEncoderControlEditor_);
	horizontalLayout3->addLayout(verticalLayoutHL33);

	QHBoxLayout *horizontalLayout4 = new QHBoxLayout();
	horizontalLayout4->addWidget(m2HorizontalUpstream_);
	verticalLayoutLeft->addLayout(horizontalLayout4);

	QVBoxLayout *verticalLayoutHL42 = new QVBoxLayout();
	verticalLayoutHL42->addWidget(m2HorizontalUpstreamStepControlEditor_);
	horizontalLayout4->addLayout(verticalLayoutHL42);

	QVBoxLayout *verticalLayoutHL43 = new QVBoxLayout();
	verticalLayoutHL43->addWidget(m2HorizontalUpstreamEncoderControlEditor_);
	horizontalLayout4->addLayout(verticalLayoutHL43);

	QHBoxLayout *horizontalLayout5 = new QHBoxLayout();
	horizontalLayout5->addWidget(m2HorizontalDownstream_);
	verticalLayoutLeft->addLayout(horizontalLayout5);

	QVBoxLayout *verticalLayoutHL52 = new QVBoxLayout();
	verticalLayoutHL52->addWidget(m2HorizontalDownstreamStepControlEditor_);
	horizontalLayout5->addLayout(verticalLayoutHL52);

	QVBoxLayout *verticalLayoutHL53 = new QVBoxLayout();
	verticalLayoutHL53->addWidget(m2HorizontalDownstreamEncoderControlEditor_);
	horizontalLayout5->addLayout(verticalLayoutHL53);

	QHBoxLayout *horizontalLayout6 = new QHBoxLayout();
	horizontalLayout6->addWidget(m2Rotational_);
	verticalLayoutLeft->addLayout(horizontalLayout6);



	QVBoxLayout *verticalLayoutHL62 = new QVBoxLayout();
	verticalLayoutHL62->addWidget(m2RotationalStepControlEditor_);
	horizontalLayout6->addLayout(verticalLayoutHL62);

	QVBoxLayout *verticalLayoutHL63 = new QVBoxLayout();
	verticalLayoutHL63->addWidget(m2RotationalEncoderControlEditor_);
	horizontalLayout6->addLayout(verticalLayoutHL63);




	QVBoxLayout *verticalLayoutRight = new QVBoxLayout();
	Mirror3_->setLayout(verticalLayoutRight);

	QHBoxLayout *horizontalLayout7 = new QHBoxLayout();
	horizontalLayout7->addWidget(m3VerticalUpstream_);
	verticalLayoutRight->addLayout(horizontalLayout7);



	QVBoxLayout *verticalLayoutHL72 = new QVBoxLayout();
	verticalLayoutHL72->addWidget(m3VerticalUpstreamStepControlEditor_);
	horizontalLayout7->addLayout(verticalLayoutHL72);

	QVBoxLayout *verticalLayoutHL73 = new QVBoxLayout();
	verticalLayoutHL73->addWidget(m3VerticalUpstreamEncoderControlEditor_);
	horizontalLayout7->addLayout(verticalLayoutHL73);

	QHBoxLayout *horizontalLayout8 = new QHBoxLayout();
	horizontalLayout8->addWidget(m3VerticalDownstream_);
	verticalLayoutRight->addLayout(horizontalLayout8);



	QVBoxLayout *verticalLayoutHL82 = new QVBoxLayout();
	verticalLayoutHL82->addWidget(m3VerticalDownstreamStepControlEditor_);
	horizontalLayout8->addLayout(verticalLayoutHL82);

	QVBoxLayout *verticalLayoutHL83 = new QVBoxLayout();
	verticalLayoutHL83->addWidget(m3VerticalDownstreamEncoderControlEditor_);
	horizontalLayout8->addLayout(verticalLayoutHL83);

	QHBoxLayout *horizontalLayout9 = new QHBoxLayout();
	horizontalLayout9->addWidget(m3HorizontalUpstream_);
	verticalLayoutRight->addLayout(horizontalLayout9);



	QVBoxLayout *verticalLayoutHL92 = new QVBoxLayout();
	verticalLayoutHL92->addWidget(m3HorizontalUpstreamStepControlEditor_);
	horizontalLayout9->addLayout(verticalLayoutHL92);

	QVBoxLayout *verticalLayoutHL93 = new QVBoxLayout();
	verticalLayoutHL93->addWidget(m3HorizontalUpstreamEncoderControlEditor_);
	horizontalLayout9->addLayout(verticalLayoutHL93);

	QHBoxLayout *horizontalLayout10 = new QHBoxLayout();
	horizontalLayout10->addWidget(m3HorizontalDownstream_);
	verticalLayoutRight->addLayout(horizontalLayout10);


	QVBoxLayout *verticalLayoutHL102 = new QVBoxLayout();
	verticalLayoutHL102->addWidget(m3HorizontalDownstreamStepControlEditor_);
	horizontalLayout10->addLayout(verticalLayoutHL102);

	QVBoxLayout *verticalLayoutHL103 = new QVBoxLayout();
	verticalLayoutHL103->addWidget(m3HorizontalDownstreamEncoderControlEditor_);
	horizontalLayout10->addLayout(verticalLayoutHL103);

	QHBoxLayout *horizontalLayout11 = new QHBoxLayout();
	horizontalLayout11->addWidget(m3Rotational_);
	verticalLayoutRight->addLayout(horizontalLayout11);


	QVBoxLayout *verticalLayoutHL112 = new QVBoxLayout();
	verticalLayoutHL112->addWidget(m3RotationalStepControlEditor_);
	horizontalLayout11->addLayout(verticalLayoutHL112);

	QVBoxLayout *verticalLayoutHL113 = new QVBoxLayout();
	verticalLayoutHL113->addWidget(m3RotationalEncoderControlEditor_);
	horizontalLayout11->addLayout(verticalLayoutHL113);

	QHBoxLayout *horizontalLayout = new QHBoxLayout();
	horizontalLayout->addWidget(Mirror2_);
	horizontalLayout->addWidget(Mirror3_);
	setLayout(horizontalLayout);
}
