#include "AMSamplePlateView.h"

AMSamplePlateView::AMSamplePlateView(QString title, QWidget *parent) :
	QGroupBox(title, parent)
{
	//samplePlate_ = new AMSamplePlate(this);
	manipulator_ = NULL;

	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	plateNameLabel_ = new QLabel(samplePlate_.plateName());
	plateNameLabel_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	existingPlates_ = new QComboBox();
	existingPlates_->addItem("Load Existing");
	sampleListView_ = new AMSampleListView(&samplePlate_);
	sampleListView_->setManipulator(manipulator_);
	//sampleListView_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	vl_ = new QVBoxLayout();
	vl_->setAlignment(Qt::AlignTop);
	vl_->addWidget(plateNameLabel_);
	vl_->addWidget(existingPlates_);
	vl_->addWidget(sampleListView_);
	setLayout(vl_);
}

void AMSamplePlateView::setManipulator(AMControlSet *manipulator){
	manipulator_ = manipulator;
	sampleListView_->setManipulator(manipulator_);
}

AMSampleListView::AMSampleListView(AMSamplePlate *samplePlate, QWidget *parent) :
		QFrame(parent)
{
	nextNew = 1;
	samplePlate_ = samplePlate;
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

	adder_ = new QPushButton(QIcon(":/add.png"), "Mark a New Sample Position ...");
	connect(adder_, SIGNAL(clicked()), this, SLOT(addNewSampleToPlate()));
	il_->addWidget(adder_);
}

void AMSampleListView::addNewSampleToPlate(){
	QString tmpStr;
	tmpStr.setNum(nextNew);
	nextNew++;
	/// \todo who owns this sample?
	AMSample *tmpSample = new AMSample("SGM Sample "+tmpStr);
	AMControlSetInfo *tmpPosition = NULL;
	if(manipulator_)
		tmpPosition = new AMControlSetInfo(manipulator_->info());
	samplePlate_->appendSamplePosition(tmpSample, tmpPosition);
}

void AMSampleListView::setManipulator(AMControlSet *manipulator){
	manipulator_ = manipulator;
}

void AMSampleListView::onSamplePositionChanged(int index){
	qDebug() << "Claims change at " << index;
}

void AMSampleListView::onSamplePositionAdded(int index){
	qDebug() << "Claims added at " << index;
	if(!manipulator_)
		return;
	AMSamplePositionItemView *tmpSPIView = new AMSamplePositionItemView(samplePlate_->samplePositionAt(index), manipulator_, index+1);
	il_->insertWidget(index, tmpSPIView, 0, Qt::AlignTop);
	sa_->ensureVisible(0, 0);
}

void AMSampleListView::onSamplePositionRemoved(int index){
	qDebug() << "Claims removed at " << index;
}

QSize AMSampleListView::sizeHint() const{
	QSize tmpSize = QFrame::sizeHint();
	int newWidth;
	if(sa_->verticalScrollBar() && sa_->verticalScrollBar()->isVisible())
		newWidth = 1.1*(tmpSize.width()+sa_->verticalScrollBar()->width());
	else
		newWidth = tmpSize.width()*1.1;
	tmpSize.setWidth(newWidth);
	qDebug() << "Width will be " << tmpSize.width();
	return tmpSize;
}

AMSamplePositionItemView::AMSamplePositionItemView(AMSamplePosition *samplePosition, AMControlSet *manipulator, int index, QWidget *parent) :
		QFrame(parent)
{
	index_ = index;
	inFocus_ = false;
	samplePosition_ = samplePosition;
	manipulator_ = manipulator;

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
	return true;
}

bool AMSamplePositionItemView::onRecallPositionClicked(){
	if(!manipulator_)
		return false;
	manipulator_->setFromInfo(samplePosition_->position());
	return true;
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
		sampleBox_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
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
	sampleBox_->addItem(samplePosition_->sample()->name());
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
