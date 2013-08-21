#ifndef AMBEAMLINESAMPLEMANAGEMENTVIEW_H
#define AMBEAMLINESAMPLEMANAGEMENTVIEW_H

#include <QWidget>

class QPushButton;

class AMBeamline;
class AMSamplePlateView;
class AMSamplePlateBrowserView;
class AMSampleCameraBrowserView;

class AMBeamlineSampleManagementView : public QWidget
{
Q_OBJECT
public:
	AMBeamlineSampleManagementView(AMBeamline *beamline, QWidget *parent = 0);

protected slots:
	void onCreateSamplePlateButtonClicked();
	void onLoadSamplePlateButtonClicked();

protected:
	AMBeamline *beamline_;

	AMSamplePlateBrowserView *samplePlateBrowserView_;
	AMSamplePlateView *samplePlateView_;
	AMSampleCameraBrowserView *cameraBrowserView_;

	QPushButton *createSamplePlateButton_;
	QPushButton *loadSamplePlateButton_;
};

#endif // AMBEAMLINESAMPLEMANAGEMENTVIEW_H
