#include "AMNumberChangeActionEditor.h"

#include <QLabel>
#include <QSpinBox>
#include <QBoxLayout>

#include "actions3/actions/AMNumberChangeAction.h"

AMNumberChangeActionEditor::AMNumberChangeActionEditor(AMNumberChangeActionInfo *info, QWidget *parent) :
	QFrame(parent)
{
	info_ = info;

	QHBoxLayout* hl = new QHBoxLayout(this);

	descriptionLabel_ = new QLabel(QString("Changing index %1 to %2").arg(info_->indexToChange()).arg(info_->newNumber()));
	hl->addWidget(descriptionLabel_);
	hl->addWidget(new QLabel("Index: "));
	indexSpinBox_ = new QSpinBox();
	indexSpinBox_->setRange(0, AMNumberChangeActionSupport::AMNumberChangeActionList()->count()-1);
	hl->addWidget(indexSpinBox_);
	hl->addWidget(new QLabel("New Number: "));
	newNumberSpinBox_ = new QSpinBox();
	newNumberSpinBox_->setRange(-9999, 9999);
	hl->addWidget(newNumberSpinBox_);
	hl->addStretch(0);

	connect(indexSpinBox_, SIGNAL(editingFinished()), this, SLOT(onIndexSpinBoxEditingFinished()));
	connect(newNumberSpinBox_, SIGNAL(editingFinished()), this, SLOT(onNewNumberSpinBoxEditingFinished()));
}

void AMNumberChangeActionEditor::onIndexSpinBoxEditingFinished(){
	info_->setIndexToChange(indexSpinBox_->value());
	descriptionLabel_->setText(QString("Changing index %1 to %2").arg(info_->indexToChange()).arg(info_->newNumber()));
}

void AMNumberChangeActionEditor::onNewNumberSpinBoxEditingFinished(){
	info_->setNewNumber(newNumberSpinBox_->value());
	descriptionLabel_->setText(QString("Changing index %1 to %2").arg(info_->indexToChange()).arg(info_->newNumber()));
}
