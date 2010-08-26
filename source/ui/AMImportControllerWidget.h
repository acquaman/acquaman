#ifndef AMIMPORTCONTROLLERWIDGET_H
#define AMIMPORTCONTROLLERWIDGET_H

#include <QWidget>
#include "ui_AMImportControllerWidget.h"
#include "ui/AMThumbnailScrollViewer.h"
#include "ui/AMRunSelector.h"

class AMImportControllerWidget : public QWidget, public Ui::AMImportControllerWidget {
public:
	explicit AMImportControllerWidget(QWidget* parent = 0) : QWidget(parent) {
		setupUi(this);
		thumbnailViewer = new AMThumbnailScrollWidget();
		verticalLayoutLeft->insertWidget(0, thumbnailViewer);

		runEdit = new AMRunSelector();
		gridLayout->addWidget(runEdit, 3, 2);
	}

	// UI elements (public within this widget class)
	AMThumbnailScrollWidget* thumbnailViewer;
	AMRunSelector* runEdit;
};

#endif // AMIMPORTCONTROLLERWIDGET_H
