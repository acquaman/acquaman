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


/// Meta-data information for channels, not contained within AMChannel, that is required by the AMScanSetModel
class AMScanSetModelChannelMetaData {
public:
	AMScanSetModelChannelMetaData(const QColor& Color = nextColor(), double Priority = 1, const QPen& LinePen = QPen(), bool Visible = true)
		: color(Color),
		priority(Priority),
		linePen(LinePen),
		visible(Visible) {}

	QColor color;
	double priority;
	QPen linePen;
	bool visible;

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

/// This class provides a standard Qt model for a set of AMScans, with the 2nd level in the tree containing their AMChannels.
/*! Roles:
	Qt::DisplayRole: QString			- the name of the scan or channel
	Qt::DecorationRole: QColor			- line/display color
	Qt::TooltipRole: QString			- detailed information
	Qt::CheckStateRole: Qt::CheckState	- whether visible or not. (Qt::Checked or Qt::Unchecked)
	Qt::UserRole or AMScanSetModel::PointerRole: AMScan* or AMChannel*	- the pointer to the object
	AMScanSetModel::PriorityRole: double- used for ordering (lowest to highest). Negative values are displayed in "exclusive" views.
	AMScanSetModel::LinePenRole: QPen	- pen used for drawing in scans


	*/

class AMScanSetModel : public QAbstractItemModel {
	Q_OBJECT

	enum ItemDataRoles { PointerRole = Qt::UserRole, PriorityRole, LinePenRole };

public:
	AMScanSetModel(QObject* parent = 0) : QAbstractItemModel(parent) {}

	QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

	QModelIndex parent ( const QModelIndex & index ) const;

	Qt::ItemFlags flags ( const QModelIndex & index ) const;

	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

	int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
	bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;

	/// returns the index (or row) of an AMScan in the top-level. returns -1 if not found.
	int indexOf(AMScan* scan) const;

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

class AMChannelSelectorBar : public QWidget {
public:
	explicit AMChannelSelectorBar(const AMScan* source = 0, QWidget* parent = 0);

	// ui components:
	QLabel* nameLabel_;
	QButtonGroup chButtons_;
	QToolButton* closeButton_;
	QHBoxLayout* chButtonLayout_;

};

class AMScanViewModeBar : public QWidget {
public:
	explicit AMScanViewModeBar(QWidget* parent = 0);

	QToolButton* plusButton_, *subtractButton_;
	QButtonGroup* modeButtons_;

};

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
};

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

class AMScanView : public QWidget
{
Q_OBJECT
public:
	enum ViewMode { Invalid = -1, Tabs = 0, OverPlot, MultiScans, MultiChannels };

    explicit AMScanView(QWidget *parent = 0);

signals:

public slots:

	/// change the view mode (newMode is a ViewMode enum: 0 for one channel at a time; 1 for channels overplotted; 2 for one plot per scan; 2 for one plot per channel.
	void changeViewMode(int newMode);

	/// add a scan to the view:
	void addScan(const AMScan* scan);

protected:

	/// helper function: remove all plot series:
	void removeAllPlotSeries();
	/// helper function: find out how many unique channels there are, and populate channelNames_;
	void uniqueChannelSearch();

	// ui components:
	QList<MPlotGW*> plots_;
	QGraphicsGridLayout* glayout_;
	int width_, rc_, cc_;// layout locations: width (num cols), rowcounter, columncounter.

	QVBoxLayout* barLayout_;

	AMScanViewModeBar* modeBar_;

	// data components:
	QList<AMScanViewEntry> scans_;
	ViewMode mode_;

	QStringList channelNames_;

	// build UI
	void setupUI();
	// setup all UI event-handling connections
	void makeConnections();



};

#endif // AMSCANVIEW_H
