#ifndef AMCONTROLWAITACTIONINFO_H
#define AMCONTROLWAITACTIONINFO_H

#include "actions3/AMActionInfo3.h"
#include "dataman/info/AMControlInfo.h"

/// This AMActionInfo-subclass specifies the information for AMControlWaitAction -- an action that waits for a control to reach a value.  This info specifies the value we are waiting for as well as how we want to match (equal, greater than, less than), and the timeout limit.
class AMControlWaitActionInfo : public AMActionInfo3
{
    Q_OBJECT
    Q_PROPERTY(AMDbObject* controlInfo READ dbReadControlInfo WRITE dbLoadControlInfo)
    Q_PROPERTY(double timeout READ timeout WRITE setTimeout)
    Q_PROPERTY(int matchType READ matchTypeAsInt WRITE setMatchTypeAsInt)

public:
    enum MatchType {
        MatchEqual = 0,         // matches when the value is the same
        MatchGreaterThan = 1,           // matches when the current value of the control is greater than the setpoint.
        MatchLessThan = 2,           // matches when the current value of the control is less than the setpoint.
        MatchInvalid
    };

    /// Constructor. You should always specify a valid \c setpoint, but we provide the default argument because we need a default constructor for database loading.
    Q_INVOKABLE AMControlWaitActionInfo(const AMControlInfo &setpoint = AMControlInfo(), double timeout = -1, AMControlWaitActionInfo::MatchType matchType = AMControlWaitActionInfo::MatchInvalid, QObject *parent = 0);
    virtual ~AMControlWaitActionInfo();

    /// Copy Constructor
    AMControlWaitActionInfo(const AMControlWaitActionInfo &other);

    /// This function is used as a virtual copy constructor
    virtual AMActionInfo3* createCopy() const { return new AMControlWaitActionInfo(*this); }

    // Re-implemented public functions
    /////////////////////////////////

    /// This should describe the type of the action
    virtual QString typeDescription() const { return "Control Wait"; }

    // New public functions
    //////////////////////////

    /// Returns a pointer to our move destination setpoint
    const AMControlInfo* controlInfo() const { return &controlInfo_; }

    /// Returns our timeout.
    double timeout() const;

    /// Return our matchtype.
    AMControlWaitActionInfo::MatchType matchType() const;

    /// Set the move destination setpoint, including the control name, value, and description.
    /*! \note We make a copy of \c controlInfo's values, and do not retain any reference to it afterward. */
    void setControlInfo(const AMControlInfo& controlInfo);
    /// Set the move destination setpoint (value only).
    void setSetpoint(double setpoint);

    void setTimeout(double newTimeout);
    void setMatchType(AMControlWaitActionInfo::MatchType newMatchType);

    // Database loading/storing
    ////////////////////////////

    /// For database storing only.
    AMControlInfo* dbReadControlInfo() { return &controlInfo_; }
    /// For database loading only. This function will never be called since dbReadControlInfo() always returns a valid setpoint, but it needs to be here.
    void dbLoadControlInfo(AMDbObject* newLoadedObject) { delete newLoadedObject; }

signals:

public slots:
protected:
    int matchTypeAsInt() const;
    void setMatchTypeAsInt(int newMatchType);

protected:
    /// The AMControlInfo that specifies where to move to
    AMControlInfo controlInfo_;

    /// The max time to wait for the control to reach a setpoint. (seconds)
    double timeout_;

    /// The desired match type.
    AMControlWaitActionInfo::MatchType matchType_;

    /// A short helper function to update the action's description
    void updateDescriptionText();

};

#endif // AMCONTROLWAITACTIONINFO_H