#ifndef AMSCANVIEW_H
#define AMSCANVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QLayout>
#include <QButtonGroup>
#include <QStringList>

#include "MPlot/MPlot.h"
#include "MPlot/MPlotSceneAndView.h"
#include "dataman/AMScan.h"


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

	// Add a scan to this model.  The AMScan must exist elsewhere, for the lifetime that it is added to the model.  Model does not take ownership of the scan.
	void addScan(AMScan* newScan);

	// removes an AMScan from this model. Does not delete the scan.  Call this before deleting a scan that has been added to the model.
	bool removeScan(AMScan* removeMe);

protected slots:
	// the AMChannelListModel is a standard Qt model, but it guarantees that only one channel will be added at a time, and it will be added at the end of all rows(channels).
	void onChannelAboutToBeAdded(const QModelIndex& parent, int start, int end);

	void onChannelAdded(const QModelIndex& parent, int start, int end);

	void onChannelAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	void onChannelRemoved(const QModelIndex& parent, int start, int end);

protected:
	QList<AMScan*> scans_;
	QList<const AMChannelListModel*> scanChannelLists_;
	QList<QList<AMScanSetModelChannelMetaData> > channels_;

};



class AMScanViewModeBar : public QFrame {
	Q_OBJECT
public:
	explicit AMScanViewModeBar(QWidget* parent = 0);

	QToolButton* plusButton_, *subtractButton_;
	QButtonGroup* modeButtons_;

};

/*
class AMScanViewChannelEntry {
public:
	AMScanViewChannelEntry() { visible = true; plotSeries = 0; destinationPlot = -1; }
	bool visible;
	MPlotSeriesBasic* plotSeries;
	int destinationPlot;
};

class AMScanViewEntry {
public:
	AMScanViewEntry() { scan = 0; scanBar = 0; primaryChannel = 0; }

	// pointer to the scan itself
	const AMScan* scan;
	// scan->channelNames() could be useful

	QList<AMScanViewChannelEntry> chList;

	// A GUI element that exists for each channel, used to toggle which channels are visible
	AMChannelSelectorBar* scanBar;

	// in tab mode, where only one channel is plotted: use this one:
	int primaryChannel;
};*/

/// A GUI class that provides a QGraphicsWidget inside the scene, that scales with the size of the scene/view
class AMScanViewMainWidget : public MPlotSceneAndView {
	Q_OBJECT
public:

	AMScanViewMainWidget(QWidget* parent = 0) : MPlotSceneAndView(parent) {
		graphicsWidget_ = new QGraphicsWidget();
		this->scene()->addItem(graphicsWidget_);
		QPalette p = graphicsWidget_->palette();
		p.setColor(QPalette::Window, QColor(Qt::red));
		graphicsWidget_->setPalette(p);
	}


	virtual ~AMScanViewMainWidget() {
		delete graphicsWidget_;
	}

	QGraphicsWidget* graphicsWidget() const { return graphicsWidget_;}

protected:
	QGraphicsWidget* graphicsWidget_;

	// On resize events: notify the graphics widget to resize it.
	virtual void resizeEvent ( QResizeEvent * event ) {
		MPlotSceneAndView::resizeEvent(event);

		graphicsWidget_->resize(event->size());
		fitInView(graphicsWidget_, Qt::KeepAspectRatioByExpanding);
	}
};

class AMScanViewChannelSelector;

/// This GUI class is a helper for AMScanViewChannelSelector.  It handles a view subset for a single Scan.
class AMScanViewScanBar : public QFrame {
	Q_OBJECT
public:
	explicit AMScanViewScanBar(AMScanSetModel* model, int scanIndex, QWidget* parent = 0);

protected:
	/// ui components:
	QLabel* nameLabel_;
	QButtonGroup chButtons_;
	QToolButton* closeButton_;
	QHBoxLayout* chButtonLayout_;

	/// Index of "our" scan in the model:
	int scanIndex_;
	/// Connected model:
	AMScanSetModel* model_;


protected slots:
	/// after a scan or channel is added in the model
	void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or channel is deleted in the model:
	void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or channel is deleted in the model:
	void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes:
	void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	/// when one of the channel toggles is clicked:
	void onChannelButtonClicked(int id);


	friend class AMScanViewChannelSelector;
};

/// This GUI class is a view on an AMScanSetModel.  It shows each scan in a horizontal bar, with checkable buttons for each channel.
class AMScanViewChannelSelector : public QWidget {
	Q_OBJECT

public:
	explicit AMScanViewChannelSelector(AMScanSetModel* model = 0, QWidget* parent = 0);
	void setModel(AMScanSetModel* model);

protected slots:
	/// after a scan or channel is added in the model
	void onRowInserted(const QModelIndex& parent, int start, int end) {

		Q_UNUSED(end)

		// top-level: inserting a scan:
		if(!parent.isValid()) {
			AMScanViewScanBar* bar = new AMScanViewScanBar(model_, start);
			barLayout_->insertWidget(start, bar);
			scanBars_.insert(start, bar);
		}

		// otherwise, inserting a channel. Handled separately by our AMScanViewScanBar's
	}

	/// before a scan or channel is deleted in the model:
	void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {
		Q_UNUSED(end)
		// invalid (top-level) parent: means we're inserting a scan
		if(!parent.isValid()){
			delete scanBars_.takeAt(start);
			// all the scans above this one need to move their scan index down:
			for(int i=start; i<scanBars_.count(); i++)
				scanBars_[i]->scanIndex_--;
		}
	}

	/// after a scan or channel is deleted in the model:
	void onRowRemoved(const QModelIndex& parent, int start, int end) {
		/// \todo Anything needed here?
	}

	/// when data changes:
	void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
		/// \todo: any changes here that we care about?
	}

protected:
	QList<AMScanViewScanBar*> scanBars_;
	QVBoxLayout* barLayout_;
	AMScanSetModel* model_;


};

class AMScanViewInternal;

/// A GUI class that provides a several different ways to view a set of scans.  It maintains an internal AMScanSetModel, and a variety of different AMScanViewInternal views can be shown within it.
class AMScanView : public QWidget
{
Q_OBJECT
public:
	enum ViewMode { Invalid = -1, Tabs = 0, OverPlot, MultiScans, MultiChannels };

    explicit AMScanView(QWidget *parent = 0);

	/// returns the AMScanSetModel used internally to hold the scans/channels.
	AMScanSetModel* model() const { return scansModel_; }

signals:

public slots:

	/// change the view mode (newMode is a ViewMode enum: 0 for one channel at a time; 1 for channels overplotted; 2 for one plot per scan; 2 for one plot per channel.
	void changeViewMode(int newMode);

	/// add a scan to the view:
	void addScan(AMScan* scan);
	/// remove a scan from the view:
	void removeScan(AMScan* scan);

protected:

	AMScanSetModel* scansModel_;

	/// List of the different views available
	QList<AMScanViewInternal*> views_;
	/// current view mode
	ViewMode mode_;

	/// helper function: remove all plot series:
	// void removeAllPlotSeries();
	/// helper function: find out how many unique channels there are, and populate channelNames_;
	void uniqueChannelSearch();

	// ui components:
	QGraphicsGridLayout* glayout_;
	int width_, rc_, cc_;// layout locations: width (num cols), rowcounter, columncounter.



	AMScanViewModeBar* modeBar_;
	AMScanViewChannelSelector* scanBars_;



	// build UI
	void setupUI();
	// setup all UI event-handling connections
	void makeConnections();



};

#endif // AMSCANVIEW_H
