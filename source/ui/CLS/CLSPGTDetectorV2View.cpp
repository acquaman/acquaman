#include "CLSPGTDetectorV2View.h"

#include <QHBoxLayout>
#include "beamline/CLS/CLSPGTDetectorV2.h"
#include "ui/beamline/AMExtendedControlEditor.h"

 CLSPGTDetectorV2View::~CLSPGTDetectorV2View(){}
CLSPGTDetectorV2View::CLSPGTDetectorV2View(CLSPGTDetectorV2 *detector, QWidget *parent) :
	AMDetectorGeneralDetailedView(detector, parent)
{
	integrationModeSelector_ = new AMExtendedControlEditor(detector->privilegedIntegrationModeControl(this), 0, false, false);

	mainHL_->addWidget(integrationModeSelector_);
}
