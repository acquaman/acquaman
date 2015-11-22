#include "AMTESTDataModelListView.h"

#include "util/TimeEvolutionStripTool/AMTESTDataModel.h"
#include "util/TimeEvolutionStripTool/AMTESTServerConnection.h"

#include <QGroupBox>

AMTESTDataModelListView::AMTESTDataModelListView(QWidget *parent)
	: QWidget(parent)
{
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

