#ifndef SGMSAMPLEMANIPULATORVIEW_H
#define SGMSAMPLEMANIPULATORVIEW_H

#include "ui/AMSampleManipulatorView.h"

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

	AMBasicControlEditor *mVerticalNC_;
	AMBasicControlEditor *mHorizontalNC_;
	AMBasicControlEditor *mInPlaneNC_;
	AMBasicControlEditor *mRotationNC_;

	QIcon upIcon_, downIcon_, inboardIcon_, outboardIcon_, upstreamIcon_, downstreamIcon_, cwIcon_, ccwIcon_;

	QGridLayout *gl_;
};

#endif // SGMSAMPLEMANIPULATORVIEW_H
