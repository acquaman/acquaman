#ifndef BIOXASSIS3820SCALERMODESVIEW_H
#define BIOXASSIS3820SCALERMODESVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASSIS3820Scaler;
class AMExtendedControlEditor;

class BioXASSIS3820ScalerModesView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSIS3820ScalerModesView(BioXASSIS3820Scaler *scaler, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSIS3820ScalerModesView();

	/// Returns the scaler being viewed.
	BioXASSIS3820Scaler* scaler() const { return scaler_; }

signals:
	/// Notifier that the scaler being viewed has changed.
	void scalerChanged(BioXASSIS3820Scaler *newScaler);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the scaler being viewed.
	void setScaler(BioXASSIS3820Scaler *newScaler);

protected:
	/// The scaler being viewed.
	BioXASSIS3820Scaler *scaler_;

	/// The inputs mode editor.
	AMExtendedControlEditor *inputsModeEditor_;
	/// The trigger source mode editor.
	AMExtendedControlEditor *triggerSourceModeEditor_;
	/// The clock source mode editor.
	AMExtendedControlEditor *clockSourceModeEditor_;
};

#endif // BIOXASSIS3820SCALERMODESVIEW_H
