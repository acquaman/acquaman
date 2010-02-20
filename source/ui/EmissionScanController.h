#ifndef EmissionScanController_H
#define EmissionScanController_H

#include <QtGui>
#include "ui_EmissionScanController.h"

class EmissionScanController : public QWidget, private Ui::EmissionScanController {
	
	Q_OBJECT
	
public:
	EmissionScanController(QWidget* parent = 0) : QWidget(parent) {
		
		setupUi(this);
	}
	
};

#endif