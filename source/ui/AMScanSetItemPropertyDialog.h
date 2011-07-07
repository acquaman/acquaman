#ifndef AMSCANSETITEMPROPERTYDIALOG_H
#define AMSCANSETITEMPROPERTYDIALOG_H

#include <QDialog>
#include <QPersistentModelIndex>
#include "ui/AMLinePropertyEditor.h"

class AMScanSetModel;


/// This widget provides controls to edit the display properties of a line/series in a plot: color, width, line style, fill (on/off), fill color.  It's a helper class for AMScanSetItemPropertyDialog, which simply extends AMLineSeriesPropertyEditor to talk directly to an item (persistent index) within the AMScanSetModel.
class AMScanSetItem1DPropertyEditor : public AMLinePropertyEditor {
	Q_OBJECT

public:
	explicit AMScanSetItem1DPropertyEditor(AMScanSetModel* model, const QPersistentModelIndex& dataSourcePersistentIndex, QWidget* parent = 0);

protected slots:
	void onLinePenChanged(const QPen& pen);

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
	~AMScanSetItemPropertyDialog();

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
