#include <QWidget>

#include "beamline/SGM/SGMBeamline.h"
#include "ui/SGM/SGMSampleManipulatorView.h"
#include "ui/dataman/AMSampleManagementPre2013Widget.h"

class SGMSSAWidget : public QWidget
{
Q_OBJECT
public:
	SGMSSAWidget(QWidget *parent = 0);

protected:
	SGMSampleManipulatorView* sampleManipulatorView_;
};
