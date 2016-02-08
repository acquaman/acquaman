#ifndef BIOXASSSRLMONOCHROMATORMASKVIEW_H
#define BIOXASSSRLMONOCHROMATORMASKVIEW_H

#include <QWidget>
#include <QLayout>

class AMExtendedControlEditor;
class BioXASSSRLMonochromatorMask;
class BioXASControlEditor;

class BioXASSSRLMonochromatorMaskView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorMaskView(BioXASSSRLMonochromatorMask *mask, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorMaskView();

	/// Returns the mask being viewed.
	BioXASSSRLMonochromatorMask* mask() const { return mask_; }

signals:
	/// Notifier that the mask being viewed has changed.
	void maskChanged(BioXASSSRLMonochromatorMask *newMask);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the mask being viewed.
	void setMask(BioXASSSRLMonochromatorMask *newMask);

protected slots:
	/// Updates the mask state editor.
	void updateStateEditor();
	/// Updates the mask upper blade editor.
	void updateUpperBladeEditor();
	/// Updates the mask lower blade editor.
	void updateLowerBladeEditor();

protected:
	/// The mask being viewed.
	BioXASSSRLMonochromatorMask *mask_;

	/// The mask state editor.
	BioXASControlEditor *stateEditor_;
	/// The upper blade editor.
	BioXASControlEditor *upperBladeEditor_;
	/// The lower blade editor.
	BioXASControlEditor *lowerBladeEditor_;
};

#endif // BIOXASSSRLMONOCHROMATORMASKVIEW_H
