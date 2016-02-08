#ifndef BIOXASM2MIRROR_H
#define BIOXASM2MIRROR_H

#include "beamline/BioXAS/BioXASMirror.h"

class BioXASM2Mirror : public BioXASMirror
{
    Q_OBJECT

public:
	/// Enum class for the fluorescence screen positions.
	class Screen { public: enum Position { In = 0, Out, Invalid }; };

	/// Constructor.
	explicit BioXASM2Mirror(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASM2Mirror();

	/// Returns the current connected state. True if this control is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the fluorescence screen control.
	AMControl* screen() const { return screen_; }

signals:
	/// Notifier that the fluorescence screen control has changed.
	void screenChanged(AMControl *newControl);

public slots:
	/// Sets the fluorescence screen control.
	void setScreen(AMControl *newControl);

protected:
	/// The fluorescence screen control.
	AMControl *screen_;
};

#endif // BIOXASM2MIRROR_H
