#ifndef CLSCROSSHAIRGENERATORCONTROLVIEW_H
#define CLSCROSSHAIRGENERATORCONTROLVIEW_H

#include <QWidget>

#include "beamline/CLS/CLSCrossHairGeneratorControl.h"

#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>

/// Simple view for changing the cross hairs of the AXIS stream.
class CLSCrossHairGeneratorControlView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSCrossHairGeneratorControlView(CLSCrossHairGeneratorControl *crossHairGenerator, QWidget *parent = 0);
	/// Destructor.
	~CLSCrossHairGeneratorControlView();

signals:

public slots:

protected slots:
	/// Handles changes to the channel.
	void onChannelChanged(int index);

	/// Handles changes to the first horizontal position.
	void onHorizontalPosition1Changed(int value);
	/// Handles changes to the second horizontal position.
	void onHorizontalPosition2Changed(int value);
	/// Handles changes to the first vertical position.
	void onVerticalPosition1Changed(int value);
	/// Handles changes to the second vertical position.
	void onVerticalPosition2Changed(int value);

	/// Handles changes to the first horizontal type.
	void onHorizontalType1Changed(int value);
	/// Handles changes to the second horizontal type.
	void onHorizontalType2Changed(int value);
	/// Handles changes to the first vertical type.
	void onVerticalType1Changed(int value);
	/// Handles changes to the second vertical type.
	void onVerticalType2Changed(int value);

	/// Handles changes to the display state.
	void onDisplayStateChanged(bool state);
	/// Handles changes to the box state.
	void onBoxStateChanged(bool state);
	/// Handles changes to the intensity.
	void onIntensityChanged(int value);

	/// Handles model updates to the channel.
	void updateChannelChanged();

	/// Handles model updates to the first horizontal position.
	void updateHorizontalPosition1Changed();
	/// Handles model updates to the second horizontal position.
	void updateHorizontalPosition2Changed();
	/// Handles model updates to the first vertical position.
	void updateVerticalPosition1Changed();
	/// Handles model updates to the second vertical position.
	void updateVerticalPosition2Changed();

	/// Handles model updates to the first horizontal type.
	void updateHorizontalType1Changed();
	/// Handles model updates to the second horizontal type.
	void updateHorizontalType2Changed();
	/// Handles model updates to the first vertical type.
	void updateVerticalType1Changed();
	/// Handles model updates to the second vertical type.
	void updateVerticalType2Changed();

	/// Handles model updates to the display state.
	void updateDisplayStateChanged();
	/// Handles model updates to the box state.
	void updateBoxStateChanged();
	/// Handles model updates to the intensity.
	void updateIntensityChanged();

protected:
	/// The cross hair generator.
	CLSCrossHairGeneratorControl *crossHairGenerator_;

	/// The channel selection combo box.
	QComboBox *channel_;

	/// The first horizontal position spin box.
	QSpinBox *horizontalPosition1_;
	/// The first horizontal position slider.
	QSlider *horizontalSlider1_;
	/// The second horizontal position spin box.
	QSpinBox *horizontalPosition2_;
	/// The second horizontal position slider.
	QSlider *horizontalSlider2_;
	/// The first vertical position spin box.
	QSpinBox *verticalPosition1_;
	/// The first vertical position slider.
	QSlider *verticalSlider1_;
	/// The second vertical position spin box.
	QSpinBox *verticalPosition2_;
	/// The second vertical position slider.
	QSlider *verticalSlider2_;

	/// The first horizontal type combo box.
	QComboBox *horizontalType1_;
	/// The second horizontal type combo box.
	QComboBox *horizontalType2_;
	/// The first vertical type combo box.
	QComboBox *verticalType1_;
	/// The second vertical type combo box.
	QComboBox *verticalType2_;

	/// The display state button.
	QPushButton *displayButton_;
	/// The box state button.
	QPushButton *boxButton_;
	/// The slider for the intensity.
	QSlider *intensity_;
};

#endif // CLSCROSSHAIRGENERATORCONTROLVIEW_H
