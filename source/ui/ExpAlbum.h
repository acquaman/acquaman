#ifndef ACQMAN_ExpAlbum_H
#define ACQMAN_ExpAlbum_H

#include <QtGui>
#include "ui_ExpAlbum.h"

class ExpAlbum : public QWidget, private Ui::ExpAlbum {
	
	Q_OBJECT
	
public:
	ExpAlbum(QWidget* parent = 0) : QWidget(parent) {
		
		setupUi(this);
	}
	
};

#endif
