#include "AMTESTDataModelListView.h"

#include "util/TimeEvolutionStripTool/AMTESTDataModel.h"
#include "util/TimeEvolutionStripTool/AMTESTServerConnection.h"

#include <QGroupBox>
#include <QScrollArea>

AMTESTDataModelListView::AMTESTDataModelListView(QWidget *parent)
	: QWidget(parent)
{
	setMinimumSize(300, 200);
	seriesDataModelButtonGroup_ = new QButtonGroup(this);
	seriesDataModelButtonGroup_->setExclusive(false);
	connect(seriesDataModelButtonGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDataModelCheckBoxChecked(QAbstractButton*)));

	imageDataModelButtonGroup_ = new QButtonGroup(this);
	connect(imageDataModelButtonGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDataModelCheckBoxChecked(QAbstractButton*)));

	dataModelLayout_ = new QVBoxLayout;

	QGroupBox *bufferGroupViewBox = new QGroupBox;
	bufferGroupViewBox->setLayout(dataModelLayout_);
	bufferGroupViewBox->setStyleSheet("border:0;");

	QScrollArea *itemScrollArea = new QScrollArea;
	itemScrollArea->setWidgetResizable(true);
	itemScrollArea->setWidget(bufferGroupViewBox);
	itemScrollArea->setFrameShape(QFrame::NoFrame);
	itemScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QVBoxLayout *vlayout = new QVBoxLayout();
	vlayout->addWidget(itemScrollArea);

	QGroupBox *listViewBox = new QGroupBox;
	listViewBox->setTitle("Available Sources");
	listViewBox->setLayout(vlayout);

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

