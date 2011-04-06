#include "VESPERSBeamlineView.h"
#include "ui/AMControlEditor.h"
#include <QHBoxLayout>

VESPERSBeamlineView::VESPERSBeamlineView(QWidget *parent) :
	QWidget(parent)
{
	AMControlEditor *yourFace = new AMControlEditor(VESPERSBeamline::vespers()->ccdDetector());
	QHBoxLayout *yourFaceLayout = new QHBoxLayout;
	yourFaceLayout->addWidget(yourFace);
	setLayout(yourFaceLayout);
}
