#include "AMSimpleDataSourceEditor.h"

#include <QFormLayout>
#include <QLabel>


AMSimpleDataSourceEditor::AMSimpleDataSourceEditor(AMDataSource *dataSource, QWidget *parent) : QWidget(parent) {
	dataSource_ = dataSource;

	QFormLayout* fl = new QFormLayout();
	if(dataSource_->rank() == 0) {
		valueLabel_ = new QLabel("value");
		valueLabel_->setStyleSheet("font: bold \"Lucida Grande\";\ncolor: rgb(121,121,121);");
		value_ = new QLabel(QString::number(double(dataSource_->value(AMnDIndex()))));

		fl->addRow(valueLabel_, value_);

		connect(dataSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onValuesChanged()));
	}
	else {
		rankLabel_ = new QLabel("Dimensions");
		rankLabel_->setStyleSheet("font: bold \"Lucida Grande\";\ncolor: rgb(121,121,121);");
		rank_ = new QLabel(QString::number(dataSource_->rank()));

		sizeLabel_ = new QLabel("Size");
		sizeLabel_->setStyleSheet("font: bold \"Lucida Grande\";\ncolor: rgb(121,121,121);");

		QStringList sizeString;
		for(int mu=0; mu<dataSource_->rank(); ++mu)
			sizeString << QString::number(dataSource_->size(mu));
		size_ = new QLabel(sizeString.join(" x "));

		fl->addRow(rankLabel_, rank_);
		fl->addRow(sizeLabel_, size_);

		connect(dataSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onSizeChanged()));
	}

	setLayout(fl);
}

void AMSimpleDataSourceEditor::onValuesChanged()
{
	value_->setText(QString::number(double(dataSource_->value(AMnDIndex()))));
}

void AMSimpleDataSourceEditor::onSizeChanged()
{
	QStringList sizeString;
	for(int mu=0; mu<dataSource_->rank(); ++mu)
		sizeString << QString::number(dataSource_->size(mu));
	size_->setText(sizeString.join(" x "));
}
