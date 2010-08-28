#include "AMSamplePlateView.h"

AMSamplePlateView::AMSamplePlateView(QWidget *parent) :
	QWidget(parent)
{
	sampleRefreshScheduled_ = false;
	manipulator_ = NULL;

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	plateNameLabel_ = new QLabel(samplePlate_.userName());
	plateNameLabel_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	loadedLabel_ = new QLabel("   originally loaded "+samplePlate_.createTime());
	existingPlates_ = new QComboBox();
	existingPlates_->addItem("Load Existing");

	QSqlQuery q2 = AMDatabase::userdb()->query();
	q2.prepare(QString("SELECT id,name,createTime FROM %1 ORDER BY createTime DESC").arg(AMDatabaseDefinition::samplePlateTableName()));
	q2.exec();
	int id;
	QString name;
	QDateTime createTime;
	while(q2.next()) {
		id = q2.value(0).toInt();
		name = q2.value(1).toString();
		createTime = q2.value(2).toDateTime();
		existingPlates_->insertItem(1, name);
		existingPlates_->setItemData(1, id, AM::IdRole);
		existingPlates_->setItemData(1, createTime, AM::DateTimeRole);
		existingPlates_->setItemData(1, createTime.toString("MMM d yyyy, h:mm ap"), AM::DescriptionRole);
	}
	connect(existingPlates_, SIGNAL(currentIndexChanged(int)), this, SLOT(onLoadExistingPlate(int)));

	connect(AMDatabase::userdb(), SIGNAL(updated(QString,int)), this, SLOT(onSampleTableItemUpdated(QString,int)));
	connect(AMDatabase::userdb(), SIGNAL(created(QString,int)), this, SLOT(onSampleTableItemCreated(QString,int)));
	connect(AMDatabase::userdb(), SIGNAL(removed(QString,int)), this, SLOT(onSampleTableItemRemoved(QString,int)));

	sampleTableModel_ = new QStandardItemModel();
	QStandardItem *tmpItem;
	QSqlQuery q = AMDatabase::userdb()->query();
	q.prepare(QString("SELECT id,name,dateTime FROM %1 ORDER BY dateTime DESC").arg(AMDatabaseDefinition::sampleTableName()));
	q.exec();
	QDateTime dateTime;
	while(q.next()){
		id = q.value(0).toInt();
		name = q.value(1).toString();
		dateTime = q.value(2).toDateTime();
		tmpItem = new QStandardItem(name);
		tmpItem->setData(id, AM::IdRole);
		tmpItem->setData(dateTime, AM::DateTimeRole);
		tmpItem->setData(dateTime.toString("MMM d yyyy, h:mm ap"), AM::DescriptionRole);
		sampleTableModel_->setItem(sampleTableModel_->rowCount(), 0, tmpItem);
		QString tmpStr;
		tmpItem = new QStandardItem(tmpStr.setNum(id));
		sampleTableModel_->setItem(sampleTableModel_->rowCount()-1, 1, tmpItem);
	}
	sampleListView_ = new AMSampleListView(&samplePlate_, sampleTableModel_);
	sampleListView_->setManipulator(manipulator_);

	vl_ = new QVBoxLayout();
	vl_->setAlignment(Qt::AlignTop);
	vl_->addWidget(plateNameLabel_);
	vl_->addWidget(loadedLabel_);
	vl_->addWidget(existingPlates_);
	vl_->addWidget(sampleListView_);
	setLayout(vl_);
}

void AMSamplePlateView::setManipulator(AMControlSet *manipulator){
	manipulator_ = manipulator;
	sampleListView_->setManipulator(manipulator_);
}

void AMSamplePlateView::onLoadExistingPlate(int index){
	int id = existingPlates_->itemData(index, AM::IdRole).toInt();
	if(id == 0)
		return;

	samplePlate_.loadFromDb(AMDatabase::userdb(), id);
}

void AMSamplePlateView::onSampleTableItemUpdated(QString tableName, int id){
	if(tableName != AMDatabaseDefinition::sampleTableName())
		return;
	sampleRefreshIDs_.append(id);
	sampleRefreshInstructions_.append(0); //Instruction 0 is update
	if(!sampleRefreshScheduled_){
		sampleRefreshScheduled_ = true;
		QTimer::singleShot(0, this, SLOT(refreshSamples()));
	}
}

void AMSamplePlateView::onSampleTableItemCreated(QString tableName, int id){
	if(tableName != AMDatabaseDefinition::sampleTableName())
		return;
	QVariant vSampleName, vSampleDateTime;
	QList<QVariant*> sampleValues;
	sampleValues << &vSampleName << &vSampleDateTime;
	QStringList colNames;
	colNames << "name" << "dateTime";
	if(!AMDatabase::userdb()->retrieve(id, tableName, colNames, sampleValues)){
		qDebug() << "Could not retrieve new sample from db";
		return;
	}
	QStandardItem *tmpItem = new QStandardItem(sampleValues.at(0)->toString());
	tmpItem->setData(id, AM::IdRole);
	tmpItem->setData(sampleValues.at(1)->toDateTime(), AM::DateTimeRole);
	tmpItem->setData(sampleValues.at(1)->toDateTime().toString("MMM d yyyy, h:mm ap"), AM::DescriptionRole);
	sampleTableModel_->insertRow(0);
	sampleTableModel_->setItem(0, 0, tmpItem);
	QString tmpStr;
	tmpItem = new QStandardItem(tmpStr.setNum(id));
	sampleTableModel_->setItem(0, 1, tmpItem);
}

void AMSamplePlateView::onSampleTableItemRemoved(QString tableName, int id){
	if(tableName != AMDatabaseDefinition::sampleTableName())
		return;
}

void AMSamplePlateView::refreshSamples(){
	QString idsToSearch;
	QString tmpStr, name;
	int id;
	QDateTime dateTime;
	QSet<int> noDups = sampleRefreshIDs_.toSet();
	QSet<int>::const_iterator i = noDups.constBegin();
	while(i != noDups.constEnd()){
		idsToSearch.append(tmpStr.setNum(*i)+",");
		++i;
	}
	idsToSearch.remove(idsToSearch.count()-1, 1);

	QSqlQuery q = AMDatabase::userdb()->query();
	q.prepare(QString("SELECT id,name,dateTime FROM %1 WHERE id IN (%2)").arg(AMDatabaseDefinition::sampleTableName()).arg(idsToSearch)) ;
	if(!q.exec())
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Could not retrieve sample update information from the database."));
	while(q.next()) {
		id = q.value(0).toInt();
		name = q.value(1).toString();
		dateTime = q.value(2).toDateTime();
		QList<QStandardItem*> items = sampleTableModel_->findItems(tmpStr.setNum(id), Qt::MatchExactly, 1);
		if(items.count() == 1){
			QStandardItem *tmpItem = sampleTableModel_->item(items.at(0)->index().row(), 0);
			if(tmpItem->text() != name)
				tmpItem->setText(name);
			if(tmpItem->data(AM::IdRole).toInt() != id)
				tmpItem->setData(id, AM::IdRole);
			if(tmpItem->data(AM::DateTimeRole).toDateTime() != dateTime)
				tmpItem->setData(dateTime, AM::DateTimeRole);
		}
	}

	sampleRefreshIDs_.clear();
	sampleRefreshInstructions_.clear();
	sampleRefreshScheduled_ = false;
}

AMSampleListView::AMSampleListView(AMSamplePlate *samplePlate, QStandardItemModel *sampleTableModel, QWidget *parent) :
		QFrame(parent)
{
	samplePlate_ = samplePlate;
	sampleTableModel_ = sampleTableModel;
	connect(samplePlate_, SIGNAL(samplePositionChanged(int)), this, SLOT(onSamplePositionChanged(int)));
	connect(samplePlate_, SIGNAL(samplePositionAdded(int)), this, SLOT(onSamplePositionAdded(int)));
	connect(samplePlate_, SIGNAL(samplePositionRemoved(int)), this, SLOT(onSamplePositionRemoved(int)));

	saf_ = new QFrame();
	il_ = new QVBoxLayout();
	il_->setAlignment(Qt::AlignTop);
	saf_->setLayout(il_);
	saf_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	sa_ = new QScrollArea();
	sa_->setWidget(saf_);
	sa_->setWidgetResizable(true);
	sa_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	QVBoxLayout *ol = new QVBoxLayout();
	ol->addWidget(sa_);
	setLayout(ol);

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	adder_ = new AMSamplePositionItemExpandingAdder(sampleTableModel_);
	adder_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	connect(adder_, SIGNAL(sampleToAddChosen(int)), this, SLOT(addNewSampleToPlate(int)));
	il_->addWidget(adder_);
}

void AMSampleListView::addNewSampleToPlate(int id){ //HEY DAVE, OPTIONALLY TAKE AN AMSAMPLE POINTER? IF YOU JUST MADE IT WHY RETRIEVE IT?
	if(!manipulator_)
		return;
	AMSample *tmpSample = new AMSample("", this);
	tmpSample->loadFromDb(AMDatabase::userdb(), id);
	AMControlSetInfo *tmpPosition = new AMControlSetInfo(manipulator_->info(), this);
	tmpPosition->storeToDb(AMDatabase::userdb());
	samplePlate_->appendSamplePosition(tmpSample, tmpPosition);
	samplePlate_->storeToDb(AMDatabase::userdb());
}

void AMSampleListView::setManipulator(AMControlSet *manipulator){
	manipulator_ = manipulator;
}

void AMSampleListView::onSamplePositionChanged(int index){
	samplePlate_->storeToDb(AMDatabase::userdb());

}

void AMSampleListView::onSamplePositionAdded(int index){
	if(!manipulator_ || !sampleTableModel_)
		return;
	if(adder_->expanded())
		adder_->resetAdder();
	AMSamplePositionItemView *tmpSPIView = new AMSamplePositionItemView(samplePlate_->samplePositionAt(index), sampleTableModel_, manipulator_, index+1);
	il_->insertWidget(index, tmpSPIView, 0, Qt::AlignTop);
	tmpSPIView->setFocus();
	sa_->ensureVisible(0, 0);
}

void AMSampleListView::onSamplePositionRemoved(int index){
	if(il_->itemAt(index) == 0)
		return;
	AMSamplePositionItemView *tmpSPIView = (AMSamplePositionItemView*)il_->itemAt(index)->widget();
	delete tmpSPIView;
}

QSize AMSampleListView::sizeHint() const{
	QSize tmpSize = QFrame::sizeHint();
	int newWidth;
	if(sa_->verticalScrollBar() && sa_->verticalScrollBar()->isVisible())
		newWidth = 1.1*(tmpSize.width()+sa_->verticalScrollBar()->width());
	else
		newWidth = tmpSize.width()*1.1;
	tmpSize.setWidth(newWidth);
	return tmpSize;
}

AMSamplePositionItemView::AMSamplePositionItemView(AMSamplePosition *samplePosition, QStandardItemModel *sampleTableModel, AMControlSet *manipulator, int index, QWidget *parent) :
		QFrame(parent)
{
	index_ = index;
	inFocus_ = false;
	ignoreNameChanged_ = false;
	setFocusPolicy(Qt::StrongFocus);
	samplePosition_ = samplePosition;
	manipulator_ = manipulator;
	sampleTableModel_ = sampleTableModel;
	connect(sampleTableModel, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onSampleTableRowAdded(QModelIndex,int,int)));

	vl_ = NULL;
	hl_ = NULL;
	indexLabel_ = NULL;
	sampleBox_ = NULL;
	positionLabel_ = NULL;
	savePositionButton_ = NULL;
	recallPositionButton_ = NULL;

	setLineWidth(1);
	setFrameStyle(QFrame::StyledPanel);

	onSamplePositionUpdate(-1);
	if(samplePosition_)
		connect(samplePosition_, SIGNAL(positionValuesChanged(int)), this, SLOT(onSamplePositionUpdate(int)));
}

int AMSamplePositionItemView::index(){
	return index_;
}

AMControlSet* AMSamplePositionItemView::manipulator(){
	return manipulator_;
}

void AMSamplePositionItemView::setIndex(int index){
	index_ = index;
}

void AMSamplePositionItemView::setManipulator(AMControlSet *manipulator){
	manipulator_ = manipulator;
}

bool AMSamplePositionItemView::onSavePositionClicked(){
	if(!manipulator_)
		return false;
	samplePosition_->position()->copyFrom(manipulator_->info());
	samplePosition_->position()->storeToDb(AMDatabase::userdb());
	return true;
}

bool AMSamplePositionItemView::onRecallPositionClicked(){
	if(!manipulator_)
		return false;
	manipulator_->setFromInfo(samplePosition_->position());
	return true;
}

bool AMSamplePositionItemView::onSampleBoxIndexChanged(int index){
	ignoreNameChanged_ = true;
	int newId = sampleTableModel_->item(index, 0)->data(AM::IdRole).toInt();
	return samplePosition_->sample()->loadFromDb(AMDatabase::userdb(), newId);
}

bool AMSamplePositionItemView::onSampleNameChanged(){
	if(ignoreNameChanged_){
		ignoreNameChanged_ = false;
		return false;
	}
	if(sampleBox_->currentText() == samplePosition_->sample()->name())
		return false;
	samplePosition_->sample()->setName(sampleBox_->currentText());
	return samplePosition_->sample()->storeToDb(AMDatabase::userdb());
}

void AMSamplePositionItemView::updateLook(){
	if(inFocus_)
		setFrameStyle(QFrame::Box);
	if(inFocus_)
		setStyleSheet("AMSamplePositionItemView { background : rgb(194, 210, 215) }");
	else{
		setStyleSheet("AMSamplePositionItemView { background : rgb(230, 222, 214) }");
		setFrameStyle(QFrame::StyledPanel);
	}
	if(inFocus_ && (vl_->indexOf(positionLabel_) == -1) ){
		vl_->addWidget(positionLabel_);
		positionLabel_->show();
	}
	else if(!inFocus_ && (vl_->indexOf(positionLabel_) != -1) ){
		vl_->removeWidget(positionLabel_);
		positionLabel_->hide();
	}
}

void AMSamplePositionItemView::onSamplePositionUpdate(int index){
	if(!samplePosition_)
		return;
	if(!vl_){
		vl_ = new QVBoxLayout();
		setLayout(vl_);
	}
	if(!hl_){
		hl_ = new QHBoxLayout();
		vl_->addLayout(hl_);
	}
	if(!indexLabel_){
		indexLabel_ = new QLabel();
		indexLabel_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
		hl_->addWidget(indexLabel_, 0, Qt::AlignLeft);
	}
	if(!sampleBox_){
		sampleBox_ = new QComboBox();
		sampleBox_->setModel(sampleTableModel_);
		sampleBox_->view()->setItemDelegate(new AMDetailedItemDelegate(this));
		sampleBox_->view()->setAlternatingRowColors(true);
		sampleBox_->setEditable(true);
		sampleBox_->setInsertPolicy(QComboBox::InsertAtCurrent);
		sampleBox_->setCompleter(NULL);
		sampleBox_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
		sampleBox_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
		setFocusProxy(sampleBox_);
		connect(sampleBox_, SIGNAL(activated(int)), this, SLOT(onSampleBoxIndexChanged(int)));
		connect(sampleBox_->lineEdit(), SIGNAL(editingFinished()), this, SLOT(onSampleNameChanged()));
		hl_->addWidget(sampleBox_, 3, Qt::AlignLeft);
	}
	if(!positionLabel_){
		positionLabel_ = new QLabel(this);
		positionLabel_->hide();
	}
	if(!savePositionButton_){
		savePositionButton_ = new QPushButton(QIcon(":/save.png"), "");
		savePositionButton_->setToolTip("Save Current Position");
		connect(savePositionButton_, SIGNAL(clicked()), this, SLOT(onSavePositionClicked()));
		hl_->addWidget(savePositionButton_, 0, Qt::AlignRight);
	}
	if(!recallPositionButton_){
		recallPositionButton_ = new QPushButton(QIcon(":/go-next.png"), "");
		recallPositionButton_->setToolTip("Move to Saved Position");
		connect(recallPositionButton_, SIGNAL(clicked()), this, SLOT(onRecallPositionClicked()));
		hl_->addWidget(recallPositionButton_, 0, Qt::AlignRight);
	}
	QString tmpStr;
	tmpStr.setNum(index_);
	indexLabel_->setText(tmpStr+". ");
	sampleBox_->setCurrentIndex( sampleBox_->findText(samplePosition_->sample()->name()) );
	sampleBox_->lineEdit()->selectAll();

	QString positionText;
	for(int x = 0; x < samplePosition_->position()->count(); x++){
		tmpStr.setNum(samplePosition_->position()->valueAt(x));
		tmpStr.prepend(": ");
		tmpStr.prepend(samplePosition_->position()->nameAt(x));
		tmpStr.append("\n");
		positionText.append(tmpStr);
	}
	positionLabel_->setText(positionText);
}

void AMSamplePositionItemView::defocusItem(){
	inFocus_ = false;
	updateLook();
}

void AMSamplePositionItemView::mousePressEvent(QMouseEvent *event){
	if (event->button() != Qt::LeftButton) {
		event->ignore();
		return;
	}
	if(inFocus_)
		defocusItem();
	else{
		inFocus_ = true;
		updateLook();
		//emit focusRequested(action_);
	}
}

AMSamplePositionItemExpandingAdder::AMSamplePositionItemExpandingAdder(QStandardItemModel *sampleTableModel, QWidget *parent) :
		QFrame(parent)
{
	sampleTableModel_ = sampleTableModel;

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	markNewButton_ = new QPushButton(QIcon(":/add.png"), "Mark a New Sample Position ...");
	markNewButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	newNameEdit_ = new QLineEdit();
	newNameEdit_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setFocusProxy(newNameEdit_);
	newNameLabel_ = new QLabel("Create a New Sample");
	chooseExistingBox_ = new QComboBox();
	chooseExistingBox_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	chooseExistingBox_->setModel(sampleTableModel_);
	emptyChooseExistingBox_ = new AMTrickComboBox();
	emptyChooseExistingBox_->addItem("Or Pick Existing Sample");
	emptyChooseExistingBox_->setCurrentIndex(0);
	goNewButton_ = new QPushButton(QIcon(":/add.png"), "Add");
	goNewButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	goExistingButton_ = new QPushButton(QIcon(":/add.png"), "Add");
	goExistingButton_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	cancelButton_ = new QPushButton(QIcon(":/Close.png"), "");

	connect(markNewButton_, SIGNAL(clicked()), this, SLOT(onMarkNewButtonClicked()));
	connect(goNewButton_, SIGNAL(clicked()), this, SLOT(onGoNewButtonClicked()));
	connect(goExistingButton_, SIGNAL(clicked()), this, SLOT(onGoExistingButtonClicked()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(shrinkBack()));
	connect(emptyChooseExistingBox_, SIGNAL(clicked()), this, SLOT(switchBoxes()));

	gl_ = new QGridLayout();
	gl_->addWidget(markNewButton_, 0, 0, 1, 1, Qt::AlignLeft);
	gl_->setColumnStretch(0, 5);
	setLayout(gl_);

	setFrameStyle(QFrame::NoFrame);
}

const bool AMSamplePositionItemExpandingAdder::expanded() const{
	if(markNewButton_->isVisible())
		return false;
	else
		return true;
}

void AMSamplePositionItemExpandingAdder::resetAdder(){
	shrinkBack();
}

void AMSamplePositionItemExpandingAdder::onMarkNewButtonClicked(){
	//HEY DAVE, MAYBE CAN WATCH DB SIGNALS SO ONLY QUERY WHEN CHANGES MADE NOT BY YOU
	QSqlQuery q = AMDatabase::userdb()->query();
	q.prepare(QString("SELECT name FROM %1 WHERE name LIKE 'Sample %'").arg(AMDatabaseDefinition::sampleTableName()));
	q.exec();
	int index;
	int max = 0;
	bool convOK;
	while(q.next()) {
		index = q.value(0).toString().remove("Sample ").toInt(&convOK);
		if(convOK && (index > max) )
			max = index;
	}
	QString tmpStr;
	tmpStr.setNum(max+1);
	newNameEdit_->setText("Sample "+tmpStr);
	chooseExistingBox_->setCurrentIndex(0);

	gl_->removeWidget(markNewButton_);
	markNewButton_->hide();
	gl_->addWidget(newNameLabel_, 0, 0, 1, 1, Qt::AlignLeft);
	gl_->addWidget(newNameEdit_, 1, 0, 1, 1, Qt::AlignLeft);
	gl_->addWidget(emptyChooseExistingBox_, 2, 0, 1, 1, Qt::AlignLeft);
	gl_->addWidget(goNewButton_, 1, 1, 1, 1, Qt::AlignLeft);
	gl_->addWidget(goExistingButton_, 2, 1, 1, 1, Qt::AlignLeft);
	gl_->addWidget(cancelButton_, 0, 1, 1, 1, Qt::AlignRight);
	gl_->setColumnStretch(0, 5);
	gl_->setColumnStretch(1, 0);
	newNameEdit_->show();
	newNameLabel_->show();
	emptyChooseExistingBox_->show();
	goNewButton_->show();
	goExistingButton_->show();
	cancelButton_->show();
	setFrameStyle(QFrame::StyledPanel);
	newNameEdit_->selectAll();
	newNameEdit_->setFocus();
}

void AMSamplePositionItemExpandingAdder::onGoNewButtonClicked(){
	AMSample *tmpSample = new AMSample(newNameEdit_->text(), this);
	tmpSample->storeToDb(AMDatabase::userdb());
	shrinkBack();
	emit sampleToAddChosen(tmpSample->id());
}

void AMSamplePositionItemExpandingAdder::onGoExistingButtonClicked(){
	shrinkBack();
	emit sampleToAddChosen(sampleTableModel_->data(sampleTableModel_->index(chooseExistingBox_->currentIndex(), 0), AM::IdRole).toInt());
}

void AMSamplePositionItemExpandingAdder::shrinkBack(){
	gl_->removeWidget(newNameEdit_);
	gl_->removeWidget(newNameLabel_);
	if(chooseExistingBox_->isVisible())
		gl_->removeWidget(chooseExistingBox_);
	else
		gl_->removeWidget(emptyChooseExistingBox_);
	gl_->removeWidget(goNewButton_);
	gl_->removeWidget(goExistingButton_);
	gl_->removeWidget(cancelButton_);
	newNameEdit_->hide();
	newNameLabel_->hide();
	if(chooseExistingBox_->isVisible())
		chooseExistingBox_->hide();
	else
		emptyChooseExistingBox_->hide();
	goNewButton_->hide();
	goExistingButton_->hide();
	cancelButton_->hide();
	gl_->addWidget(markNewButton_, 0, 0, 1, 1, Qt::AlignLeft);
	gl_->setColumnStretch(0, 5);
	markNewButton_->show();
	setFrameStyle(QFrame::NoFrame);
}

void AMSamplePositionItemExpandingAdder::switchBoxes(){
	chooseExistingBox_->setMinimumWidth(emptyChooseExistingBox_->width());
	emptyChooseExistingBox_->hidePopup();
	gl_->removeWidget(emptyChooseExistingBox_);
	gl_->addWidget(chooseExistingBox_, 2, 0, 1, 1, Qt::AlignLeft);
	emptyChooseExistingBox_->hide();
	chooseExistingBox_->show();
	chooseExistingBox_->setFocus();
	chooseExistingBox_->showPopup();
}

AMTrickComboBox::AMTrickComboBox(QWidget *parent) :
		QComboBox(parent)
{}

void AMTrickComboBox::mousePressEvent(QMouseEvent *e){
	emit clicked();
	e->ignore();
}
