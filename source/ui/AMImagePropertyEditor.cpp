#include "AMImagePropertyEditor.h"
#include "ui_AMImagePropertyEditor.h"

AMImagePropertyEditor::AMImagePropertyEditor(QWidget *parent) :
	QWidget(parent),
	ui_(new Ui::AMImagePropertyEditor)
{
	ui_->setupUi(this);
}

AMImagePropertyEditor::~AMImagePropertyEditor()
{
	delete ui_;
}
