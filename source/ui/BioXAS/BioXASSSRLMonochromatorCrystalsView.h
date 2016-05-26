#ifndef BIOXASSSRLMONOCHROMATORCRYSTALSVIEW_H
#define BIOXASSSRLMONOCHROMATORCRYSTALSVIEW_H

#include <QWidget>
#include <QLayout>
#include <QGroupBox>

class BioXASSSRLMonochromator;
class AMExtendedControlEditor;
class CLSControlEditor;

class BioXASSSRLMonochromatorCrystalsView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorCrystalsView(BioXASSSRLMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorCrystalsView();

	/// Returns the monochromator being viewed.
	BioXASSSRLMonochromator* mono() const { return mono_; }

signals:
	/// Notifier that the monochromator being viewed has changed.
	void monoChanged(BioXASSSRLMonochromator *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the monochromator being viewed.
	void setMono(BioXASSSRLMonochromator *newControl);

protected slots:
	/// Updates the crystal 1 pitch editor.
	void updateCrystal1PitchEditor();
	/// Updates the crystal 1 roll editor.
	void updateCrystal1RollEditor();
	/// Updates the crystal 2 pitch editor.
	void updateCrystal2PitchEditor();
	/// Updates the crystal 2 roll editor.
	void updateCrystal2RollEditor();

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

	/// The crystal 1 pitch editor.
	CLSControlEditor *crystal1PitchEditor_;
	/// The crystal 1 roll editor.
	CLSControlEditor *crystal1RollEditor_;
	/// The crystal 2 pitch editor.
	CLSControlEditor *crystal2PitchEditor_;
	/// The crystal 2 roll editor.
	CLSControlEditor *crystal2RollEditor_;
};

#endif // BIOXASSSRLMONOCHROMATORCRYSTALSVIEW_H
