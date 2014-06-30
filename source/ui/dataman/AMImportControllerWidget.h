/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMIMPORTCONTROLLERWIDGET_H
#define AMIMPORTCONTROLLERWIDGET_H

#include <QWidget>
#include "ui_AMImportControllerWidget.h"
#include "ui/AMThumbnailScrollViewer.h"
#include "ui/dataman/AMRunSelector.h"

class AMImportControllerWidget : public QWidget, public Ui::AMImportControllerWidget {
public:
 	virtual ~AMImportControllerWidget(){}
	explicit AMImportControllerWidget(QWidget* parent = 0) : QWidget(parent) {
		setupUi(this);
		thumbnailViewer = new AMThumbnailScrollWidget();
		verticalLayoutLeft->insertWidget(0, thumbnailViewer);

		runEdit = new AMRunSelector(AMDatabase::database("user"));
		gridLayout->addWidget(runEdit, 3, 2);
	}

	// UI elements (public within this widget class)
	AMThumbnailScrollWidget* thumbnailViewer;
	AMRunSelector* runEdit;
};

#endif // AMIMPORTCONTROLLERWIDGET_H
