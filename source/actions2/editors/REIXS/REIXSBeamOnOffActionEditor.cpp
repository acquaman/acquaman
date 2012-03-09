#include "REIXSBeamOnOffActionEditor.h"

#include <QRadioButton>
#include <QHBoxLayout>
#include <QLabel>

REIXSBeamOnOffActionEditor::REIXSBeamOnOffActionEditor(REIXSBeamOnOffActionInfo *info, QWidget *parent) :
	QFrame(parent)
{
	info_ = info;
	setFrameStyle(QFrame::StyledPanel);
	setFocusPolicy(Qt::ClickFocus);

	QRadioButton* beamOnButton = new QRadioButton("On");
	QRadioButton* beamOffButton = new QRadioButton("Off");

	QHBoxLayout* hl = new QHBoxLayout(this);
	hl->addWidget(new QLabel("Beam: "));
	hl->addWidget(beamOnButton);
	hl->addWidget(beamOffButton);
	hl->addStretch();

	beamOnButton->setChecked(info_->beamOn());
	beamOffButton->setChecked(!info_->beamOn());

	connect(beamOnButton, SIGNAL(toggled(bool)), this, SLOT(onBeamOnRadioButtonToggled(bool)));
}

#include <QDebug>
void REIXSBeamOnOffActionEditor::onBeamOnRadioButtonToggled(bool beamOn)
{
	qDebug() << "Beam on toggled:" << beamOn;
	info_->setBeamOn(beamOn);
}
