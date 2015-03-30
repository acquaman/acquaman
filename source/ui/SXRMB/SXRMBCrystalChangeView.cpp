#include "SXRMBCrystalChangeView.h"

#include "ui/beamline/AMExtendedControlEditor.h"

#include <QVBoxLayout>

SXRMBCrystalChangeView::SXRMBCrystalChangeView(SXRMBCrystalChangeModel *crystalChange, QWidget *parent)
	: QWidget(parent)
{
	crystalChange_ = crystalChange;

	QVBoxLayout *editorLayout = new QVBoxLayout;

	AMExtendedControlEditor *editor = new AMExtendedControlEditor(crystalChange_->crystalSelectionControl());
	editor->setControlFormat('f', 2);
	editorLayout->addWidget(editor);
	editor = new AMExtendedControlEditor(crystalChange_->crystalYControl());
	editor->setControlFormat('f', 4);
	editorLayout->addWidget(editor);
	editor = new AMExtendedControlEditor(crystalChange_->crystalThetaControl());
	editor->setControlFormat('f', 4);
	editorLayout->addWidget(editor);
	editor = new AMExtendedControlEditor(crystalChange_->crystalChiControl());
	editor->setControlFormat('f', 4);
	editorLayout->addWidget(editor);

	setLayout(editorLayout);
}

SXRMBCrystalChangeView::~SXRMBCrystalChangeView()
{

}
