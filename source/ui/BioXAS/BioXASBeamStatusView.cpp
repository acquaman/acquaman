#include "BioXASBeamStatusView.h"

#include "beamline/CLS/CLSBeamlineStatus.h"

#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"

#include "ui/CLS/CLSBeamlineStatusView.h"
#include "ui/CLS/CLSControlEditor.h"

BioXASBeamStatusView::BioXASBeamStatusView(CLSBeamlineStatus *beamStatus, QWidget *parent) :
	CLSBeamlineStatusView(beamStatus, parent)
{

}

BioXASBeamStatusView::~BioXASBeamStatusView()
{

}


QWidget* BioXASBeamStatusView::createComponentView(AMControl *control)
{
	QWidget *view = 0;

	if (control) {

		bool controlFound = false;

		BioXASShutters *shutters = qobject_cast<BioXASShutters*>(control);
		if (!controlFound && shutters) {

			QVBoxLayout *shuttersViewLayout = new QVBoxLayout();

			foreach (AMControl *shutter, shutters->shuttersList())
				shuttersViewLayout->addWidget(new CLSControlEditor(shutter));

			view = new QWidget();
			view->setLayout(shuttersViewLayout);

			controlFound = true;
		}

		BioXASValves *valves = qobject_cast<BioXASValves*>(control);
		if (!controlFound && valves) {
			CLSControlEditor *editor = new CLSControlEditor(valves);
			editor->setTitle("Valves");

			view = editor;
			controlFound = true;
		}

		BioXASM1MirrorMaskState *mirrorMask = qobject_cast<BioXASM1MirrorMaskState*>(control);
		if (!controlFound && mirrorMask) {
			CLSControlEditor *editor = new CLSControlEditor(mirrorMask);
			editor->setTitle("Mirror Mask");

			view = editor;
			controlFound = true;
		}

		BioXASSSRLMonochromatorMaskState *monoMask = qobject_cast<BioXASSSRLMonochromatorMaskState*>(control);
		if (!controlFound && monoMask) {
			CLSControlEditor *editor = new CLSControlEditor(monoMask);
			editor->setTitle("Monochromator Mask");

			view = editor;
			controlFound = true;
		}
	}

	return view;
}
