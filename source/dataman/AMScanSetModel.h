/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMSCANSETMODEL_H
#define AMSCANSETMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QPen>
#include <QColor>

#include "MPlot/MPlotColorMap.h"

#include "acquaman.h"

class AMScan;
class AMDataSource;

/// This class holds visualization information about AMDataSources; all the plot settings that are associated with a particular plot/layout, rather than with the AMDataSource itself.
class AMDataSourcePlotSettings {
public:
	/// Default Constructor
	AMDataSourcePlotSettings(double Priority = 1, const QPen& LinePen = QPen(nextColor()), bool Visible = true)
		: priority(Priority),
		  visible(Visible),
		  linePen(LinePen) {

		areaFilled = false;
		useStandardColorMap = true;
		standardColorMap = MPlotColorMap::Jet;
		brightness = 1;
		contrast = 1;
		gamma = 1;
	}



	/// Priority level for this data source (used for ordering... lower numbers appear first.)
	double priority;
	/// Whether this data source is shown/enabled in non-exclusive views.
	bool visible;

	// 1D plot settings:
	/// Pen used for this data source (dots, dashes, etc.), as well as width and color
	QPen linePen;
	/// True if the area below the plot should be filled \note These don't work yet, since MPlot doesn't yet support filled plots
	bool areaFilled;
	/// The brush of the fill, if used (ie: areaFilled is true) \note These don't work yet, since MPlot doesn't yet support filled plots
	QBrush fillBrush;

	// 2D plot settings:
	bool useStandardColorMap;
	MPlotColorMap::StandardColorMap standardColorMap;
	QColor customMapColor1, customMapColor2;
	double brightness, contrast, gamma;

	/// Resultant colormap used for multi-dimensional data
	MPlotColorMap colorMap;


	/// Globally-accessible function to get the "next" data source color to use.
	static QColor nextColor() {
		static int i = 0;

		switch(i++ % 11) {
		case 0: return QColor(255, 0, 128);
		case 1: return QColor(0, 128, 255);
		case 2: return QColor(128, 255, 0);
		case 3: return QColor(255, 128, 0);
		case 4: return QColor(128, 0, 255);
		case 5: return QColor(0, 0, 255);
		case 6: return QColor(0, 128, 0);
			// Yellow is hard to see: case 7: return QColor(255, 255, 0);
		case 7: return QColor(255, 0, 0);
		case 8: return QColor(0, 64, 128);
		case 9: return QColor(128, 64, 0);
		case 10: default: return QColor(128, 0, 64);
		}
	}

};

/// This class provides a standard Qt model for a set of AMScans, with the 2nd level in the tree containing their AMDataSources.  It can be used with a standard Qt view (ex: QTreeView), and it's also used with custom views like AMScanView.
/*!
The data() function is implemented to return values that are relevant to standard Qt views:

Data Roles:

 Qt::DisplayRole: QString			- the name of the scan or data source
 Qt::DecorationRole: QColor		- line/display color (\todo Return a QPixmap for 2D data sources, based on their colormap gradient?)

 Qt::TooltipRole: QString			- detailed information (Scan's name, number, dateTime and sample name; or data source description() )
 AM::DescriptionRole: QString		- detailed information (Scan sample name and dateTime; or data source description() )

 Qt::CheckStateRole: Qt::CheckState	- Data sources only: whether visible or not. (Qt::Checked or Qt::Unchecked)

 // custom roles:
 AM::PointerRole: AMScan* or AMDataSource*	- the pointer to the object
 AM::PriorityRole: double (Data sources only): used for ordering (lowest to highest).
 AM::CanCloseRole: whether an 'x' should be shown beside this scan/data-source, to allow closing or deleting.  On by default.

 AM::RankRole: int (Data sources only): the rank (# of dimensions) of the data source

 AM::LinePenRole: QPen	- pen used for drawing in scans
 AMScanSetModel::FilledRole
 AMScanSetModel::FillColorRole

 AMScanSetModel::ColorMapRole
 AMScanSetModel::UseStandardColorMapRole
 AMScanSetModel::StandardColorMapRole
 AMScanSetModel::FirstColorRole
 AMScanSetModel::SecondColorRole
 AMScanSetModel::BrightnessRole
 AMScanSetModel::ContrastRole
 AMScanSetModel::GammaRole


 \note While while the Qt standard model API supports inserting/removing multiple rowse, the AMScanSetModel guarantees that only <i>one</i> row (ie: Scan or Data Source) will be inserted/removed/modified at a time.  For all the rowsInserted(), rowsRemoved(), and dataChanged() signals, it's safe to assume that \c start and \c end are the same, as well as \c topLeft and \c bottomRight.

 \todo !!!!!!
 hookup edited properties in scan/data sources (name, etc.) to dataChanged() signal out of model.
 */

class AMScanSetModel : public QAbstractItemModel {
	Q_OBJECT
public:

	/// Additional custom roles for the data we expose:
	enum DataRoles { FilledRole = AM::UserRole + 20, FillColorRole, ColorMapRole, UseStandardColorMapRole, StandardColorMapRole, FirstColorRole, SecondColorRole, BrightnessRole, ContrastRole, GammaRole };

	/// Default constructor
	AMScanSetModel(QObject* parent = 0) : QAbstractItemModel(parent) {}

	/// Implemented from QAbstractItemModel.  Use this to create a QModelIndex to access a scan or data source in the data() function.  Scans are found by row-based indexing at the top level of the tree (ie: \c parent is an invalid QModelIndex() ).  Data sources are found by row-based indexing, using a valid \c parent corresponding to their Scan's index().
	QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

	/// Implemented from QAbstractItemModel.  Gives the parent index (ie: Scan index) for a lower-level element (ie: data source index).
	QModelIndex parent ( const QModelIndex & index ) const;

	/// Implemented from QAbstractItemModel.
	Qt::ItemFlags flags ( const QModelIndex & index ) const;

	/// Returns data values for roles that are relevant to standard Qt views.
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

	// Shortcut functions for accessing custom roles (ie: plot settings) which cannot be accessed through the scan itself
	////////////////////////////////////

	/// Returns a reference to the plot settings for the data source at \c dataSourceIndex within the scan at \c scanIndex. The plot settings include things like the visibility, line color, line pen, color map, etc.
	const AMDataSourcePlotSettings& plotSettings(int scanIndex, int dataSourceIndex) const {
		return sourcePlotSettings_.at(scanIndex).at(dataSourceIndex);
	}
	/// Indicates whether the data source at \c dataSourceIndex within the scan at \c scanIndex should be visible on plots.
	bool isVisible(int scanIndex, int dataSourceIndex) const {
		return sourcePlotSettings_.at(scanIndex).at(dataSourceIndex).visible;
	}
	/// Returns the color to use for data source at \c dataSourceIndex within the scan at \c scanIndex
	QColor plotColor(int si, int di) const {	return sourcePlotSettings_.at(si).at(di).linePen.color(); }
	/// Returns the pen that should be used to draw the data source at \c dataSourceIndex within the scan at \c scanIndex
	QPen plotPen(int si, int di) const {
		return sourcePlotSettings_.at(si).at(di).linePen;
	}
	/// Returns the color map to use for data source at \c dataSourceIndex within the scan at \c scanIndex
	MPlotColorMap plotColorMap(int si, int di) const { return sourcePlotSettings_.at(si).at(di).colorMap; }
	/// Returns the priority to use for data source at \c dataSourceIndex within the scan at \c scanIndex. The priority is a double value that controls the stacking order of plots on top of each other.
	double priority(int si, int di) const { return sourcePlotSettings_.at(si).at(di).priority; }




	/// Implemented from QAbstractItemModel.
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/// Returns the number of scans (for a top-level \c parent = QModelIndex() ), or the number of data sources (for a scan-level \c parent).
	int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
	/// Returns the number of scans represented within the model.
	int scanCount() const { return rowCount(); }
	/// Return a list of the names of all scans in this model:
	QStringList scanNames() const;

	/// Implemented from QAbstractItemModel.  Number of columns is always 1.
	int columnCount ( const QModelIndex & parent = QModelIndex() ) const { Q_UNUSED(parent); return 1; }
	/// Implemented as required in QAbstractItemModel.
	bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;

	/// returns the row number of an AMScan in the top-level. returns -1 if not found.
	int indexOf(const AMScan* scan) const;
	/// returns the row number of an AMDataSource within an AMScan. Returns -1 if not found.
	int indexOf(const AMDataSource* dataSource, int scanIndex) const;
	/// returns the row number of an AMDataSource within an AMScan. Returns -1 if not found.
	int indexOf(const AMDataSource* dataSource, const AMScan* insideHere) const;

	/// Shortcuts to accessing a scan pointer.
	AMScan* scanAt(int scanIndex) const {
		if(scanIndex < 0 || scanIndex >= scans_.count())
			return 0;

		return scans_.at(scanIndex);
	}

	/// get a model index suitable for accessing a scan element:
	QModelIndex indexForScan(int scanIndex) const {
		return index(scanIndex, 0, QModelIndex());
	}


	/// get a model index suitable for accessing a data source element:
	QModelIndex indexForDataSource(int scanIndex, int dataSourceIndex) {
		return index(dataSourceIndex, 0, indexForScan(scanIndex));
	}


	/// shortcut for accessing a data source pointer:
	AMDataSource* dataSourceAt(int scanIndex, int dataSourceIndex) const;


	/// returns the name of an "exclusive" data source: one that might be preferred in exclusive views.  Returns empty string if an exclusive data source is not yet established.
	QString exclusiveDataSourceName() const { return exclusiveDataSourceName_; }

	/// Set the exclusive data source, by name. To clear the exclusive data source, specify an empty string. (This will cause 'exclusive' views to show nothing.)
	bool setExclusiveDataSourceByName(const QString& exclusiveDataSourceName);

	/// returns a list of all data source names that exist (over all scans in the model). Warning: this is slow.  O(n), where n is the total number of data sources in all scans.
	/*! \todo Optimize with caching. */
	QStringList allDataSourceNames() const;


	/// returns a list of all data source names that exist and are visible in at least one scan. Warning: this is slow.  O(n), where n is the total number of data sources in all scans.
	/*! \todo Optimize with caching. */
	QStringList visibleDataSourceNames() const;


	// Resizable Interface:

	/// Add a scan to this model.  The AMScan must exist elsewhere, for the lifetime that it is added to the model.  The model does not take ownership of the scan.
	void addScan(AMScan* newScan);

	/// removes an AMScan from this model. Does not delete the scan.  You must call this before deleting a scan that has been added to the model.
	bool removeScan(AMScan* removeMe);

	/// edit interface. Not all roles/values can be edited.
	/*! Acceptable roles for editing:

   Scans:
  - [no editable roles]

   Data Sources:
  - Qt::TooltipRole and AM::DescriptionRole (calls setDescription() )
  - Qt::CheckStateRole (plot options: sets visible or not)
  - AM::PriorityRole (plot options: sets data source ordering)
  - Qt::DecorationRole (sets color of line pen)
  - AM::LinePenRole (plot options: sets pen used for data source, including the color)



 */
	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );


public slots:
	// Convenience functions for setting plot-settings parameters
	//////////////////////////////////////////////////////////////
	void setVisible(int scanIndex, int dataSourceIndex, bool isVisible) {
		sourcePlotSettings_[scanIndex][dataSourceIndex].visible = isVisible;
		QModelIndex i = indexForDataSource(scanIndex, dataSourceIndex);
		emit dataChanged(i, i);
	}

signals:
	void exclusiveDataSourceChanged(QString exclusiveDataSourceName);


protected slots:

	void onDataSourceAboutToBeAdded(int index);
	void onDataSourceAdded(int index);
	void onDataSourceAboutToBeRemoved(int index);
	void onDataSourceRemoved(int index);

	/// This slot catches signals from scans that change their meta-data (name, number, dateTime, or sample), and emits the model's dataChanged() as needed.
	void onMetaDataChanged();
	/// This slot catches changes in the modified() flag for scans, which must cause a dataChanged() to update the display text.
	void onScanModifiedChanged(bool isModified);

protected:
	QList<AMScan*> scans_;
	QList<QList<AMDataSourcePlotSettings> > sourcePlotSettings_;

	QString exclusiveDataSourceName_;

};

#endif // AMSCANSETMODEL_H
