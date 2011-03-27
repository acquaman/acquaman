#include "AMSamplePlateItemWidget.h"
#include "ui_AMSamplePlateItemWidget.h"

AMSamplePlateItemWidget::AMSamplePlateItemWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AMSamplePlateItemWidget)
{
	ui->setupUi(this);
}

AMSamplePlateItemWidget::~AMSamplePlateItemWidget()
{
	delete ui;
}

void AMSamplePlateItemWidget::setSampleNameText(const QString& string) {
	ui->sampleName->setText(string);
}

void AMSamplePlateItemWidget::setDateTimeText(const QString& createdText) {
	ui->createdLabel->setText(createdText);
}

void AMSamplePlateItemWidget::setElementText(const QString& elementText) {
	ui->elementsLabel->setText(elementText);
}

void AMSamplePlateItemWidget::setPositionText(const AMControlInfoList& controlInfo) {
	QStringList controlNames;
	QStringList controlPositions;

	for(int i=0; i<controlInfo.count(); i++) {
		controlNames << controlInfo.at(i).name();
		controlPositions << QString("%1 %2").arg(controlInfo.at(i).value()).arg(controlInfo.at(i).units());
	}

	ui->controlNames->setText(controlNames.join("\n"));
	ui->controlValues->setText(controlPositions.join("\n"));
}
