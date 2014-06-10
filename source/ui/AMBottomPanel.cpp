#include "AMBottomPanel.h"

 AMBottomPanel::~AMBottomPanel(){}
AMBottomPanel::AMBottomPanel(QWidget *parent)
	: QFrame(parent)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setStyleSheet("QFrame {\nbackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(89, 89, 89, 255), stop:0.494444 rgba(89, 89, 89, 255), stop:0.5 rgba(58, 58, 58, 255), stop:1 rgba(58, 58, 58, 255));\nborder-bottom: 1px solid black;\n}");
	setMinimumHeight(64);

	layout_ = new QHBoxLayout;

	setLayout(layout_);
}
