#include "BioXASStripTool.h"

BioXASStripToolWidget::BioXASStripToolWidget(QWidget *parent) :
	AMStripToolWidget(0, parent)
{
	// Create and set striptool.

	AMStripTool *stripTool = new AMStripTool(this);

	ringCurrent_ = new AMReadOnlyPVControl("SR1Current", "PCT1402-01:mA:fbk", this);
	stripTool->addItem(ringCurrent_);

	setStripTool(stripTool);

	// Sizing preferences.

	QSizePolicy sizePreferences = QSizePolicy();
	sizePreferences.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	sizePreferences.setVerticalPolicy(QSizePolicy::MinimumExpanding);
	sizePreferences.setHeightForWidth(true);

	setSizePolicy(sizePreferences);

	resize(width(), width());
}

BioXASStripToolWidget::~BioXASStripToolWidget()
{

}

QSize BioXASStripToolWidget::sizeHint() const
{
	return QSize(100, 100);
}

int BioXASStripToolWidget::heightForWidth(int width) const
{
	return width;
}
