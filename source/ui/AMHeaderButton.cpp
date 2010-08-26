#include "AMHeaderButton.h"

#define AMHEADERBUTTON_STYLE_SHEET "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(233, 233, 233, 255), stop:0.502513 rgba(199, 198, 198, 255), stop:1 rgba(163, 162, 162, 255)); border: 1px solid black; border-top-color: rgb(180, 180, 180); border-bottom-color: rgb(121, 121, 121); border-left-color: rgb(180, 180, 180); border-right-color: rgb(180, 180, 180);"

AMHeaderButton::AMHeaderButton(QWidget *parent) :
	QToolButton(parent)
{
	setStyleSheet(AMHEADERBUTTON_STYLE_SHEET);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setMaximumHeight(16);
}
