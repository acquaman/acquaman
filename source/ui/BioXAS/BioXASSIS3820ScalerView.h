#ifndef BIOXASSIS3820SCALERVIEW_H
#define BIOXASSIS3820SCALERVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QCheckBox>

class CLSSIS3820Scaler;

class BioXASSIS3820ScalerView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSIS3820ScalerView(CLSSIS3820Scaler *scaler, bool showOptions = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSIS3820ScalerView();

	/// Returns the scaler being viewed.
	CLSSIS3820Scaler* scaler() const { return scaler_; }

signals:

public slots:
	/// Sets whether the scaler view options are shown.
	void setOptionsVisible(bool isVisible);
	/// Sets whether the scaler channel views are shown.
	void setChannelViewsVisible(bool isVisible);
	/// Sets whether the dark current widget is shown.
	void setDarkCurrentVisible(bool isVisible);

protected:
	/// The scaler being viewed.
	CLSSIS3820Scaler *scaler_;

	/// The scaler controls view.
	QGroupBox *controlsBox_;
	/// The scaler view options.
	QGroupBox *optionsBox_;
	/// The scaler channels view.
	QGroupBox *channelsBox_;
	/// The scaler dark current widget.
	QGroupBox *darkCurrentBox_;
};

#endif // BIOXASSIS3820SCALERVIEW_H
