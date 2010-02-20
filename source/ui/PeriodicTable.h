#ifndef PeriodicTable_H
#define PeriodicTable_H

#include "ui_PeriodicTable.h"

class PeriodicTable : public QWidget, private Ui::PeriodicTable {
	
	Q_OBJECT
	
public:
	PeriodicTable(QWidget* parent = 0) : QWidget(parent) {
		
		setupUi(this);
	}
	
};

#endif