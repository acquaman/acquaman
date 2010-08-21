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
	AMControlSetInfo *tmpPosition = SGMBeamline::sgm()->ssaManipulatorSet()->info();
	AMSamplePosition *tmpSP = new AMSamplePosition(tmpSample, tmpPosition, this);
	AMSamplePositionItemView *tmpSPIView = new AMSamplePositionItemView(tmpSP, 1);
	il_->addWidget(tmpSPIView);
}

AMSamplePositionItemView::AMSamplePositionItemView(AMSamplePosition *samplePosition, int index, QWidget *parent) :
		QFrame(parent)
{
	index_ = index;
	samplePosition_ = samplePosition;

	hl_ = NULL;
	indexLabel_ = NULL;
	sampleBox_ = NULL;
	positionLabel_ = NULL;
	savePositionButton_ = NULL;
	recallPositionButton_ = NULL;

	setLineWidth(3);
	setFrameStyle(QFrame::StyledPanel);

	onSamplePositionUpdate();
}

int AMSamplePositionItemView::index(){
	return index_;
}

void AMSamplePositionItemView::setIndex(int index){
	index_ = index;
}

void AMSamplePositionItemView::onSamplePositionUpdate(){
	if(!samplePosition_)
		return;
	if(!hl_){
		hl_ = new QHBoxLayout();
		setLayout(hl_);
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
		positionLabel_ = new QLabel();
		hl_->addWidget(positionLabel_);
	}
	if(!savePositionButton_){
		savePositionButton_ = new QPushButton("Save");
		hl_->addWidget(savePositionButton_);
	}
	if(!recallPositionButton_){
		recallPositionButton_ = new QPushButton("Move To");
		hl_->addWidget(recallPositionButton_);
	}
	QString tmpStr;
	tmpStr.setNum(index_);
	indexLabel_->setText(tmpStr+". ");
	sampleBox_->addItem(samplePosition_->sample()->name());
/*	QString positionText;
	for(int x = 0; x < samplePosition_->position()->count(); x++){
		tmpStr.setNum(samplePosition_->position()->valueAt(x));
		tmpStr.prepend(": ");
		tmpStr.prepend(samplePosition_->position()->nameAt(x));
		tmpStr.append("\n");
		positionText.append(tmpStr);
	}
	positionLabel_->setText(positionText);
*/
}
