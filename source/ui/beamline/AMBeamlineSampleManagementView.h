#ifndef AMBEAMLINESAMPLEMANAGEMENTVIEW_H
#define AMBEAMLINESAMPLEMANAGEMENTVIEW_H

#include <QWidget>

class AMBeamline;
class AMSamplePlateView;
class AMSampleCameraBrowserView;

class AMBeamlineSampleManagementView : public QWidget
{
Q_OBJECT
public:
	AMBeamlineSampleManagementView(AMBeamline *beamline, QWidget *parent = 0);

protected:
	AMBeamline *beamline_;
	AMSamplePlateView *samplePlateView_;
	AMSampleCameraBrowserView *cameraBrowserView_;
};

#endif // AMBEAMLINESAMPLEMANAGEMENTVIEW_H
