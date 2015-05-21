#ifndef BIOXASM2MIRROR_H
#define BIOXASM2MIRROR_H

#include <QObject>

#include "beamline/BioXAS/BioXASMirror.h"

class BioXASM2Mirror : public BioXASMirror
{
    Q_OBJECT

public:
	/// Enum class for the fluorescence screen positions.
	class Screen { public: enum Position { In = 0, Out, Invalid }; };

	/// Constructor.
	explicit BioXASM2Mirror(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASM2Mirror();

	/// Returns the fluorescence screen control.
	AMControl* screenControl() const { return screen_; }

public slots:
	/// Sets the screen control position.
	void setScreenPosition(Screen::Position newPosition);

protected slots:
	/// Updates the connected state.
	virtual void updateConnected();

protected:
	/// The mirror fluorescence screen control.
	AMSinglePVControl *screen_;

};

#endif // BIOXASM2MIRROR_H
