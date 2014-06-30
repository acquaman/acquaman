#ifndef REIXSXESSPECTROMETERCONTROLPANEL_H
#define REIXSXESSPECTROMETERCONTROLPANEL_H

#include "ui/beamline/AMControlMoveButton.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "REIXSXESSpectrometerControlEditor.h"
#include "ui/REIXS/REIXSXESMCPDetectorView.h"



#include <QWidget>

class REIXSXESSpectrometerControlPanel : public QWidget
{
    Q_OBJECT
public:
	explicit REIXSXESSpectrometerControlPanel(REIXSXESMCPDetector* detector, QWidget *parent);

signals:

public slots:

private:
	AMControlMoveButton *sampleXUp_, *sampleXDown_;
	REIXSXESMCPDetectorView* detectorView_;
	REIXSXESSpectrometerControlEditor* spectrometerEditor_;




};

#endif // REIXSXESSPECTROMETERCONTROLPANEL_H
