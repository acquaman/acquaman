#ifndef ACQMAN_ProtocolViewer_H
#define ACQMAN_ProtocolViewer_H

#include <QtGui>
#include "ui_ProtocolViewer.h"

class ProtocolViewer : public QWidget, private Ui::ProtocolViewer {
	
	Q_OBJECT
	
public:
	ProtocolViewer(QWidget* parent = 0) : QWidget(parent) {
		
		setupUi(this);
	}
	
};

#endif
