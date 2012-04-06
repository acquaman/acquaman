#include "AMListActionEditor3.h"

#include <QBoxLayout>
#include <QLabel>

AMListActionEditor3::AMListActionEditor3(AMListActionInfo3 *info, QWidget *parent) :
	QFrame(parent)
{
	info_ = info;
	setFrameStyle(QFrame::StyledPanel);

	QHBoxLayout* hl = new QHBoxLayout(this);
	hl->addWidget(new QLabel("List of actions"));
	hl->addStretch();
}
