#ifndef BIOXASSSRLMONOCHROMATORMASKVIEW_H
#define BIOXASSSRLMONOCHROMATORMASKVIEW_H

#include <QWidget>
#include <QLayout>

class AMExtendedControlEditor;
class BioXASSSRLMonochromator;
class CLSControlEditor;

class BioXASSSRLMonochromatorMaskView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorMaskView(BioXASSSRLMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorMaskView();

	/// Returns the mono being viewed.
	BioXASSSRLMonochromator* mono() const { return mono_; }

signals:
	/// Notifier that the mono being viewed has changed.
	void monoChanged(BioXASSSRLMonochromator *newMono);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the mono being viewed.
	void setMono(BioXASSSRLMonochromator *newMono);

protected slots:
	/// Updates the mask state editor.
	void updateStateEditor();
	/// Updates the mask upper blade editor.
	void updateUpperBladeEditor();
	/// Updates the mask lower blade editor.
	void updateLowerBladeEditor();

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

	/// The mono mask state editor.
	CLSControlEditor *stateEditor_;
	/// The mono mask upper blade control editor.
	CLSControlEditor *upperBladeEditor_;
	/// The mono mask lower blade control editor.
	CLSControlEditor *lowerBladeEditor_;
};

#endif // BIOXASSSRLMONOCHROMATORMASKVIEW_H
