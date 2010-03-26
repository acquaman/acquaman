#ifndef ACQMAN_Scheduler_H
#define ACQMAN_Scheduler_H

#include "ui_Scheduler.h"

class Scheduler : public QWidget, private Ui::Scheduler {
	
	Q_OBJECT
	
public:
	Scheduler(QWidget* parent = 0) : QWidget(parent) {
		
		setupUi(this);
	}
	
};

#endif
