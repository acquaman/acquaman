#include "BottomBar.h"

BottomBar::BottomBar(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	
	makeConnections();
}

BottomBar::~BottomBar()
{

}
