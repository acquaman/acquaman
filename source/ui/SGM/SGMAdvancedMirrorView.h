#ifndef SGMADVANCEDMIRRORVIEW_H
#define SGMADVANCEDMIRRORVIEW_H

#include <QWidget>
#include <QLabel>

#include "ui/beamline/AMControlEditor.h"

class SGMAdvancedMirrorView : public QWidget
{
Q_OBJECT
public:
	SGMAdvancedMirrorView(QWidget *parent = 0);

protected:
	QLabel *testLabel_;
	QLabel *testLabel2_;
	QLabel *testLabel3_;
	QLabel *testLabel4_;
	AMControlEditor *m2VerticalUpstreamStepControlEditor_;
	AMControlEditor *m2VerticalUpstreamEncoderControlEditor_;
	QLabel *testLabel7_;
	QLabel *testLabel8_;
	AMControlEditor *m2VerticalDownstreamStepControlEditor_;
	AMControlEditor *m2VerticalDownstreamEncoderControlEditor_;
	QLabel *testLabel11_;
	QLabel *testLabel12_;
	AMControlEditor *m2HorizontalUpstreamStepControlEditor_;
	AMControlEditor *m2HorizontalUpstreamEncoderControlEditor_;
	QLabel *testLabel15_;
	QLabel *testLabel16_;
	AMControlEditor *m2HorizontalDownstreamStepControlEditor_;
	AMControlEditor *m2HorizontalDownstreamEncoderControlEditor_;
	QLabel *testLabel19_;
	QLabel *testLabel20_;
	AMControlEditor *m2RotationalStepControlEditor_;
	AMControlEditor *m2RotationalEncoderControlEditor_;
	QLabel *testLabel23_;
	QLabel *testLabel24_;
	AMControlEditor *m3VerticalUpstreamStepControlEditor_;
	AMControlEditor *m3VerticalUpstreamEncoderControlEditor_;
	QLabel *testLabel27_;
	QLabel *testLabel28_;
	AMControlEditor *m3VerticalDownstreamStepControlEditor_;
	AMControlEditor *m3VerticalDownstreamEncoderControlEditor_;
	QLabel *testLabel31_;
	QLabel *testLabel32_;
	AMControlEditor *m3HorizontalUpstreamStepControlEditor_;
	AMControlEditor *m3HorizontalUpstreamEncoderControlEditor_;
	QLabel *testLabel35_;
	QLabel *testLabel36_;
	AMControlEditor *m3HorizontalDownstreamStepControlEditor_;
	AMControlEditor *m3HorizontalDownstreamEncoderControlEditor_;
	QLabel *testLabel39_;
	QLabel *testLabel40_;
	AMControlEditor *m3RotationalStepControlEditor_;
	AMControlEditor *m3RotationalEncoderControlEditor_;








};


#endif // SGMADVANCEDMIRRORVIEW_H
