#ifndef BIOXASSIDEPOEBEAMSTATUSCONTROLEDITORSVIEW_H
#define BIOXASSIDEPOEBEAMSTATUSCONTROLEDITORSVIEW_H

#include <QWidget>
#include <QLayout>

#include "ui/BioXAS/BioXASSidePOEBeamStatusControlView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASSidePOEBeamStatusControlEditorsView : public BioXASSidePOEBeamStatusControlView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSidePOEBeamStatusControlEditorsView(BioXASSidePOEBeamStatusControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSidePOEBeamStatusControlEditorsView();

	/// Returns the beam status editor.
	AMExtendedControlEditor* beamStatusEditor() const { return beamStatusEditor_; }
	/// Returns the mirror mask editor.
	AMExtendedControlEditor* mirrorMaskEditor() const { return mirrorMaskEditor_; }
	/// Returns the mono mask editor.
	AMExtendedControlEditor* monoMaskEditor() const { return monoMaskEditor_; }

public slots:
	/// Clears the view.
	virtual void clear();
	/// Updates the view.
	virtual void update();

protected:
	/// The beam status editor.
	AMExtendedControlEditor *beamStatusEditor_;
	/// The mirror mask editor.
	AMExtendedControlEditor *mirrorMaskEditor_;
	/// The mono mask editor.
	AMExtendedControlEditor *monoMaskEditor_;
};

#endif // BIOXASSIDEPOEBEAMSTATUSCONTROLEDITORSVIEW_H
