#ifndef CLSPGTDETECTORV2VIEW_H
#define CLSPGTDETECTORV2VIEW_H

#include "ui/beamline/AMDetectorView.h"

class CLSPGTDetectorV2;
class AMExtendedControlEditor;

class CLSPGTDetectorV2View : public AMDetectorGeneralDetailedView
{
Q_OBJECT
public:
 	virtual ~CLSPGTDetectorV2View();
	CLSPGTDetectorV2View(CLSPGTDetectorV2 *detector, QWidget *parent = 0);

protected:
	AMExtendedControlEditor *integrationModeSelector_;
};

#endif // CLSPGTDETECTORV2VIEW_H
