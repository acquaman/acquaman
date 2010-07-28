#ifndef AMIMPORTCONTROLLERWIDGET_H
#define AMIMPORTCONTROLLERWIDGET_H

#include <QWidget>
#include "ui_AMImportControllerWidget.h"
#include "ui/AMThumbnailScrollViewer.h"

class AMImportControllerWidget : public QWidget, public Ui::AMImportControllerWidget {
public:
	explicit AMImportControllerWidget(QWidget* parent = 0) : QWidget(parent) {
		setupUi(this);
		thumbnailViewer = new AMThumbnailScrollWidget();
		verticalLayoutLeft->insertWidget(0, thumbnailViewer);
	}

	AMThumbnailScrollWidget* thumbnailViewer;
};

#endif // AMIMPORTCONTROLLERWIDGET_H
