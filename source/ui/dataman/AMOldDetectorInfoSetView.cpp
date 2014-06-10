#include "AMOldDetectorInfoSetView.h"

#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>

 AMOldDetectorInfoSetView::~AMOldDetectorInfoSetView(){}
AMOldDetectorInfoSetView::AMOldDetectorInfoSetView(const AMOldDetectorInfoSet &infoSet, QWidget *parent) :
	QGroupBox(parent)
{
	internalView_ = new AMOldDetectorInfoSetViewInternal(infoSet, parent);

	setTitle(infoSet.name());
	scrollArea_ = new QScrollArea();
	scrollArea_->setWidget(internalView_);
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(scrollArea_);
	mainVL->setContentsMargins(0,0,0,0);
	setLayout(mainVL);
}

 AMOldDetectorInfoSetViewInternal::~AMOldDetectorInfoSetViewInternal(){}
AMOldDetectorInfoSetViewInternal::AMOldDetectorInfoSetViewInternal(const AMOldDetectorInfoSet &infoSet, QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout *mainVL = new QVBoxLayout();

	QLabel *tempLabel;
	QCheckBox *tempCheckBox;
	QHBoxLayout *tempHL;
	for(int x = 0, size = infoSet.count(); x < size; x++){
		tempLabel = new QLabel(infoSet.detectorInfoAt(x)->description());
		tempCheckBox = new QCheckBox();
		tempCheckBox->setChecked(infoSet.isActiveAt(x));
		tempHL = new QHBoxLayout();
		tempHL->addWidget(tempLabel);
		tempHL->addWidget(tempCheckBox);
		mainVL->addLayout(tempHL);
	}

	setLayout(mainVL);
}
