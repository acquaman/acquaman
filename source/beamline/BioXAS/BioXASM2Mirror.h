#ifndef BIOXASM2MIRROR_H
#define BIOXASM2MIRROR_H

#include <QObject>

#include "beamline/AMPVControl.h"

class BioXASM2Mirror : public QObject
{
    Q_OBJECT

public:
	/// Enum class for the fluorescence screen positions.
	class Screen { public: enum Position { In = 0, Out, Invalid }; };

	/// Constructor.
	explicit BioXASM2Mirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASM2Mirror();

	/// Returns true if the mirror is connected, false otherwise.
	bool isConnected() const { return isConnected_; }
	/// Returns the current fluorescence screen position.
	double screenPosition() const;

	/// Returns the fluorescence screen control.
	AMPVControl* screenControl() const { return screen_; }

signals:
	/// Notifier that the mirror's connected state has changed.
	void connected(bool isConnected);

public slots:
	/// Sets the screen control position.
	void setScreenPosition(Screen::Position newPosition);

protected slots:
	/// Sets the current connected state.
	void setConnected(bool isConnected);

	/// Handles updating the current connected state.
	void onConnectedChanged();

protected:
	/// The current connected state.
	bool isConnected_;
	/// The control for the mirror fluorescence screen.
	AMPVControl *screen_;

};

#endif // BIOXASM2MIRROR_H
