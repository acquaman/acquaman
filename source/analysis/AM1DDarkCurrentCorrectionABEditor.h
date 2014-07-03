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


#ifndef AM1DDARKCURRENTCORRECTIONABEDITOR_H
#define AM1DDARKCURRENTCORRECTIONABEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QFormLayout>

#include "analysis/AM1DDarkCurrentCorrectionAB.h"

class AM1DDarkCurrentCorrectionABEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AM1DDarkCurrentCorrectionABEditor(AM1DDarkCurrentCorrectionAB *analysisBlock, QWidget *parent = 0);
    virtual ~AM1DDarkCurrentCorrectionABEditor();

signals:

public slots:

protected slots:
    /// Handles setting the data name for the analysis blockbased on the new choice of input data source.
    void onDataNameChoiceChanged(int index);
    /// Helper slot. Populates the names combo box.
    void populateComboBox();

protected:
    /// Pointer to the dark current analysis block.
    AM1DDarkCurrentCorrectionAB *analysisBlock_;

    /// The combo box that contains all of the names. Used for the data input source.
    QComboBox *dataNames_;

};

#endif // AM1DDARKCURRENTCORRECTIONABEDITOR_H
