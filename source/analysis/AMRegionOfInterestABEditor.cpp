#include "AMRegionOfInterestABEditor.h"

AMRegionOfInterestABEditor::AMRegionOfInterestABEditor(AMRegionOfInterestAB *analysisBlock, QWidget *parent) :
	QWidget(parent)
{
	analysisBlock_ = analysisBlock;

	dataSourceBox_ = new QComboBox();
	updateDataSourcesBox();

	connect( dataSourceBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onDataSourceBoxIndexChanged(int)) );


}

AMRegionOfInterestABEditor::~AMRegionOfInterestABEditor()
{

}

void AMRegionOfInterestABEditor::updateDataSourcesBox()
{
	// Clear the data source box.

	dataSourceBox_->clear();

	// Add item to the box for each input data source.

	for (int i = 0, count = analysisBlock_->inputDataSourceCount(); i < count; i++) {
		AMDataSource *inputSource = analysisBlock_->inputDataSourceAt(i);

		if (analysisBlock_->name() != inputSource->name() && !inputSource->hiddenFromUsers())
			dataSourceBox_->addItem(inputSource->description(), inputSource->name());
	}
}

void AMRegionOfInterestABEditor::onInputSourceBoxIndexChanged()
{
	analysisBlock_->set
}

