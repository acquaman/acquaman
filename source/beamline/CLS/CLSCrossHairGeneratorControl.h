#ifndef CLSCROSSHAIRGENERATORCONTROL_H
#define CLSCROSSHAIRGENERATORCONTROL_H

#include <QObject>

#include "beamline/AMPVControl.h"

/// Simple implementation that models the cross hair generator used on old CLS monitors.
class CLSCrossHairGeneratorControl : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSCrossHairGeneratorControl(const QString &baseVideoChannelPVName, const QString &baseCrossHairPVName, QObject *parent = 0);
	/// Destructor.
	~CLSCrossHairGeneratorControl();

	/// Returns the first horizontal position.
	int horizontalPosition1() const;
	/// Returns the second horizontal position.
	int horizontalPosition2() const;
	/// Returns the first vertical position.
	int verticalPosition1() const;
	/// Returns the second vertical position.
	int verticalPosition2() const;

	/// Returns the first horizontal type.
	int horizontalType1() const;
	/// Returns the second horizontal type.
	int horizontalType2() const;
	/// Returns the first vertical type.
	int verticalType1() const;
	/// Returns the second vettical type.
	int verticalType2() const;

	/// Returns the display button state.
	bool displayState() const;
	/// Returns the box mode state.
	bool boxState() const;
	/// Returns the intensity of the cross hair lines.
	int intensity() const;

	/// Returns the current channel.
	int channel() const;

signals:
	/// Notifier that the first horizontal position changed.
	void horizontalPosition1Changed();
	/// Notifier that the second horizontal position changed.
	void horizontalPosition2Changed();
	/// Notifier that the first vertical position changed.
	void verticalPosition1Changed();
	/// Notifier that the second vertical position changed.
	void verticalPosition2Changed();

	/// Notifier that the first horizontal type changed.
	void horizontalType1Changed();
	/// Notifier that the second horizontal type changed.
	void horizontalType2Changed();
	/// Notifier that the first vertical type changed.
	void verticalType1Changed();
	/// Notifier that the second vertical type changed.
	void verticalType2Changed();

	/// Notifier that the display state changed.
	void displayStateChanged();
	/// Notifier that the box state changed.
	void boxStateChanged();
	/// Notifier that the intensity changed.
	void intensityChanged();

	/// Notifier that the channel has changed.
	void channelChanged();

public slots:
	/// Sets the first horizontal position.
	void setHorizontalPosition1(int value);
	/// Sets the second horizontal position.
	void setHorizontalPosition2(int value);
	/// Sets the first vertical position.
	void setVerticalPosition1(int value);
	/// Sets the first vertical position.
	void setVerticalPosition2(int value);

	/// Sets the first horizontal type.
	void setHorizontalType1(int value);
	/// Sets the second horizontal type.
	void setHorizontalType2(int value);
	/// Sets the first vertical type.
	void setVerticalType1(int value);
	/// Sets the first vertical type.
	void setVerticalType2(int value);

	/// Sets the display state.
	void setDisplayState(bool state);
	/// Sets the box state.
	void setBoxState(bool state);
	/// Sets the intensity.
	void setIntensity(int value);

	/// Sets the channel.
	void setChannel(int value);

protected:
	/// Controls the first horizontal position.
	AMPVControl *horizontalPosition1_;
	/// Controls the second horizontal position.
	AMPVControl *horizontalPosition2_;
	/// Controls the first vertical position.
	AMPVControl *verticalPosition1_;
	/// Controls the second vertical position.
	AMPVControl *verticalPosition2_;

	/// Controls the first horizontal type.
	AMPVControl *horizontalType1_;
	/// Controls the second horizontal type.
	AMPVControl *horizontalType2_;
	/// Controls the first vertical type.
	AMPVControl *verticalType1_;
	/// Controls the second vettical type.
	AMPVControl *verticalType2_;

	/// Controls the display button state.
	AMPVControl *displayState_;
	/// Controls the box mode state.
	AMPVControl *boxState_;
	/// Controls the intensity of the cross hair lines.
	AMPVControl *intensity_;

	/// Controls the channel select.
	AMPVControl *channel_;
};

#endif // CLSCROSSHAIRGENERATORCONTROL_H
