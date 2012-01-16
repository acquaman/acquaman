#ifndef REIXSCONTROLMOVEACTION_H
#define REIXSCONTROLMOVEACTION_H

#include "actions2/actions/AMControlMoveAction.h"
#include "actions2/actions/REIXS/REIXSControlMoveActionInfo.h"
#include <QMap>

/// This subclass of AMControlMoveAction provides the required lookup of controls on the REIXS beamline at the CLS.
class REIXSControlMoveAction : public AMControlMoveAction
{
    Q_OBJECT
public:
	// Constructors and copying
	//////////////////////////////

	/// Constructor. Requires and takes ownership of an existing AMControlMoveActionInfo \c info.
	Q_INVOKABLE REIXSControlMoveAction(REIXSControlMoveActionInfo* info, QObject *parent = 0) : AMControlMoveAction(info, parent) {}
	/// Copy constructor: must re-implement, but can simply use the base class copy constructor.
	REIXSControlMoveAction(const REIXSControlMoveAction& other) : AMControlMoveAction(other) {}
	/// Virtual copy constructor
	virtual REIXSControlMoveAction* createCopy() const { return new REIXSControlMoveAction(*this); }

	// Re-implemented virtual functions
	/////////////////////////

	/// Re-implemented to return a control from the REIXS beamline.
	AMControl* getControlForAction(const AMControlInfo &controlInfo) const;

signals:

public slots:

protected:
	/// Holds a mapping from control names to AMControl instances in REIXSBeamline. (This could alternatively be built into the beamline class itself, or we could use an AMControlSet. All of these options let us restrict which AMControls are accessible to Actions.)
	static QMap<QString, AMControl*> name2Control_;
	/// Helper function to populate the mapping, if it's empty.
	void populateControlLookup() const;

};

#endif // REIXSCONTROLMOVEACTION_H
