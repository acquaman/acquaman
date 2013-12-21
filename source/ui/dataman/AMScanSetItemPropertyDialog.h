/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMSCANSETITEMPROPERTYDIALOG_H
#define AMSCANSETITEMPROPERTYDIALOG_H

#include <QDialog>
#include <QPersistentModelIndex>
#include "ui/AMLinePropertyEditor.h"
#include "ui/dataman/AMImagePropertyEditor.h"

class AMScanSetModel;


/// This widget provides controls to edit the display properties of a line/series in a plot: color, width, line style, fill (on/off), fill color.  It's a helper class for AMScanSetItemPropertyDialog, which simply extends AMLinePropertyEditor to talk directly to an item (persistent index) within the AMScanSetModel.
class AMScanSetItem1DPropertyEditor : public AMLinePropertyEditor {
	Q_OBJECT

public:
 	virtual ~AMScanSetItem1DPropertyEditor();
	explicit AMScanSetItem1DPropertyEditor(AMScanSetModel* model, const QPersistentModelIndex& dataSourcePersistentIndex, QWidget* parent = 0);

protected slots:
	void onLinePenChanged(const QPen& pen);

protected:
	AMScanSetModel* model_;
	QPersistentModelIndex pi_;
};

/// This widget provides the controls to edit the display properties of a surface map (color map): whether to use a standard color map (and which colormap), or the two custom colors in a linear color map. Also provides brightness, contrast, and gamma adjustments.  It's a helper class for AMScanSetItemPropertyDialog, and simply extends AMImagePropertyEditor to talk directly to an item within the AMScanSetModel.
class AMScanSetItem2DPropertyEditor : public AMImagePropertyEditor {
	Q_OBJECT

public:
 	virtual ~AMScanSetItem2DPropertyEditor();
	explicit AMScanSetItem2DPropertyEditor(AMScanSetModel* model, const QPersistentModelIndex& dataSourcePersistentIndex, QWidget* parent = 0);

protected slots:
	void onColorMapChanged(const MPlotColorMap& map);

protected:
	AMScanSetModel* model_;
	QPersistentModelIndex pi_;
};

/// This class provides a general property editor for items (data sources) in an AMScanSetModel... such as line color/style, colormap, etc.  Depending on the rank of the data source, it will embed different widgets; this class provides the shell, and the open/close behaviour handling.
/*! <b>Memory management:</b>

This dialog deletes itself when accept()ed or reject()ed, as well as when the scan or data source it represents is removed from the model. */
class AMScanSetItemPropertyDialog : public QDialog
{
	Q_OBJECT
public:
	/// Construct a property dialog on the given \c model, for the scan at \c scanIndex and the data source at \c dataSourceIndex. (These are just current indexes... If they change later because of scans and data sources being added, we'll handle that internally for you.)
	AMScanSetItemPropertyDialog(AMScanSetModel* model, int scanIndex, int dataSourceIndex, QWidget* parent = 0);
	/// Destructor
	virtual ~AMScanSetItemPropertyDialog();

signals:

public slots:

protected slots:
	/// When a scan or data source is removed from the model. Did we lose the scan or data source we represent?
	void onRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// When the model data changes... We watch this to possibly update the window title
	void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

protected:
	/// The model we're built for.
	AMScanSetModel* model_;

	/// Used to maintain the index of the data source, throughout inserts and removals.  pi_.parent() will be the index of the scan.
	QPersistentModelIndex pi_;

};

#endif // AMSCANSETITEMPROPERTYDIALOG_H
