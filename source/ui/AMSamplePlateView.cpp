#include "AMSamplePlateView.h"

AMSamplePlateView::AMSamplePlateView(QString title, QWidget *parent) :
	QGroupBox(title, parent)
{
	samplePlate_ = new AMSamplePlate(this);

	plateNameLabel_ = new QLabel(samplePlate_->plateName());
	plateNameLabel_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	existingPlates_ = new QComboBox();
	existingPlates_->addItem("Load Existing");
	sampleListView_ = new AMSampleListView();
	sampleListView_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	vl_ = new QVBoxLayout();
	vl_->setAlignment(Qt::AlignTop);
	vl_->addWidget(plateNameLabel_);
	vl_->addWidget(existingPlates_);
	vl_->addWidget(sampleListView_);
	setLayout(vl_);
}

AMSampleListView::AMSampleListView(QWidget *parent) :
		QFrame(parent)
{

	saf_ = new QFrame();
	il_ = new QVBoxLayout();
	il_->setAlignment(Qt::AlignTop);
	saf_->setLayout(il_);
	saf_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	sa_ = new QScrollArea();
	sa_->setWidget(saf_);
	sa_->setWidgetResizable(true);
	QVBoxLayout *ol = new QVBoxLayout();
	ol->addWidget(sa_);
	setLayout(ol);

	AMSample *tmpSample = new AMSample("Mullens", this);
	//AMControlSetInfo *tmpPosition = SGMBeamline::sgm()->ssaManipulatorSet()->info();
	AMControlSetInfo *tmpPosition = new AMControlSetInfo(SGMBeamline::sgm()->ssaManipulatorSet()->info(), this);
	tmpPosition->setValueAt(0, 15);
	tmpPosition->setValueAt(1, -5);
	tmpPosition->setValueAt(2, 4000);
	tmpPosition->setValueAt(3, 60);
	AMSamplePosition *tmpSP = new AMSamplePosition(tmpSample, tmpPosition, this);
	AMSamplePositionItemView *tmpSPIView = new AMSamplePositionItemView(tmpSP, SGMBeamline::sgm()->ssaManipulatorSet(), 1);
	il_->addWidget(tmpSPIView);
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

	onSamplePositionUpdate();
	if(samplePosition_)
		connect(samplePosition_, SIGNAL(positionValuesChanged()), this, SLOT(onSamplePositionUpdate()));
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
	for(int x = 0; x < samplePosition_->position()->count(); x++)
		qDebug() << "Copy value at " << x << manipulator_->info()->valueAt(x);
	samplePosition_->position()->copyFrom(manipulator_->info());
	for(int x = 0; x < samplePosition_->position()->count(); x++)
		qDebug() << "Claims value at " << x << samplePosition_->position()->valueAt(x);
	return true;
}

bool AMSamplePositionItemView::onRecallPositionClicked(){
	if(!manipulator_)
		return false;
	for(int x = 0; x < samplePosition_->position()->count(); x++)
		qDebug() << "Caller claims value at " << x << samplePosition_->position()->valueAt(x);
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

void AMSamplePositionItemView::onSamplePositionUpdate(){
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
		hl_->addWidget(indexLabel_);
	}
	if(!sampleBox_){
		sampleBox_ = new QComboBox();
		hl_->addWidget(sampleBox_);
	}
	if(!positionLabel_){
		positionLabel_ = new QLabel(this);
		positionLabel_->hide();
	}
	if(!savePositionButton_){
		savePositionButton_ = new QPushButton(QIcon(":/save.png"), "");
		savePositionButton_->setToolTip("Save Current Position");
		connect(savePositionButton_, SIGNAL(clicked()), this, SLOT(onSavePositionClicked()));
		hl_->addWidget(savePositionButton_);
	}
	if(!recallPositionButton_){
		recallPositionButton_ = new QPushButton(QIcon(":/go-next.png"), "");
		recallPositionButton_->setToolTip("Move to Saved Position");
		connect(recallPositionButton_, SIGNAL(clicked()), this, SLOT(onRecallPositionClicked()));
		hl_->addWidget(recallPositionButton_);
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
	qDebug() << "Text is " << positionText;
	for(int x = 0; x < samplePosition_->position()->count(); x++){
		qDebug() << "At " << x << samplePosition_->position()->valueAt(x);
	}
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
