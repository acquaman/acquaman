#include "BioXASMirrorBendView.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASMirrorBendView::BioXASMirrorBendView(BioXASMirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	// Create bend editor.

	bendEditor_ = new BioXASControlEditor(0);
	bendEditor_->setTitle("Radius");
	bendEditor_->setFormat('f');
	bendEditor_->setPrecision(2);

	// Create bender view.

	upstreamEditor_ = new BioXASControlEditor(0);
	upstreamEditor_->setTitle("Upstream bender");
	upstreamEditor_->setFormat('f');
	upstreamEditor_->setPrecision(2);

	downstreamEditor_ = new BioXASControlEditor(0);
	downstreamEditor_->setTitle("Downstream bender");
	downstreamEditor_->setFormat('f');
	downstreamEditor_->setPrecision(2);

	QHBoxLayout *benderBoxLayout = new QHBoxLayout();
	benderBoxLayout->addWidget(upstreamEditor_);
	benderBoxLayout->addWidget(downstreamEditor_);

	QGroupBox *benderBox = new QGroupBox();
	benderBox->setFlat(true);
	benderBox->setLayout(benderBoxLayout);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(bendEditor_);
	layout->addWidget(benderBox);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);
}

BioXASMirrorBendView::~BioXASMirrorBendView()
{

}

void BioXASMirrorBendView::refresh()
{
	// Clear the view.

	bendEditor_->setControl(0);
	upstreamEditor_->setControl(0);
	downstreamEditor_->setControl(0);

	// Update view element.

	if (mirror_) {
		bendEditor_->setControl(mirror_->bend());
		upstreamEditor_->setControl(mirror_->upstreamBenderMotor());
		downstreamEditor_->setControl(mirror_->downstreamBenderMotor());
	}
}

void BioXASMirrorBendView::setMirror(BioXASMirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_)
			disconnect( mirror_, 0, this, 0 );

		mirror_ = newMirror;

		if (mirror_) {
			connect( mirror_, SIGNAL(bendChanged(BioXASMirrorBendControl*)), this, SLOT(updateBendEditor()) );
			connect( mirror_, SIGNAL(upstreamBenderMotorChanged(CLSMAXvMotor*)), this, SLOT(updateUpstreamEditor()) );
			connect( mirror_, SIGNAL(downstreamBenderMotorChanged(CLSMAXvMotor*)), this, SLOT(updateDownstreamEditor()) );
		}

		refresh();

		emit mirrorChanged(mirror_);
	}
}

void BioXASMirrorBendView::updateBendEditor()
{
	AMControl *bendControl = 0;

	if (mirror_)
		bendControl = mirror_->bend();

	bendEditor_->setControl(bendControl);
}

void BioXASMirrorBendView::updateUpstreamEditor()
{
	AMControl *upstreamControl = 0;

	if (mirror_)
		upstreamControl = mirror_->upstreamBenderMotor();

	upstreamEditor_->setControl(upstreamControl);
}

void BioXASMirrorBendView::updateDownstreamEditor()
{
	AMControl *downstreamControl = 0;

	if (mirror_)
		downstreamControl = mirror_->downstreamBenderMotor();

	downstreamEditor_->setControl(downstreamControl);
}
