#ifndef BIOXASSSRLMONOCHROMATORBASICVIEW_H
#define BIOXASSSRLMONOCHROMATORBASICVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASSSRLMonochromator;
class AMExtendedControlEditor;
class BioXASSSRLMonochromatorRegionControlEditor;

class BioXASSSRLMonochromatorBasicView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorBasicView(BioXASSSRLMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorBasicView();

	/// Returns the monochromator being viewed.
	BioXASSSRLMonochromator* mono() const { return mono_; }

signals:
	/// Notifier that the monochromator being viewed has changed.
	void monoChanged(BioXASSSRLMonochromator *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the mono being viewed.
	void setMono(BioXASSSRLMonochromator *newControl);

protected slots:
	/// Updates the energy editor.
	void updateEnergyEditor();
	/// Updates the bragg editor.
	void updateBraggEditor();
	/// Updates the region editor.
	void updateRegionEditor();

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

	/// The energy editor.
	AMExtendedControlEditor *energyEditor_;
	/// The bragg angle editor.
	AMExtendedControlEditor *braggAngleEditor_;
	/// The region editor.
	BioXASSSRLMonochromatorRegionControlEditor *regionEditor_;
};

#endif // BIOXASSSRLMONOCHROMATORBASICVIEW_H
