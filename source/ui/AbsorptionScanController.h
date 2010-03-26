#ifndef ACQMAN_AbsorptionScanController_H
#define ACQMAN_AbsorptionScanController_H

#include <QtGui>
#include "ui_AbsorptionScanController.h"

class AbsorptionScanController : public QWidget, private Ui::AbsorptionScanController {
	
	Q_OBJECT
	
public:
	AbsorptionScanController(QWidget* parent = 0) : QWidget(parent) {
		
		setupUi(this);
	}
	
};

#endif
