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


#ifndef AM3DNORMALIZATIONABEDITOR_H
#define AM3DNORMALIZATIONABEDITOR_H

#include <QWidget>
#include <QComboBox>

#include "analysis/AM3DNormalizationAB.h"

/*!
  This is a very basic view that contains a drop box that holds all of the available data sources for mormalization.
*/
class AM3DNormalizationABEditor : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AM3DNormalizationAB pointer.
 	virtual ~AM3DNormalizationABEditor();
	explicit AM3DNormalizationABEditor(AM3DNormalizationAB *analysisBlock, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles setting the data name for the analysis block based on the new choice of input data source.
	void onDataNameChoiceChanged(int index);
	/// Handles setting the normalization name for the analysis block based on the new choice of input data source.
	void onNormalizationNameChoiceChanged(int index);
	/// Helper slot.  Populates the names combo box.
	void populateComboBox();

protected:
	/// Pointer to the derivative analysis block.
	AM3DNormalizationAB *analysisBlock_;

	/// The combo box that contains all of the names.  Used for the data input source.
	QComboBox *dataNames_;
	/// The combo box that contains all of the names.  Used for the normalization data source.
	QComboBox *normalizationNames_;
};

#endif // AM3DNORMALIZATIONABEDITOR_H
