#include "AMSortFilterWidget.h"
#include <QGridLayout>
#include <QFormLayout>

AMSortFilterWidget::AMSortFilterWidget(QSortFilterProxyModel *model, QWidget *parent) :
	QWidget(parent)
{
	proxyModel_ = model;


	isCurrentlyFiltered_ = false;
	fieldComboBox_ = new QComboBox();
	modeComboBox_ = new QComboBox();
	caseSensitiveCheckBox_ = new QCheckBox();
	criteriaLineEdit_ = new QLineEdit();
	applyFilterPushButton_ = new QPushButton("Search");
	clearFilterPushButton_ = new QPushButton("Clear");
	fieldComboBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	modeComboBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	refreshColumns();
	initModes();
	initCaseSensitivity();
	initLayout();
	setCurrentlyFiltered(false);


	connect(criteriaLineEdit_, SIGNAL(returnPressed()), this, SLOT(onFilterApplied()));
	connect(applyFilterPushButton_, SIGNAL(clicked()), this, SLOT(onFilterApplied()));
	connect(clearFilterPushButton_, SIGNAL(clicked()), this, SLOT(onClearFilterButtonClicked()));
}

bool AMSortFilterWidget::isCurrentlyFiltered()
{
	return isCurrentlyFiltered_;
}

QSortFilterProxyModel *AMSortFilterWidget::proxyModel()
{
	return proxyModel_;
}

void AMSortFilterWidget::refreshColumns()
{
	if(!proxyModel() || !fieldComboBox_)
		return;

	fieldComboBox_->clear();

	for (int iField = 0; iField < proxyModel()->columnCount(); iField++)
	{
		fieldComboBox_->addItem(proxyModel()->headerData(iField, Qt::Horizontal).toString());
	}
}

void AMSortFilterWidget::initLayout()
{
	QVBoxLayout* mainLayout = new QVBoxLayout();

	QFormLayout* filterSetup = new QFormLayout();
	filterSetup->addRow("Search Criteria:", criteriaLineEdit_);
	filterSetup->addRow("Field:", fieldComboBox_);
	filterSetup->addRow("Filter Mode:", modeComboBox_);
	filterSetup->addRow("Case Sensitive:", caseSensitiveCheckBox_);

	QHBoxLayout* buttonLayout = new QHBoxLayout();

	buttonLayout->addWidget(applyFilterPushButton_, 0, Qt::AlignLeft);
	buttonLayout->addWidget(clearFilterPushButton_, 0, Qt::AlignLeft);
	buttonLayout->insertStretch(2, 100);

	mainLayout->addLayout(filterSetup);
	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);
}

void AMSortFilterWidget::initModes()
{
	modeComboBox_->addItem("Contains");
	modeComboBox_->addItem("Regular Expression");
	modeComboBox_->addItem("Exact Match");
}

void AMSortFilterWidget::initCaseSensitivity()
{
	if(!proxyModel() || !caseSensitiveCheckBox_)
		return;

	caseSensitiveCheckBox_->setChecked(proxyModel()->filterCaseSensitivity() == Qt::CaseSensitive);
}

void AMSortFilterWidget::setCurrentlyFiltered(bool value)
{
	clearFilterPushButton_->setEnabled(value);
	if(value != isCurrentlyFiltered_)
	{
		isCurrentlyFiltered_ = value;
		emit isCurrentlyFilteredStateChanged(isCurrentlyFiltered_);
	}
}

void AMSortFilterWidget::onFilterApplied()
{

	QSortFilterProxyModel* viewProxyModel = proxyModel();

	if(!viewProxyModel)
		return;

	setCurrentlyFiltered(true);

	if(caseSensitiveCheckBox_->isChecked())
		viewProxyModel->setFilterCaseSensitivity(Qt::CaseSensitive);
	else
		viewProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

	viewProxyModel->setFilterKeyColumn(fieldComboBox_->currentIndex());

	switch (modeComboBox_->currentIndex()){
		case 0:
		viewProxyModel->setFilterWildcard(criteriaLineEdit_->text());
		break;
		case 1:
		viewProxyModel->setFilterRegExp(criteriaLineEdit_->text());
		break;
		case 2:
		viewProxyModel->setFilterRegExp(QString("^%1$").arg(criteriaLineEdit_->text()));
		break;

	}


}

void AMSortFilterWidget::onClearFilterButtonClicked()
{
	if(!proxyModel())
		return;

	setCurrentlyFiltered(false);

	criteriaLineEdit_->setText(QString());
	proxyModel()->setFilterFixedString(QString());
}



