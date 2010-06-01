#ifndef AMSCANSETMODEL_H
#define AMSCANSETMODEL_H

#include <QObject>
#include "dataman/AMChannel.h"
#include "dataman/AMScan.h"
#include <QAbstractItemModel>

#include <QPen>
#include <QColor>

/// This class holds meta-data information for channels, not contained within AMChannel itself, that is required by the AMScanSetModel.
class AMScanSetModelChannelMetaData {
public:
	/// Default Constructor
	AMScanSetModelChannelMetaData(const QColor& Color = nextColor(), double Priority = 1, const QPen& LinePen = QPen(), bool Visible = true)
		: color(Color),
		priority(Priority),
		linePen(LinePen),
		visible(Visible) {}

	/// common color used for plotting this channel
	QColor color;
	/// Priority level for this channel (used for ordering... lower numbers appear first.)  Only channels with a priority < 0 are shown in "exclusive" views.
	double priority;
	/// Pen used for this channel (dots, dashes, etc.)
	QPen linePen;
	/// Whether this channel is shown/enabled in non-exclusive views.
	bool visible;

	/// Globally-accessible function to get the "next" channel color to use.
	static QColor nextColor() {
		static int i = 0;

		switch(i++ % 12) {
		case 0: return QColor(255, 0, 128);
		case 1: return QColor(0, 128, 255);
		case 2: return QColor(128, 255, 0);
		case 3: return QColor(255, 128, 0);
		case 4: return QColor(128, 0, 255);
		case 5: return QColor(0, 0, 255);
		case 6: return QColor(0, 128, 0);
		case 7: return QColor(255, 255, 0);
		case 8: return QColor(255, 0, 0);
		case 9: return QColor(0, 64, 128);
		case 10: return QColor(128, 64, 0);
		case 11: default: return QColor(128, 0, 64);
		}
	}

};

/// This class provides a standard Qt model for a set of AMScans, with the 2nd level in the tree containing their AMChannels.  It can be used with a standard Qt view (ex: QTreeView), and it's also used inside custom views like AMScanView.
/*! Roles:

	Qt::DisplayRole: QString			- the name of the scan or channel

	Qt::DecorationRole: QColor			- line/display color

	Qt::TooltipRole: QString			- detailed information

	Qt::CheckStateRole: Qt::CheckState	- whether visible or not. (Qt::Checked or Qt::Unchecked)

	Qt::UserRole or AMScanSetModel::PointerRole: AMScan* or AMChannel*	- the pointer to the object

	AMScanSetModel::PriorityRole: double- used for ordering (lowest to highest). Negative values are displayed in "exclusive" views.

	AMScanSetModel::LinePenRole: QPen	- pen used for drawing in scans


	\note While while the Qt standard model API supports inserting/removing multiple rows at once, the AMScanSetModel guarantees that only <i>one</i> row (ie: Scan or Channel) will be inserted/removed/modified at a time.  For all the rowsInserted(), rowsRemoved(), and dataChanged() signals, it's safe to assume that \c start and \c end are the same, as well as \c topLeft and \c bottomRight.
	\note When adding, rows will ALWAYS be added at the end of all existing rows.  However, rows may be <i>removed</i> from any position.

	\todo !!!!!!
	hookup edited properties in scan/channels (name, etc.) to dataChanged() signal out of model.
	*/

class AMScanSetModel : public QAbstractItemModel {
	Q_OBJECT
public:
	enum ItemDataRoles { PointerRole = Qt::UserRole, PriorityRole, LinePenRole };


	/// Default constructor
	AMScanSetModel(QObject* parent = 0) : QAbstractItemModel(parent) {}

	/// Implemented from QAbstractItemModel.  Use this to create a QModelIndex to access a scan or channel.  Scans are found by row-based indexing at the top level of the tree (ie: \c parent is an invalid QModelIndex() ).  Channels are found by row-based indexing, using a valid parent corresponding to their Scan's index().
	QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

	/// Implemented from QAbstractItemModel.  Gives the parent index (ie: Scan index) for a lower-level element (ie: Channel index).
	QModelIndex parent ( const QModelIndex & index ) const;

	/// Implemented from QAbstractItemModel.
	Qt::ItemFlags flags ( const QModelIndex & index ) const;

	/// Returns data values based on the pointer roles and meta-data roles given in the class description.
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

	/// Implemented from QAbstractItemModel.
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/// Returns the number of scans (for a top-level \c parent = QModelIndex() ), or the number of channels (for a scan-level \c parent).
	int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
	/// Returns the number of scans represented within the model.
	int numScans() const { return rowCount(); }

	/// Implemented from QAbstractItemModel.  Number of columns is always 1.
	int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
	/// Implemented as required in QAbstractItemModel.
	bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;

	/// returns the row number of an AMScan in the top-level. returns -1 if not found.
	int indexOf(AMScan* scan) const;

	/// Shortcuts to accessing a scan pointer. (Can also retrieve using data() with an appropriate indexForScan() and the AMScanSetModel::PointerRole.)
	AMScan* scanAt(int scanIndex) const {
		if((unsigned)scanIndex >= (unsigned)scans_.count())
			return 0;

		return scans_[scanIndex];

		// equivalent: return data(index(scanIndex,0), PointerRole).value<AMScan*>();
	}

	/// get a model index suitable for accessing a scan element:
	QModelIndex indexForScan(int scanIndex) const {
		return index(scanIndex, 0, QModelIndex());
	}

	/// get a model index suitable for accessing a channel element:
	QModelIndex indexForChannel(int scanIndex, int channelIndex) {
		return index(channelIndex, 0, indexForScan(scanIndex));
	}

	/// shortcut for accessing a chanel:
	AMChannel* channelAt(int scanIndex, int channelIndex) const {
		if((unsigned)scanIndex >= (unsigned)scans_.count())
			return 0;

		if(channelIndex >= scans_.at(scanIndex)->numChannels())
			return 0;

		return scans_.at(scanIndex)->channel(channelIndex);
	}



	// Resizable Interface:

	/// Add a scan to this model.  The AMScan must exist elsewhere, for the lifetime that it is added to the model.  Model does not take ownership of the scan.
	void addScan(AMScan* newScan);

	/// removes an AMScan from this model. Does not delete the scan.  Call this before deleting a scan that has been added to the model.
	bool removeScan(AMScan* removeMe);

	/// edit interface. Not all roles/values can be edited.
	/*! Acceptable roles for editing:

	  Scans:
		- Qt::DisplayRole (setName())

	  Channels:
		- Qt::DecorationRole (meta-data: sets color)
		- Qt::CheckStateRole (meta-data: sets visible or not)
		- AMScanSetModel::PriorityRole (meta-data: sets channel ordering / appearance in exclusive views)
		- AMScanSetModel::LinePenRole (meta-data: sets pen used for channel)

	*/
	bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

protected slots:
	/// the AMChannelListModel is a standard Qt model, but it guarantees that only one channel will be added at a time, and it will be added at the end of all rows(channels).
	void onChannelAboutToBeAdded(const QModelIndex& parent, int start, int end);
/// received after the channel is added.
	void onChannelAdded(const QModelIndex& parent, int start, int end);

	/// AMChannelListModel guarantees that only one channel will be removed at a time. This is received before it gets removed.  \c start and \c end will always be the same.
	void onChannelAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// This is received after the channel removal completes.
	void onChannelRemoved(const QModelIndex& parent, int start, int end);

protected:
	QList<AMScan*> scans_;
	QList<const AMChannelListModel*> scanChannelLists_;
	QList<QList<AMScanSetModelChannelMetaData> > chMetaData_;

};

#endif // AMSCANSETMODEL_H
