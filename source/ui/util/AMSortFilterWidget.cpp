#include "AMSortFilterWidget.h"


AMSortFilterBuilder::AMSortFilterBuilder(QWidget *parent) :
	QWidget(parent)
{
	fieldComboBox_ = new QComboBox();
	modeComboBox_ = new QComboBox();
	modeComboBox_->addItem("Contains");
	modeComboBox_->addItem("Regular Expression");
	modeComboBox_->addItem("Exact Match");

	criteriaLineEdit_ = new QLineEdit();
	caseSensitiveCheckBox_ = new QCheckBox();

	layout_ = new QFormLayout();

	layout_->addRow("Search Criteria:", criteriaLineEdit_);
	layout_->addRow("Field:", fieldComboBox_);
	layout_->addRow("Filter Mode:", modeComboBox_);
	layout_->addRow("Case Sensitive:", caseSensitiveCheckBox_);

	fieldComboBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	modeComboBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setLayout(layout_);
}

int AMSortFilterBuilder::filterKeyColumn() const
{
	return fieldComboBox_->currentIndex();
}

QString AMSortFilterBuilder::criteriaText() const
{
	switch (modeComboBox_->currentIndex()){
		case 0:
			return QString(".*%1.*").arg(criteriaLineEdit_->text());
		case 2:
			return QString("^%1$").arg(criteriaLineEdit_->text());
		default:
				return criteriaLineEdit_->text();
	}
}

bool AMSortFilterBuilder::isCaseSensitive() const
{
	return caseSensitiveCheckBox_->isChecked();
}

void AMSortFilterBuilder::addField(const QString &fieldName)
{
	fieldComboBox_->addItem(fieldName);
}

void AMSortFilterBuilder::clearFields()
{
	fieldComboBox_->clear();
}

QFormLayout* AMSortFilterBuilder::formLayout()
{
	return layout_;
}

void AMSortFilterBuilder::clear()
{
	criteriaLineEdit_->setText("");
	if(fieldComboBox_->count() > 0)
		fieldComboBox_->setCurrentIndex(0);
}

AMSortFilterWidget::AMSortFilterWidget(QSortFilterProxyModel *model, QWidget *parent) :
	QWidget(parent)
{
	proxyModel_ = model;


	isCurrentlyFiltered_ = false;
	QIcon toggleHideIcon;
	toggleHideIcon.addFile(QString::fromUtf8("://system-search-2.png"), QSize(), QIcon::Normal);
	toggleHideButton_ = new QPushButton();
	toggleHideButton_->setIcon(toggleHideIcon);
	toggleHideButton_->setCheckable(true);
	filteredWarningLabel_ = new QLabel("<b>Data is Filtered, click here to clear</b>");
	filteredWarningLabel_->setVisible(false);

	QIcon clearFilterPushButtonIcon;
	clearFilterPushButtonIcon.addFile(QString::fromUtf8("://Close.png"), QSize(), QIcon::Normal);
	clearFilterPushButton_ = new QPushButton();
	clearFilterPushButton_->setIcon(clearFilterPushButtonIcon);
	clearFilterPushButton_->setVisible(false);

	applyFilterPushButton_ = new QPushButton("Search");
	applyFilterPushButton_->setVisible(false);
	filterBuilder_ = new AMSortFilterBuilder();
	filterBuilder_->setVisible(false);

	refreshColumns();
	initLayout();
	setCurrentlyFiltered(false);

	connect(applyFilterPushButton_, SIGNAL(clicked()), this, SLOT(onFilterApplied()));
	connect(clearFilterPushButton_, SIGNAL(clicked()), this, SLOT(onClearFilterButtonClicked()));
	connect(toggleHideButton_, SIGNAL(clicked(bool)), this, SLOT(setFilterAreaVisible(bool)));
}

bool AMSortFilterWidget::isCurrentlyFiltered()
{
	return isCurrentlyFiltered_;
}

void AMSortFilterWidget::addManualColumn(const QString &header)
{
	filterBuilder_->addField(header);
}

QSortFilterProxyModel *AMSortFilterWidget::proxyModel()
{
	return proxyModel_;
}

void AMSortFilterWidget::refreshColumns()
{
	if(!proxyModel())
		return;

	filterBuilder_->clearFields();

	for (int iField = 0; iField < proxyModel()->columnCount(); iField++)
	{
		filterBuilder_->addField(proxyModel()->headerData(iField, Qt::Horizontal).toString());
	}
}

void AMSortFilterWidget::initLayout()
{
	QVBoxLayout* mainLayout = new QVBoxLayout();

	QHBoxLayout* statusLayout = new QHBoxLayout();
	statusLayout->addWidget(toggleHideButton_);
	statusLayout->addWidget(clearFilterPushButton_);
	statusLayout->addWidget(filteredWarningLabel_);
	statusLayout->addStretch();
	mainLayout->addLayout(statusLayout);

	mainLayout->addWidget(filterBuilder_);

	QHBoxLayout* controlLayout = new QHBoxLayout();
	controlLayout->addWidget(applyFilterPushButton_);
	controlLayout->addStretch();
	mainLayout->addLayout(controlLayout);

	//QHBoxLayout* statusWarningLayout = new QHBoxLayout();
	//statusWarningLayout->addWidget(clearFilterPushButton_);
	//statusWarningLayout->addWidget(filteredWarningLabel_);
	//statusWarningLayout->addStretch();
	//mainLayout->addLayout(statusWarningLayout);


	setLayout(mainLayout);
}

void AMSortFilterWidget::setCurrentlyFiltered(bool value)
{
	clearFilterPushButton_->setVisible(value);
	filteredWarningLabel_->setVisible(value);
	if(value != isCurrentlyFiltered_)
	{
		isCurrentlyFiltered_ = value;
	}
}

void AMSortFilterWidget::setFilterAreaVisible(bool visible)
{
	filterBuilder_->setVisible(visible);
	applyFilterPushButton_->setVisible(visible);
}

QSortFilterProxyModel *AMSortFilterWidget::model()
{
	return proxyModel_;
}

void AMSortFilterWidget::onFilterApplied()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	QSortFilterProxyModel* viewProxyModel = proxyModel();

	if(!viewProxyModel)
		return;

	if(filterBuilder_->isCaseSensitive())
		viewProxyModel->setFilterCaseSensitivity(Qt::CaseSensitive);
	else
		viewProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

	viewProxyModel->setFilterKeyColumn(filterBuilder_->filterKeyColumn());

	proxyModel_->setFilterRegExp(filterBuilder_->criteriaText());

	setCurrentlyFiltered(true);
	emit filterChanged(isCurrentlyFiltered_);
	QApplication::restoreOverrideCursor();
}

void AMSortFilterWidget::onClearFilterButtonClicked()
{
	if(!proxyModel())
		return;	

	filterBuilder_->clear();
	proxyModel()->setFilterFixedString(QString());
	setCurrentlyFiltered(false);
	emit filterChanged(isCurrentlyFiltered_);
}




