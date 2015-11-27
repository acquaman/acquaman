#include "AMTESTDataModelListView.h"

#include "util/TimeEvolutionStripTool/AMTESTDataModel.h"
#include "util/TimeEvolutionStripTool/AMTESTServerConnection.h"

#include <QGroupBox>

AMTESTDataModelListView::AMTESTDataModelListView(QWidget *parent)
	: QWidget(parent)
{
	seriesDataModelButtonGroup_ = new QButtonGroup(this);
	seriesDataModelButtonGroup_->setExclusive(false);
	connect(seriesDataModelButtonGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDataModelCheckBoxChecked(QAbstractButton*)));

	imageDataModelButtonGroup_ = new QButtonGroup(this);
	connect(imageDataModelButtonGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDataModelCheckBoxChecked(QAbstractButton*)));

	QGroupBox *listViewBox = new QGroupBox;
	listViewBox->setTitle("Available Sources");

	dataModelLayout_ = new QVBoxLayout;
	listViewBox->setLayout(dataModelLayout_);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(listViewBox);

	setLayout(layout);
}

AMTESTDataModelListView::~AMTESTDataModelListView()
{

}

void AMTESTDataModelListView::addNewDataModels(AMTESTServerConnection *connection)
{
	foreach (AMTESTDataModel *dataModel, connection->dataModels()){

		QCheckBox *dataModelCheckBox = new QCheckBox(dataModel->name());
		dataModelCheckBoxes_.insert(dataModel->name(), dataModelCheckBox);
		dataModelLayout_->addWidget(dataModelCheckBox);

		if (dataModel->isSeriesDataModel())
			seriesDataModelButtonGroup_->addButton(dataModelCheckBox);

		else if (dataModel->isImageDataModel())
			imageDataModelButtonGroup_->addButton(dataModelCheckBox);
	}
}

void AMTESTDataModelListView::removeDataModels(const QStringList &dataModelNames)
{
	foreach (QString dataModelName, dataModelNames){

		QCheckBox *dataModelCheckBox = dataModelCheckBoxes_.value(dataModelName);
		dataModelLayout_->removeWidget(dataModelCheckBox);
		dataModelCheckBox->deleteLater();
	}
}

void AMTESTDataModelListView::onDataModelCheckBoxChecked(QAbstractButton *checkBox)
{
	QCheckBox *dataModelCheckBox = qobject_cast<QCheckBox *>(checkBox);

	if (dataModelCheckBox){

		QString dataModelName = dataModelCheckBoxes_.key(dataModelCheckBox);

		if (dataModelCheckBox->isChecked())
			emit dataModelToBeAdded(dataModelName);

		else
			emit dataModelToBeRemoved(dataModelName);
	}
}

