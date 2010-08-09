#include "AMWorkflowManagerView.h"

AMWorkflowManagerView::AMWorkflowManagerView(QWidget *parent) :
	QWidget(parent)
{
	queueEmpty_ = true;

	data_ << "Workflow goes here ... just like food goes here";
	model_ = new QStringListModel();
	model_->setStringList(data_);
	listView_ = new QListView();
	listView_->setModel(model_);
	listView_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	workflowActions_ = new AMBeamlineActionsList(this);
	vl_ = new QVBoxLayout();
	vl_->addWidget(listView_);
	setLayout(vl_);
}

void AMWorkflowManagerView::onStartScanRequested(){
	if(isEmpty())
		emit freeToScan(true);
}
void AMWorkflowManagerView::onAddToQueueRequested(AMScanConfiguration *cfg){
	SGMXASScanConfiguration *sxsc = (SGMXASScanConfiguration*)cfg;
	QString range = "";
	QString tmpNum = "";
	tmpNum.setNum(sxsc->regions()->start(0));
	range = tmpNum;
	tmpNum.clear();
	tmpNum.setNum(sxsc->regions()->end(sxsc->regions()->count()-1));
	range.append(" to "+tmpNum);
	QStringList tmpList = model_->stringList();
	tmpList << "\nSGMXASScan from "+range;
	model_->setStringList(tmpList);
	AMBeamlineScanAction *scanAction = new AMBeamlineScanAction(sxsc, "SGMXASScan", "", this);
	workflowActions_->appendAction(scanAction);
//	scanAction->start();
}



AMBeamlineActionListModel::AMBeamlineActionListModel(QObject *parent)
{
	actions_ = new QList<AMBeamlineActionItem*>();
}

int AMBeamlineActionListModel::rowCount(const QModelIndex &index) const{
	return actions_->count();
}

QVariant AMBeamlineActionListModel::data(const QModelIndex &index, int role) const{
	if(!index.isValid())
		return QVariant();
	if(index.row() >= actions_->count())
		return QVariant();
	if(role == Qt::DisplayRole)
		return qVariantFromValue((void*)actions_->at(index.row()));
	else
		return QVariant();
}

QVariant AMBeamlineActionListModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}

bool AMBeamlineActionListModel::setData(const QModelIndex &index, const QVariant &value, int role){
	if (index.isValid()  && index.row() < actions_->count() && role == Qt::EditRole) {
		bool conversionOK = false;
		AMBeamlineActionItem* actionItem;
		actionItem = (AMBeamlineActionItem*) value.value<void*>();

		actions_->replace(index.row(), actionItem);
		return true;
	}
	return false;	// no value set
}

bool AMBeamlineActionListModel::insertRows(int position, int rows, const QModelIndex &index){
	if (index.row() <= actions_->count() && position <= actions_->count()) {
		beginInsertRows(QModelIndex(), position, position+rows-1);
		AMBeamlineActionItem *tmpAction = NULL;
		for (int row = 0; row < rows; ++row) {
			actions_->insert(position, tmpAction);
		}
		endInsertRows();
		return true;
	}
	return false;
}

bool AMBeamlineActionListModel::removeRows(int position, int rows, const QModelIndex &index){
	if (index.row() < actions_->count() && position < actions_->count()) {
		beginRemoveRows(QModelIndex(), position, position+rows-1);
		for (int row = 0; row < rows; ++row) {
			actions_->removeAt(position);
		}
		endRemoveRows();
		return true;
	}
	return false;
}

Qt::ItemFlags AMBeamlineActionListModel::flags(const QModelIndex &index) const{
	Qt::ItemFlags flags;
	if (index.isValid() && index.row() < actions_->count() && index.column()<4)
		flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return flags;
}

AMBeamlineActionsList::AMBeamlineActionsList(QObject *parent) :
		QObject(parent)
{
	actions_ = NULL;
	setupModel();
}

AMBeamlineActionItem* AMBeamlineActionsList::action(size_t index) const{
	QVariant retVal = actions_->data(actions_->index(index, 0), Qt::DisplayRole);
	if(retVal.isValid())
		return (AMBeamlineActionItem*) retVal.value<void*>();
	return NULL;
}

bool AMBeamlineActionsList::setAction(size_t index, AMBeamlineActionItem *action){
	return actions_->setData(actions_->index(index, 0), qVariantFromValue((void*)action), Qt::EditRole);
}

bool AMBeamlineActionsList::addAction(size_t index, AMBeamlineActionItem *action){
	bool retVal;
	if(!actions_->insertRows(index, 1))
		return false;
	setAction(index, action);
	return true;
}

bool AMBeamlineActionsList::appendAction(AMBeamlineActionItem *action){
	return addAction(count(), action);
}

bool AMBeamlineActionsList::deleteAction(size_t index){
	if(count() == 0)
		return false;
	return actions_->removeRows(index, 1);
}

bool AMBeamlineActionsList::setupModel(){
	actions_ = new AMBeamlineActionListModel(this);
	if(actions_){
		connect(actions_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)) );
		return true;
	}
	return false;
}

AMBeamlineActionsListView::AMBeamlineActionsListView(AMBeamlineActionsList *actionsList, QWidget *parent) :
		QWidget(parent)
{
	actionsList_ = actionsList;
	int nlSize = 600;
	this->setMaximumSize(nlSize, 300);
	this->setMinimumSize(nlSize, 300);
	scene = new QGraphicsScene(0, -10, nlSize-50, 260);

	redrawBeamlineActionsList();
	view = new QGraphicsView(scene);
	view->setRenderHint(QPainter::Antialiasing);
	view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	view->setBackgroundBrush(QColor(230, 200, 167));
	view->resize(nlSize-50, 260);
	view->setMaximumHeight(260);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QFormLayout *fl_ = new QFormLayout(this);
	fl_->addRow(view);
	setLayout(fl_);
	connect(actionsList_->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(handleDataChanged(QModelIndex,QModelIndex)));
	connect(actionsList_->model(), SIGNAL(rowsInserted(const QModelIndex,int,int)), this, SLOT(handleRowsInsert(QModelIndex,int,int)));
	connect(actionsList_->model(), SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(handleRowsRemoved(QModelIndex,int,int)));
}

void AMBeamlineActionsListView::handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight){
	redrawBeamlineActionsList();
}

void AMBeamlineActionsListView::handleRowsInsert(const QModelIndex &parent, int start, int end){
	redrawBeamlineActionsList();
}

void AMBeamlineActionsListView::handleRowsRemoved(const QModelIndex &parent, int start, int end){
	redrawBeamlineActionsList();
}

void AMBeamlineActionsListView::redrawBeamlineActionsList(){
	int nlSize = 600;
	scene->clear();
	for (int i = 0; i < actionsList_->count(); ++i) {
		BeamlineActionGraphicItem *item = new BeamlineActionGraphicItem(nlSize-60);
		item->setPos(10, i*30);
		scene->addItem(item);
	}
}

BeamlineActionGraphicItem::BeamlineActionGraphicItem(int width)
{
	color = QColor("blue");
	width_ = width;
}

void BeamlineActionGraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->setPen(Qt::NoPen);
	painter->setBrush(Qt::darkGray);
	painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
	painter->setPen(QPen(Qt::black, 1));
	painter->setBrush(Qt::gray);
	painter->drawRoundedRect(0, -15, width_, 30, 1, 1, Qt::RelativeSize);
	QString deltaVal;
	deltaVal.setNum(width_);
	QChar deltaChar(0x0394);
	deltaVal.prepend(" = ");
	deltaVal.prepend(deltaChar);
	QRectF box(0, -15, width_, 30);
	painter->drawText(box, Qt::AlignHCenter, deltaVal, &box);
}
