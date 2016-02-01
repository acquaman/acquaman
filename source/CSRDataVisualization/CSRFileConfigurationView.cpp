#include "CSRFileConfigurationView.h"

#include <math.h>
#include <QHBoxLayout>
#include <QPushButton>

CSRFileConfigurationView::CSRFileConfigurationView(CSRFileConfiguration *info, QWidget *parent)
	: QWidget(parent)
{
	fileConfiguration_ = info;

	fileSize_ = new QSpinBox;
	fileSize_->setRange(0, 100000);
	fileSize_->setPrefix("Size: ");
	fileSize_->setSuffix(" MB");
	fileSize_->setValue(fileConfiguration_->fileSize()/pow(2, 20));
	connect(fileSize_, SIGNAL(editingFinished()), this, SLOT(onFileSizeChanged()));

	numberOfPoints_ = new QSpinBox;
	numberOfPoints_->setRange(0, 1e9);
	numberOfPoints_->setPrefix("# points: ");
	numberOfPoints_->setValue(fileConfiguration_->numberOfPoints());
	connect(numberOfPoints_, SIGNAL(editingFinished()), this, SLOT(onNumberOfPointsChanged()));

	numberOfRevolutions_ = new QSpinBox;
	numberOfRevolutions_->setRange(0, 1e6);
	numberOfRevolutions_->setPrefix("Revolutions: ");
	numberOfRevolutions_->setValue(fileConfiguration_->numberOfRevolutions());
	connect(numberOfRevolutions_, SIGNAL(editingFinished()), this, SLOT(onNumberOfRevolutionsChanged()));

	QPushButton *deleteButton = new QPushButton("Remove");
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteRequested()));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(fileSize_, 0, Qt::AlignCenter);
	layout->addWidget(numberOfPoints_, 0, Qt::AlignCenter);
	layout->addWidget(numberOfRevolutions_, 0, Qt::AlignCenter);
	layout->addWidget(deleteButton, 0, Qt::AlignCenter);

	setLayout(layout);
}

void CSRFileConfigurationView::onFileSizeChanged()
{
	fileConfiguration_->setFileSize(fileSize_->value()*pow(2, 20));
}

void CSRFileConfigurationView::onNumberOfPointsChanged()
{
	fileConfiguration_->setNumberOfPoints(numberOfPoints_->value());
}

void CSRFileConfigurationView::onNumberOfRevolutionsChanged()
{
	fileConfiguration_->setNumberOfRevolutions(numberOfRevolutions_->value());
}

void CSRFileConfigurationView::onDeleteRequested()
{
	emit deleteRequested(this);
}
