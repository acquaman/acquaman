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


#ifndef AM1DNORMALIZATIONABEDITOR_H
#define AM1DNORMALIZATIONABEDITOR_H

#include <QWidget>
#include <QComboBox>

#include "analysis/AM1DNormalizationAB.h"

/*!
  This is a very basic view that contains a drop box that holds all of the available data sources for normalization.

  \todo Because of the dynamic nature of setting various data sources, the AMDataSource API will need to have a general capability for choosing data sources like this (or in some other way).  That way some of these views can be generalized rather than making a specific one for every analysis block.
  */
class AM1DNormalizationABEditor : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes in an AM1DNormalizationAB pointer.
 	virtual ~AM1DNormalizationABEditor();
	explicit AM1DNormalizationABEditor(AM1DNormalizationAB *analysisBlock, QWidget *parent = 0);

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
	/// Pointer to the normalization analysis block.
	AM1DNormalizationAB *analysisBlock_;

	/// The combo box that contains all of the names.  Used for the data input source.
	QComboBox *dataNames_;
	/// The combo box that contains all of the names.  Used for the normalization data source.
	QComboBox *normalizationNames_;

};

#endif // AM1DNORMALIZATIONABEDITOR_H
