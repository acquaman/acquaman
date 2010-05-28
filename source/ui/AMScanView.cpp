#include "AMScanView.h"
#include <QGraphicsWidget>

QModelIndex AMScanSetModel::index ( int row, int column, const QModelIndex & parent ) const {
	// top level:
	if(!parent.isValid()) {
		if(column == 0 && row < scans_.count())
			return createIndex(row, column, -1);	// id of -1 in a model index indicates a Scan-level index. Row is the index of the scan.
		else
			return QModelIndex();
	}

	// Parent is a scan-level index
	else if(parent.internalId() == -1 && parent.column() == 0 && parent.row() < scans_.count() ) {
		if(column == 0 && row < scans_.at(parent.row())->numChannels() )
			return createIndex(row, column, parent.row() );
		else
			return QModelIndex();
	}

	// anything else (For parent-indices that correspond to channels, there are no children indices)
	return QModelIndex();
}

QModelIndex AMScanSetModel::parent ( const QModelIndex & index ) const {

	// scan-level indices: parent is the top level
	if(index.internalId() == -1)
		return QModelIndex();

	// if index is a channel level index: internalId is the index of its parent scan, which becomes the row in the parent index.
	if(index.isValid() && index.column() == 0 && index.internalId() >=0 && index.internalId() < scans_.count() )
		return createIndex(index.internalId(), 0, -1);

	return QModelIndex();
}

Qt::ItemFlags AMScanSetModel::flags ( const QModelIndex & index ) const  {

	// Scans:
	if(index.internalId() == -1)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	// Channels:
	if(index.internalId() >= 0)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;

	return Qt::NoItemFlags;
}

QVariant AMScanSetModel::data ( const QModelIndex & index, int role) const {
	if(!index.isValid())
		return QVariant();

	// scan-level index:
	if(index.internalId() == -1 && index.row() < scans_.count() && index.column() == 0) {
		AMScan* scan = scans_.at(index.row());

		switch(role) {
		case Qt::DisplayRole:
			return scan->name();
			break;
		case Qt::DecorationRole:
			return QVariant();
			break;
		case Qt::ToolTipRole:
			return QString("%1 (%2): %3").arg(scan->name()).arg(scan->sampleName()).arg(scan->dateTime().toString());
			break;
		case Qt::CheckStateRole:
			return QVariant();	/// \todo For now... No checking/unchecking scans.
			break;
		case Qt::UserRole:
			return qVariantFromValue(scan);
			break;
		default:
			return QVariant();
			break;
		}
	}


	// channel-level index:
	if(index.internalId() >= 0 && index.internalId() < scans_.count() ) {
		AMScan* scan = scans_.at(index.internalId());

		if(index.row() < scan->numChannels() && index.column() == 0) {
			AMChannel* channel = scan->channel(index.row());

			switch(role) {
			case Qt::DisplayRole:
				return channel->name();
				break;
			case Qt::DecorationRole:
				return channels_.at(index.internalId()).at(index.row()).color;
				break;
			case Qt::ToolTipRole:
				return QString("%1 (%2): %3").arg(channel->name()).arg(scan->name()).arg(channel->expression());
				break;
			case Qt::CheckStateRole:
				return channels_.at(index.internalId()).at(index.row()).visible ? Qt::Checked : Qt::Unchecked;
				break;
			case Qt::UserRole:
				return qVariantFromValue(channel);
				break;
			case PriorityRole:
				return channels_.at(index.internalId()).at(index.row()).priority;
				break;
			case LinePenRole:
				return channels_.at(index.internalId()).at(index.row()).linePen;
			default:
				return QVariant();
				break;
			}
		}
	}

	return QVariant();
}

QVariant AMScanSetModel::headerData ( int section, Qt::Orientation orientation, int role ) const {
	if(role != Qt::DisplayRole)
		return QVariant();
	if(orientation == Qt::Horizontal)
		return QString("Channel");
	if(orientation == Qt::Vertical)
		return QVariant(section);
	return QVariant();
}

int AMScanSetModel::rowCount ( const QModelIndex & parent ) const  {
	// top level: return number of scans
	if(!parent.isValid())
		return scans_.count();

	// scan-level: return number of channels:
	if(parent.internalId() == -1 && parent.row() < scans_.count())
		return scans_.at(parent.row())->numChannels();

	return 0;
}

int AMScanSetModel::columnCount ( const QModelIndex & parent ) const {
	Q_UNUSED(parent)

	return 1;
}

bool AMScanSetModel::hasChildren ( const QModelIndex & parent  ) const {
	// scans have children.
	if(parent.internalId() == -1)
		return true;
	// channels don't.
	else
		return false;
}


/// returns the index (or row) of an AMScan in the top-level. returns -1 if not found.
int AMScanSetModel::indexOf(AMScan* scan) const {
	return scans_.indexOf(scan);
}


// Resizable Interface:

// Add a scan to this model.  The AMScan must exist elsewhere, for the lifetime that it is added to the model.  Model does not take ownership of the scan.
void AMScanSetModel::addScan(AMScan* newScan) {
	beginInsertRows(QModelIndex(), scans_.count(), scans_.count());
	scans_.append(newScan);
	scanChannelLists_.append(newScan->channelList());

	QList<AMScanSetModelChannelMetaData> chs;
	for(int i=0; i<newScan->numChannels(); i++)
		chs.append(AMScanSetModelChannelMetaData());
	channels_.append(chs);

	/// \todo hook up signals from newScan->channelList to catch channel creation and deletion
	connect(newScan->channelList(), SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)), this, SLOT(onChannelAboutToBeAdded(QModelIndex, int, int)));
	connect(newScan->channelList(), SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onChannelAdded(QModelIndex, int, int)));
	connect(newScan->channelList(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onChannelAboutToBeRemoved(QModelIndex, int, int)));
	connect(newScan->channelList(), SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onChannelRemoved(QModelIndex, int, int)));
	endInsertRows();
}

// removes an AMScan from this model. Does not delete the scan.  Call this before deleting a scan that has been added to the model.
bool AMScanSetModel::removeScan(AMScan* removeMe) {
	int index = scans_.indexOf(removeMe);

	if(index != -1) {
		beginRemoveRows(QModelIndex(), index, index);
		scans_.removeAt(index);
		scanChannelLists_.removeAt(index);
		channels_.removeAt(index);
		endRemoveRows();
		return true;
	}
	else
		return false;
}


// the AMChannelListModel is a standard Qt model, but it guarantees that only one channel will be added at a time, and it will be added at the end of all rows(channels).
void AMScanSetModel::onChannelAboutToBeAdded(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(parent)

	AMChannelListModel* source = qobject_cast<AMChannelListModel*>(sender());

	int scanIndex = scanChannelLists_.indexOf(source);
	if(scanIndex == -1)
		return;

	beginInsertRows(index(scanIndex,0), start, end);

}

void AMScanSetModel::onChannelAdded(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(parent)

	AMChannelListModel* source = qobject_cast<AMChannelListModel*>(sender());
	int scanIndex = scanChannelLists_.indexOf(source);
	if(scanIndex == -1)
		return;

	for(int i=start; i<=end; i++)
		channels_[scanIndex].insert(i, AMScanSetModelChannelMetaData());

	endInsertRows();
}

void AMScanSetModel::onChannelAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(parent)

	AMChannelListModel* source = qobject_cast<AMChannelListModel*>(sender());
	int scanIndex = scanChannelLists_.indexOf(source);
	if(scanIndex == -1)
		return;

	beginRemoveRows(index(scanIndex,0), start, end);
}

void AMScanSetModel::onChannelRemoved(const QModelIndex& parent, int start, int end) {

	Q_UNUSED(parent)

	AMChannelListModel* source = qobject_cast<AMChannelListModel*>(sender());
	int scanIndex = scanChannelLists_.indexOf(source);
	if(scanIndex == -1)
		return;

	for(int i=end; i>=start; i--)
		channels_[scanIndex].removeAt(i);


	endRemoveRows();
}


AMChannelSelectorBar::AMChannelSelectorBar(const AMScan* source, QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* hl = new QHBoxLayout();
	nameLabel_ = new QLabel(source->name());
	nameLabel_->setStyleSheet("color: white;");
	hl->addWidget(nameLabel_);
	hl->addStretch(0.5);

	chButtonLayout_ = new QHBoxLayout();
	chButtonLayout_->setSpacing(0);
	hl->addLayout(chButtonLayout_);
	if(source) {
		for(int i=0; i<source->numChannels(); i++) {
			QToolButton* cb = new QToolButton();
			cb->setText(source->channel(i)->name());
			cb->setCheckable(true);
			chButtons_.addButton(cb, i);
			chButtonLayout_->addWidget(cb);
		}
	}
	hl->addStretch(1);

	closeButton_ = new QToolButton();
	closeButton_->setText("X");
	hl->addWidget(closeButton_);

	hl->setMargin(6);
	setLayout(hl);

	setObjectName("AMChannelSelectorBar");
	setStyleSheet("QWidget#AMChannelSelectorBar { "
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
		"border-bottom: 1px solid black;"
		"}");
}


AMScanViewModeBar::AMScanViewModeBar(QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* hl = new QHBoxLayout();
	QHBoxLayout* hl2 = new QHBoxLayout(), *hl3 = new QHBoxLayout();
	hl2->setSpacing(0);
	hl3->setSpacing(0);

	QToolButton* tabButton_ = new QToolButton();
	tabButton_->setText("1");
	QToolButton* overplotButton_ = new QToolButton();
	overplotButton_->setText("OP");
	QToolButton* multiScansButton_ = new QToolButton();
	multiScansButton_->setText("M-S");
	QToolButton* multiChannelsButton_ = new QToolButton();
	multiChannelsButton_->setText("M-C");

	tabButton_->setCheckable(true);
	overplotButton_->setCheckable(true);
	multiScansButton_->setCheckable(true);
	multiChannelsButton_->setCheckable(true);

	modeButtons_ = new QButtonGroup(this);
	modeButtons_->addButton(tabButton_, AMScanView::Tabs);
	modeButtons_->addButton(overplotButton_, AMScanView::OverPlot);
	modeButtons_->addButton(multiScansButton_, AMScanView::MultiScans);
	modeButtons_->addButton(multiChannelsButton_, AMScanView::MultiChannels);
	tabButton_->setChecked(true);

	hl2->addWidget(tabButton_);
	hl2->addWidget(overplotButton_);
	hl2->addWidget(multiScansButton_);
	hl2->addWidget(multiChannelsButton_);

	hl->addLayout(hl2);
	hl->addStretch(1);

	plusButton_ = new QToolButton();
	plusButton_->setText("+");
	hl3->addWidget(plusButton_);
	subtractButton_ = new QToolButton();
	subtractButton_->setText("-");
	hl3->addWidget(subtractButton_);

	hl->addLayout(hl3);
	hl->setMargin(6);
	hl->setSpacing(24);
	setLayout(hl);

	setObjectName("AMScanViewModeBar");
	setStyleSheet("QWidget#AMScanViewModeBar { "
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
		"border-bottom: 1px solid black;"
		"}");



}

AMScanView::AMScanView(QWidget *parent) :
    QWidget(parent)
{
	mode_ = Invalid;
	width_ = 1;
	rc_ = cc_ = 0;

	setupUI();
	makeConnections();

	changeViewMode(Tabs);
}


void AMScanView::setupUI() {
	QVBoxLayout* vl = new QVBoxLayout();
	vl->setMargin(6);
	vl->setSpacing(6);

	AMScanViewMainWidget* mw = new AMScanViewMainWidget();
	mw->setMinimumSize(120,120);

	vl->addWidget(mw);

	barLayout_ = new QVBoxLayout();
	barLayout_->setSpacing(0);
	vl->addLayout(barLayout_);

	modeBar_ = new AMScanViewModeBar();
	vl->addWidget(modeBar_);

	setLayout(vl);


	// setup grid layout within main plot area:

	glayout_ = new QGraphicsGridLayout();
	glayout_->setSpacing(12);
	glayout_->setContentsMargins(6,6,6,6);
	mw->graphicsWidget()->setLayout(glayout_);

}


// newMode:
void AMScanView::changeViewMode(int newMode) {

	// nothing changing:
	if(newMode == mode_)
		return;

	// if we're having to change the number of plots, we need to delete and remove all the plot series (otherwise some will be deleted with their plot parents)
	removeAllPlotSeries();

	// starting point for all: all series have been removed from their plots. However, an uncertain number of plots may still exist in the layout.

	switch(newMode) {
	case Tabs:
		{
			// remove plots until there is only one left:
			while(plots_.count() > 1)
				delete plots_.takeLast();

			// add a plot if we don't have any yet.
			if(plots_.count() == 0) {
				plots_ << new MPlotGW();
				plots_[0]->plot()->setScalePadding(5);
				plots_[0]->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
				glayout_->addItem(plots_[0], 0, 0, Qt::AlignCenter);
			}

			// for all associated scans: add primary channel
			for(int i=0; i<scans_.count(); i++) {
				AMScanViewEntry& scan = scans_[i];


				AMScanViewChannelEntry& ch = scan.chList[scan.primaryChannel];
				if(ch.plotSeries == 0) {
					ch.plotSeries = new MPlotSeriesBasic();
				}
				ch.plotSeries->setModel(scan.scan->channel(scan.primaryChannel));
				plots_[0]->plot()->addItem(ch.plotSeries);
				ch.destinationPlot = 0;
				// todo: colorize?
			}

			mode_ = Tabs;
		}
		break;

	case OverPlot:
		{
			// remove plots until there is only one left:
			while(plots_.count() > 1)
				delete plots_.takeLast();

			// add a plot if we don't have any yet.
			if(plots_.count() == 0) {
				plots_ << new MPlotGW();
				plots_[0]->plot()->setScalePadding(5);
				plots_[0]->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
				glayout_->addItem(plots_[0], 0, 0, Qt::AlignCenter);
			}

			// for all associated scans: add all visible channels:
			for(int j=0; j<scans_.count(); j++) {
				AMScanViewEntry& scan = scans_[j];

				for(int i=0; i<scan.chList.count(); i++) {
					AMScanViewChannelEntry& ch = scan.chList[i];
					if(ch.visible == false)
						continue;
					if(ch.plotSeries == 0) {
						ch.plotSeries = new MPlotSeriesBasic();
					}
					ch.plotSeries->setModel(scan.scan->channel(i));
					plots_[0]->plot()->addItem(ch.plotSeries);
					ch.destinationPlot = 0;
					// todo: colorize?
				}
			}

			mode_ = OverPlot;
		}
		break;

	// multi-scans mode: every scan is on a separate plot (every visible channel is shown). We need as many plots as there are scans.
	case MultiScans:
		{
			// remove all plots:
			while(!plots_.isEmpty() > 0)
				delete plots_.takeLast();

			width_ = sqrt(scans_.count());
			rc_ = 0; cc_ = 0;
			// add a plot if we don't have enough yet
			while(plots_.count() < scans_.count()) {
				MPlotGW* newPlot = new MPlotGW();
				plots_ << newPlot;
				newPlot->plot()->setScalePadding(5);
				newPlot->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
				glayout_->addItem(newPlot, rc_, cc_++, Qt::AlignCenter);
				if(cc_ == width_) {
					rc_++;
					cc_ = 0;
				}
			}

			// for all associated scans: add all visible channels:
			for(int j=0; j<scans_.count(); j++) {
				AMScanViewEntry& scan = scans_[j];

				for(int i=0; i<scan.chList.count(); i++) {
					AMScanViewChannelEntry& ch = scan.chList[i];
					if(ch.visible == false)
						continue;
					if(ch.plotSeries == 0) {
						ch.plotSeries = new MPlotSeriesBasic();
					}
					ch.plotSeries->setModel(scan.scan->channel(i));
					plots_[j]->plot()->addItem(ch.plotSeries);
					ch.destinationPlot = j;
					// todo: colorize?
				}
			}

			mode_ = MultiScans;
		}
		break;

	case MultiChannels:
		{

			// how many different channels are there?
			uniqueChannelSearch();

			// remove all plots:
			while(!plots_.isEmpty() > 0)
				delete plots_.takeLast();

			width_ = sqrt(channelNames_.count());
			rc_ = 0; cc_ = 0;
			// add a plot if we don't have enough yet
			while(plots_.count() < channelNames_.count()) {
				MPlotGW* newPlot = new MPlotGW();
				plots_ << newPlot;
				newPlot->plot()->setScalePadding(5);
				newPlot->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
				glayout_->addItem(newPlot, rc_, cc_++, Qt::AlignCenter);
				if(cc_ == width_) {
					rc_++;
					cc_ = 0;
				}
			}

			// for all associated scans: add all visible channels:
			for(int j=0; j<scans_.count(); j++) {
				AMScanViewEntry& scan = scans_[j];

				for(int i=0; i<scan.chList.count(); i++) {
					AMScanViewChannelEntry& ch = scan.chList[i];
					if(ch.visible == false)
						continue;
					if(ch.plotSeries == 0) {
						ch.plotSeries = new MPlotSeriesBasic();
					}
					ch.plotSeries->setModel(scan.scan->channel(i));
					int plotIndex = channelNames_.indexOf(scan.scan->channel(i)->name());
					plots_[plotIndex]->plot()->addItem(ch.plotSeries);
					ch.destinationPlot = plotIndex;
					// todo: colorize?
				}
			}

			mode_ = MultiChannels;
		}
		break;
	default:
		break;
	}

	// in case this was called programmatically (instead of by clicking on the button)... the mode button won't be set.  This will re-emit the mode-change signal, but this function will exit immediately on the second time because it's already in the correct mode.
	modeBar_->modeButtons_->button(mode_)->setChecked(true);
}

void AMScanView::makeConnections() {

	// connect mode bar to changeViewMode:
	connect(modeBar_->modeButtons_, SIGNAL(buttonClicked(int)), this, SLOT(changeViewMode(int)));
}

// removes all existing plotSeries from their plots.  This keeps the series alive; does not delete; only removes from parent plot so we can delete the parent plot.
void AMScanView::removeAllPlotSeries() {

	for(int j=0; j<scans_.count(); j++) {
		AMScanViewEntry& scan = scans_[j];

		for(int i=0; i<scan.chList.count(); i++) {
			AMScanViewChannelEntry& ch = scan.chList[i];
			if(ch.plotSeries && ch.destinationPlot >= 0) {
				plots_[ch.destinationPlot]->plot()->removeItem(ch.plotSeries);
				ch.destinationPlot = -1;
			}
		}
	}
}


void AMScanView::uniqueChannelSearch() {

	// unique channels:
	QSet<QString> uchannels;

	for(int j=0; j<scans_.count(); j++)
		for(int i=0; i<scans_[j].scan->numChannels(); i++)
			if(scans_[j].chList[i].visible && !uchannels.contains(scans_[j].scan->channel(i)->name()))
				uchannels.insert(scans_[j].scan->channel(i)->name());

	channelNames_ = uchannels.toList();
}


void AMScanView::addScan(const AMScan* scan) {
	AMScanViewEntry e;

	e.scan = scan;
	e.scanBar = new AMChannelSelectorBar(scan);
	barLayout_->addWidget(e.scanBar);
	// todo: hookup connections for clicks on channel buttons.

	for(int i=0; i<scan->numChannels(); i++) {
		AMScanViewChannelEntry ch;
		// todo: defaults for which channels are visible, which aren't.
		// todo: defaults for which is the primary channel.
		e.chList.append(ch);
	}



	// insert handling depends on mode_ from here on:
	switch(mode_) {
	case Tabs:
		{
			AMScanViewChannelEntry& ch = e.chList[e.primaryChannel];
			if(ch.plotSeries == 0) {
				ch.plotSeries = new MPlotSeriesBasic();
			}
			ch.plotSeries->setModel(e.scan->channel(e.primaryChannel));
			plots_[0]->plot()->addItem(ch.plotSeries);
			ch.destinationPlot = 0;

			scans_.append(e);
			e.scanBar->chButtons_.button(e.primaryChannel)->setChecked(true);
		}
		break;

	case OverPlot:
		{
			e.scanBar->chButtons_.setExclusive(false);

			for(int i=0; i<e.chList.count(); i++) {
				AMScanViewChannelEntry& ch = e.chList[i];
				if(ch.visible == false) {
					e.scanBar->chButtons_.button(i)->setChecked(false);
					continue;
				}
				e.scanBar->chButtons_.button(i)->setChecked(true);
				if(ch.plotSeries == 0) {
					ch.plotSeries = new MPlotSeriesBasic();
				}
				ch.plotSeries->setModel(e.scan->channel(i));
				plots_[0]->plot()->addItem(ch.plotSeries);
				ch.destinationPlot = 0;
				// todo: colorize?
			}

			scans_.append(e);
		}
		break;

	case MultiScans:
		{
			e.scanBar->chButtons_.setExclusive(false);

			// going to need one more plot...
			MPlotGW* newPlot = new MPlotGW();
			plots_ << newPlot;
			newPlot->plot()->setScalePadding(5);
			newPlot->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
			glayout_->addItem(newPlot, rc_, cc_++, Qt::AlignCenter);
			if(cc_ == width_) {
				rc_++;
				cc_ = 0;
			}


			for(int i=0; i<e.chList.count(); i++) {
				AMScanViewChannelEntry& ch = e.chList[i];
				if(ch.visible == false) {
					e.scanBar->chButtons_.button(i)->setChecked(false);
					continue;
				}
				e.scanBar->chButtons_.button(i)->setChecked(true);
				if(ch.plotSeries == 0) {
					ch.plotSeries = new MPlotSeriesBasic();
				}
				ch.plotSeries->setModel(e.scan->channel(i));
				plots_[plots_.count()-1]->plot()->addItem(ch.plotSeries);
				ch.destinationPlot = plots_.count()-1;
				// todo: colorize?
			}

			scans_.append(e);

		}
		break;

	case MultiChannels:
		{

			e.scanBar->chButtons_.setExclusive(false);

			// see if this introduces a new channel that we don't have already:
			QSet<QString> newChannels = QSet<QString>::fromList(scan->channelNames());
			for(int i=0; i<e.chList.count(); i++)
				if(!e.chList[i].visible)
					newChannels.remove(scan->channel(i)->name());

			QSet<QString> oldChannels = QSet<QString>::fromList(channelNames_);
			newChannels.subtract(oldChannels);
			channelNames_.append(newChannels.toList());

			foreach(QString newChannel, newChannels) {
				MPlotGW* newPlot = new MPlotGW();
				plots_ << newPlot;
				newPlot->plot()->setScalePadding(5);
				newPlot->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
				glayout_->addItem(newPlot, rc_, cc_++, Qt::AlignCenter);
				// todo: problem where we don't recalcualte width. Will keep on adding with current width.
				// fix by switching to flowlayout.
				if(cc_ == width_) {
					rc_++;
					cc_ = 0;
				}
			}

			// add all visible channels:

			for(int i=0; i<e.chList.count(); i++) {
				AMScanViewChannelEntry& ch = e.chList[i];
				if(ch.visible == false) {
					e.scanBar->chButtons_.button(i)->setChecked(false);
					continue;
				}
				e.scanBar->chButtons_.button(i)->setChecked(true);
				if(ch.plotSeries == 0) {
					ch.plotSeries = new MPlotSeriesBasic();
				}
				ch.plotSeries->setModel(e.scan->channel(i));
				int plotIndex = channelNames_.indexOf(e.scan->channel(i)->name());
				plots_[plotIndex]->plot()->addItem(ch.plotSeries);
				ch.destinationPlot = plotIndex;
				// todo: colorize?
			}

			scans_.append(e);
		}
		break;

	default:
		changeViewMode(Tabs);
		break;

	}
}


