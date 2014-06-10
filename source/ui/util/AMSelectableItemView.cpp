#include "AMSelectableItemView.h"

 AMSelectableItemView::~AMSelectableItemView(){}
AMSelectableItemView::AMSelectableItemView(QWidget *parent)
	: QWidget(parent)
{
	checkBox_ = new QCheckBox;

	connect(checkBox_, SIGNAL(toggled(bool)), this, SIGNAL(itemSelected(bool)));

	itemLayout_ = new QHBoxLayout;
	itemLayout_->addWidget(checkBox_);
	itemLayout_->addStretch();

	setLayout(itemLayout_);
	setMaximumHeight(35);
}

AMSelectableItemView::AMSelectableItemView(const QString &text, bool checked, QWidget *parent)
	: QWidget(parent)
{
	checkBox_ = new QCheckBox(text);
	checkBox_->setChecked(checked);

	connect(checkBox_, SIGNAL(toggled(bool)), this, SIGNAL(itemSelected(bool)));

	itemLayout_ = new QHBoxLayout;
	itemLayout_->addWidget(checkBox_);
	itemLayout_->addStretch();

	setLayout(itemLayout_);
	setMaximumHeight(35);
}

void AMSelectableItemView::setText(const QString &text)
{
	checkBox_->setText(text);
}

void AMSelectableItemView::setSelected(bool isSelected)
{
	checkBox_->setChecked(isSelected);
}
