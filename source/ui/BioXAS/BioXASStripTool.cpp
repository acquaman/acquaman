#include "BioXASStripTool.h"

BioXASStripTool::BioXASStripTool(QWidget *parent) :
	AMStripTool(parent)
{
	// Create and add control preferences.

	ringCurrent_ = new AMReadOnlyPVControl("SR1Current", "PCT1402-01:mA:fbk", this);
	addControl(ringCurrent_);

	// Sizing preferences.

	QSizePolicy sizePreferences = QSizePolicy();
	sizePreferences.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	sizePreferences.setVerticalPolicy(QSizePolicy::MinimumExpanding);
	sizePreferences.setHeightForWidth(true);

	setSizePolicy(sizePreferences);

	resize(width(), width());
}

BioXASStripTool::~BioXASStripTool()
{

}

QSize BioXASStripTool::sizeHint() const
{
	return QSize(100, 100);
}

int BioXASStripTool::heightForWidth(int width) const
{
	return width;
}
