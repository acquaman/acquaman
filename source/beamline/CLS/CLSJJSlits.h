#ifndef CLSJJSLITS_H
#define CLSJJSLITS_H

#include <QObject>

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSJJSlitGapControl.h"
#include "beamline/CLS/CLSJJSlitCenterControl.h"

class CLSJJSlitScanConfiguration;

/// The class definition of CLSJJSlit, which contains a vertical CLSJJSlitsBladesControl and a horizontal CLSJJSlitsBladesControl
class CLSJJSlits : public QObject
{
	Q_OBJECT

public:
	/// Enum describing different slit directions.
	class Direction { public: enum Option { Invalid = 0, Vertical = 1, Horizontal = 2 }; };
	/// Enum describing different slit properties.
	class Property { public: enum Option { Invalid = 0, Gap = 1, Center = 2 }; };

	/// Constructor
	explicit CLSJJSlits(const QString &name, const QString &upperBladePVName, const QString &lowerBladePVName, const QString &inboardBladePVName, const QString &outboardBladePVName, QObject*parent = 0);
	/// Destructor
	virtual ~CLSJJSlits();
	/// Returns the name.
	const QString name() const { return name_; }
	/// Returns the current connected state.
	bool isConnected() const { return connected_; }

	/// Returns the upper blade motor control.
	AMControl* upperBladeControl() const { return upperBlade_; }
	/// Returns the lower blade motor control.
	AMControl* lowerBladeControl() const { return lowerBlade_; }
	/// Returns the inboard blade motor control.
	AMControl* inboardBladeControl() const { return inboardBlade_; }
	/// Returns the outboard blade motor control.
	AMControl* outboardBladeControl() const { return outboardBlade_; }

	/// Returns the vertical blades gap control.
	AMControl* verticalGapControl() const { return verticalGap_; }
	/// Returns the vertical blades center control.
	AMControl* verticalCenterControl() const { return verticalCenter_; }
	/// Returns the horizontal blades gap control.
	AMControl* horizontalGapControl() const { return horizontalGap_; }
	/// Returns the horizontal blades center control.
	AMControl* horizontalCenterControl() const { return horizontalCenter_; }

	/// Returns the slit scan configuration.
	CLSJJSlitScanConfiguration* slitScanConfiguration() const { return slitScanConfiguration_; }

	/// Returns the control with the given direction and value.
	AMControl* control(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property);
	/// Returns a list of controls with the given direction.
	QList<AMControl*> controls(CLSJJSlits::Direction::Option direction);
	/// Returns a list of controls with the given value.
	QList<AMControl*> controls(CLSJJSlits::Property::Option property);

	/// Returns a newly-created action that moves the specified control to the desired setpoint.
	AMAction3* createMoveAction(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property, double setpoint);
	/// Returns a newly-created action that move all of the controls to the desired positions.
	AMAction3* createMoveAction(double verticalGapSetpoint, double verticalCenterSetpoint, double horizontalGapSetpoint, double horizontalCenterSetpoint);

	/// Returns a newly-created action that moves all controls to the origin (0, 0).
	AMAction3* createMoveToOriginAction();
	/// Returns a newly-created action that moves the desired controls to the origin.
	AMAction3* createMoveToOriginAction(CLSJJSlits::Direction::Option direction);

	/// Returns a newly-created action that moves the desired controls to the minimum gap.
	AMAction3* createMoveToMinGapAction(CLSJJSlits::Direction::Option direction);
	/// Returns a newly-created action that moves the desired controls to the maximum gap.
	AMAction3* createMoveToMaxGapAction(CLSJJSlits::Direction::Option direction);

	/// Returns a newly-created action that moves the desired controls so the gap is closed.
	AMAction3* createCloseGapAction(CLSJJSlits::Direction::Option direction);
	/// Returns a newly-created action that moves the desired controls to the widest possible gap.
	AMAction3* createOpenGapAction(CLSJJSlits::Direction::Option direction);

	/// Returns a newly-created action that moves the desired control to the min position.
	AMAction3* createMoveToMinPositionAction(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property);
	/// Returns a newly-created action that moves the desired control to the max position.
	AMAction3* createMoveToMaxPositionAction(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property);
	/// Returns a newly-created action that moves the desired controls to the max position.
	AMAction3* createMoveToMaxPositionAction(CLSJJSlits::Property::Option property);

	/// Returns a newly-created action that scans the given control for its optimal value, and applies the optimization.
	AMAction3* createOptimizationAction(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property);
	/// Returns a newly-created action that scans the given controls for their optimal values, and applies the optimization.
	AMAction3* createOptimizationAction(CLSJJSlits::Property::Option property);

	/// Returns a string representation of the direction.
	static QString directionToString(CLSJJSlits::Direction::Option direction);
	/// Returns a string representation of the property.
	static QString propertyToString(CLSJJSlits::Property::Option property);

	/// Returns the direction corresponding to the given string, Invalid if no match is found.
	static CLSJJSlits::Direction::Option stringToDirection(const QString &string);
	/// Returns the property corresponding to the given string, Invalid if no match is found.
	static CLSJJSlits::Property::Option stringToProperty(const QString &string);

signals:
	/// Notifier that the connected state has changed.
	void connectedChanged(bool isConnected);

protected slots:
	/// Sets the current connected state.
	void setConnected(bool isConnected);

	/// Handles updating the current connected state.
	void updateConnected();

	/// Clears the configuration of all scan axes/controls.
	void clearConfiguration(AMGenericStepScanConfiguration *configuration);
	/// Sets the configuration control at the given scan axis.
	virtual void setConfigurationControl(AMGenericStepScanConfiguration *configuration, int scanAxis, AMControl *control);
	/// Sets up a default configuration, clearing the configuration and setting the configuration controls.
	virtual void setupDefaultConfiguration(const QList<AMControl*> &controls, AMGenericStepScanConfiguration *configuration);

protected:
	/// The name.
	QString name_;
	/// Current connected state.
	bool connected_;

	/// Upper blade motor control.
	AMPVwStatusControl *upperBlade_;
	/// Lower blade motor control.
	AMPVwStatusControl *lowerBlade_;
	/// Inboard blade motor control.
	AMPVwStatusControl *inboardBlade_;
	/// Outboard blade motor control.
	AMPVwStatusControl *outboardBlade_;

	/// Vertical blades gap control.
	CLSJJSlitGapControl *verticalGap_;
	/// Vertical blades center control.
	CLSJJSlitCenterControl *verticalCenter_;
	/// Horizontal blades gap control.
	CLSJJSlitGapControl *horizontalGap_;
	/// Horizontal blades center control.
	CLSJJSlitCenterControl *horizontalCenter_;

	/// The JJ slits scan configuration.
	AMGenericStepScanConfiguration *configuration_;
};

#endif // CLSJJSLITS_H
