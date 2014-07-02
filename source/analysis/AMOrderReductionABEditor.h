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


#ifndef AMORDERREDUCTIONABEDITOR_H
#define AMORDERREDUCTIONABEDITOR_H

#include <QWidget>
#include <QComboBox>

#include "analysis/AMOrderReductionAB.h"

///	This provides a basic view to change the reduced axis as well as switch between other data sources.
class AMOrderReductionABEditor : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMOrderReductionAB pointer.
 	virtual ~AMOrderReductionABEditor();
	explicit AMOrderReductionABEditor(AMOrderReductionAB *analysisBlock, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles changing all the necessary things when the data source is changed.
	void onDataSourceChanged(int index);
	/// Handles changing the reduced axis.
	void onReducedAxisChanged(int index);
	/// Helper slot that populates the combo boxes.
	void populateComboBox();

protected:
	/// Pointer to the order reduction analysis block.
	AMOrderReductionAB *analysisBlock_;

	/// The combo box that contains all the data source names.  Used for choosing which data source to use.
	QComboBox *sourceNames_;
	/// The combo box that contains the reduced axis options.
	QComboBox *reducedAxisOptions_;
};

#endif // AMORDERREDUCTIONABEDITOR_H
