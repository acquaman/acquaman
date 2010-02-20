#ifndef ProtocolViewer_H
#define ProtocolViewer_H

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