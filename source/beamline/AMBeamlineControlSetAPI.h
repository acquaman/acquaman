#ifndef AMBEAMLINECONTROLSETAPI_H
#define AMBEAMLINECONTROLSETAPI_H

class AMControlSet;

class AMBeamlineControlSetAPI {
public:
	AMBeamlineControlSetAPI();

	virtual ~AMBeamlineControlSetAPI();

	/// Returns the control set that contains all of the public controls.  These are used with actions for automatic lookup.
	virtual AMControlSet *exposedControls() const { return 0; }
};

#endif // AMBEAMLINECONTROLSETAPI_H
