#include "BottomBar.h"

BottomBar::BottomBar(QWidget *parent) : QWidget(parent)
{
	setupUi(this);

	statusView_ = new AMStatusView();
	mainHLayout->addWidget(statusView_);

	
	makeConnections();
}

BottomBar::~BottomBar()
{

}
