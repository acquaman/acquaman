#ifndef BIOXASSOLLERSLITVIEW_H
#define BIOXASSOLLERSLITVIEW_H

#include <QWidget>

#include "ui/CLS/CLSControlEditor.h"

class BioXASSollerSlit;

class BioXASSollerSlitView : public QWidget
{
    Q_OBJECT

public:
	/// Destructor.
	explicit BioXASSollerSlitView(BioXASSollerSlit *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSollerSlitView();

	/// Returns the Soller slits being viewed.
	BioXASSollerSlit* control() const { return sollerSlit_; }

signals:
	/// Notifier that the Soller slit being viewed has changed.
	void controlChanged(BioXASSollerSlit *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the Soller slit being viewed.
	void setControl(BioXASSollerSlit *newControl);

protected slots:
	/// Updates the x motor editor.
	void updateXMotorEditor();
	/// Updates the z motor editor.
	void updateZMotorEditor();

protected:
	/// The Soller slits being viewed.
	BioXASSollerSlit *sollerSlit_;

	/// The x motor editor.
	CLSControlEditor *xMotorEditor_;
	/// The z motor editor.
	CLSControlEditor *zMotorEditor_;
};

#endif // BIOXASSOLLERSLITVIEW_H
