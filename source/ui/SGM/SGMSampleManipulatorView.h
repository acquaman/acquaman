#ifndef SGMSAMPLEMANIPULATORVIEW_H
#define SGMSAMPLEMANIPULATORVIEW_H

#include "ui/AMSampleManipulatorView.h"

class AMBeamlineListAction;

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

	void onStopAllButtonClicked();
	void onTransferPositionButtonClicked();
	void onMeasurePositionButtonClicked();

	void onHVButtonClicked();
	void onHVStateChanged();

	void onIlluminatorSliderValueMoved(int newValue);
	void onIlluminatorFeedbackChanged(double newValue);
	void onIlluminatorPreset(int presetIndex);


protected:
	QPushButton *mUpButton_;
	QPushButton *mDownButton_;
	QPushButton *mInboardButton_;
	QPushButton *mOutboardButton_;
	QPushButton *mUpstreamButton_;
	QPushButton *mDownstreamButton_;
	QPushButton *mCWButton_;
	QPushButton *mCCWButton_;

	QPushButton *stopAllButton_;
	QPushButton *transferPositionButton_;
	AMBeamlineListAction *transferPositionActions_;
	QPushButton *measurePositionButton_;
	AMBeamlineListAction *measurementPositionActions_;
	QPushButton *hvButton_;

	AMControl *mVerticalCtrl_;
	AMControl *mHorizontalCtrl_;
	AMControl *mInPlaneCtrl_;
	AMControl *mRotationCtrl_;

	AMControlEditor *mVerticalNC_;
	AMControlEditor *mHorizontalNC_;
	AMControlEditor *mInPlaneNC_;
	AMControlEditor *mRotationNC_;

	QSlider *illuminatorSlider_;
	QButtonGroup *illuminatorPresets_;
	QToolButton *illuminatorOff_;
	QToolButton *illuminatorDim_;
	QToolButton *illuminatorMid_;
	QToolButton *illuminatorOn_;

	QIcon upIcon_, downIcon_, inboardIcon_, outboardIcon_, upstreamIcon_, downstreamIcon_, cwIcon_, ccwIcon_;

	QGridLayout *gl_;

	double lastHVValue_;
};

#endif // SGMSAMPLEMANIPULATORVIEW_H
