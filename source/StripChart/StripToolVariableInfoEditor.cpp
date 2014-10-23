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


#include "StripToolVariableInfoEditor.h"
#include <QDebug>

StripToolVariableInfoEditor::StripToolVariableInfoEditor(QObject *parent) :
	QObject(parent)
{
	info_ = 0;
	dialog_ = 0;
}



StripToolVariableInfo *StripToolVariableInfoEditor::info() const {
	return info_;
}



EditPVDialog* StripToolVariableInfoEditor::dialog() {
	return dialog_;
}



void StripToolVariableInfoEditor::editInfo(StripToolVariableInfo* toEdit) {
	if (toEdit == 0) {
		qDebug() << "StripToolVariableInfoEditor :: cannot edit the info for a variable = 0. No changes made.";
		return;
	}

	info_ = toEdit;
	dialog_ = new EditPVDialog();

	setDialogFromInfo();
	executeDialog();

	// clean up?
}



void StripToolVariableInfoEditor::setDialogFromInfo() {

	if (info() != 0) {
		if (info()->hasName()) {
			dialog()->displayName(info()->name(), false);
			/*
			qDebug() << "StripToolVariableInfoEditor :: displaying variable name : " << info()->name();
			*/
		}

		if (info()->hasCreationDateTime()) {
			dialog()->displayCreationDateTime(info()->creationDateTime(), false);
			/*
			qDebug() << "StripToolVariableInfoEditor :: displaying varible creation date/time : " << info()->creationDateTime();
			*/
		}

		if (info()->importAutomatically())
			dialog()->displayAutoImport("true", true);
		else
			dialog()->displayAutoImport("false", true);

		if (info()->hasDescription()) {
			dialog()->displayDescription(info()->description(), true);
			/*
			qDebug() << "StripToolVariableInfoEditor :: displaying variable description : " << info()->description();
			*/
		}

		if (info()->hasUnits()) {
			dialog()->displayUnits(info()->units(), true);
			/*
			qDebug() << "StripToolVariableInfoEditor :: displaying variable units : " << info()->units();
			*/
		}

		if (info()->hasGranularity()) {
			dialog()->displayGranularity(QString::number(info()->granularity()), true);
			/*
			qDebug() << "StripToolVariableInfoEditor :: displaying variable granularity : " << info()->granularity();
			*/
		}

		if (info()->hasAverageOverPoints()) {
			dialog()->displayAverageOverPoints(QString::number(info()->averageOverPoints()), true);
		}

		if (info()->hasColor()) {
			dialog()->displayColorName(info()->colorName(), true);
			/*
			qDebug() << "StripToolVariableInfoEditor :: displaying variable color name : " << info()->colorName();
			*/
		}

		QString axisMax = "";
		if (info()->hasCustomAxisMax())
			axisMax = QString::number(info()->customAxisMax());

		dialog()->displayAxisMax(axisMax, true);
		connect( info(), SIGNAL(dataMaxChanged(double)), dialog(), SIGNAL(dataMaxUpdated(double)) );

		QString axisMin = "";
		if (info()->hasCustomAxisMin())
			axisMin = QString::number(info()->customAxisMin());

		dialog()->displayAxisMin(axisMin, true);
		connect( info(), SIGNAL(dataMinChanged(double)), dialog(), SIGNAL(dataMinUpdated(double)) );

	}

}



void StripToolVariableInfoEditor::executeDialog() {
	if (dialog_ != 0 && info_ != 0) {

		if (dialog()->exec()) {
			qDebug() << "StripToolVariableInfoEditor :: applying any changes made to variable info.";

			if (dialog()->importStateChanged()) {
				QString newState = dialog()->importState();
				if (newState == "true")
					info()->setToImportAutomatically(true);
				else
					info()->setToImportAutomatically(false);
			}

			if (dialog()->descriptionChanged())
				info()->setDescription(dialog()->description());

			if (dialog()->unitsChanged())
				info()->setUnits(dialog()->units());

			if (dialog()->granularityChanged())
				info()->setGranularity(dialog()->granularity().toInt());

			if (dialog()->averageOverPointsChanged())
				info()->setAverageOverPoints(dialog()->averageOverPoints().toInt());

			if (dialog()->colorNameChanged())
				info()->setColor( QColor(dialog()->colorName()) );

			if (dialog()->axisMaxChanged()) {
				QString dialogMax = dialog()->axisMax();
				if (dialogMax == "")
					info()->eraseCustomAxisMax();
				else
					info()->setCustomAxisMax(dialogMax.toDouble());
			}

			if (dialog()->axisMinChanged()) {
				QString dialogMin = dialog()->axisMin();
				if (dialogMin == "")
					info()->eraseCustomAxisMin();
				else
					info()->setCustomAxisMin(dialogMin.toDouble());
			}
		}
	}
}
