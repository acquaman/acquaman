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


#ifndef STWIDGET_H
#define STWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QInputDialog>
#include <QMenu>
#include <QAction>

#include "beamline/AMPVControl.h"

#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"

#include "STVariable.h"
#include "STPlotWidget.h"
#include "STPlotEditor.h"
#include "STVariableCollection.h"
#include "STVariableCollectionEditor.h"
#include "STEditorDialog.h"

class STWidget : public QWidget
{
    Q_OBJECT

public:
    STWidget(QWidget *parent = 0);
    ~STWidget();

public slots:

protected slots:
    void onVariableConnected(int variableIndex);
    void onVariableAxisInfoChanged(int variableIndex);
    void toAddVariable();
    void toEditVariables();
    void toEditPlot();

    virtual void onCustomContextMenuRequested(QPoint position);

protected:
    void showEditorDialog(STEditor *editor);

protected:
    STVariableCollection *variables_;
    STPlotWidget *plotWidget_;
};

#endif // STWIDGET_H
