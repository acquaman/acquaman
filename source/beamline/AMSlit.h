#ifndef AMSLIT_H
#define AMSLIT_H

#include "beamline/AMControl.h"
#include "beamline/AMSlitControl.h"
#include "beamline/AMControlSet.h"

#define AMSLIT_CANNOT_OPEN 39810
#define AMSLIT_CANNOT_CLOSE 39811

class AMSlit : public AMControl
{
	Q_OBJECT

public:
	/// Enumeration of possible slit blade control orientations.
	enum BladeOrientation { OpensPositively = 0, OpensNegatively = 1 };

	/// Constructor.
	explicit AMSlit(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMSlit();

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns true if opening is supported.
	virtual bool canOpen() const;
	/// Returns true if closing is supported.
	virtual bool canClose() const;

	/// Returns true if the first blade opens positively, false otherwise.
	bool firstBladeOpensPositively() const;
	/// Returns true if the first blade opens negatively, false otherwise.
	bool firstBladeOpensNegatively() const;
	/// Returns true if the second blade opens positively, false otherwise.
	bool secondBladeOpensPositively() const;
	/// Returns true if the second blade opens negatively, false otherwise.
	bool secondBladeOpensNegatively() const;

	/// Returns the current value of the first blade control.
	double firstBladeValue() const;
	/// Returns the current value of the second blade control.
	double secondBladeValue() const;
	/// Returns the current gap value.
	double gapValue() const;
	/// Returns the current center value.
	double centerValue() const;

	/// Returns the first blade control.
	AMControl* firstBlade() const { return firstBlade_; }
	/// Returns the second blade control.
	AMControl* secondBlade() const { return secondBlade_; }
	/// Returns the gap control.
	AMSlitControl* gap() const { return gap_; }
	/// Returns the center control.
	AMSlitControl* center() const { return center_; }

	/// Adds a child control. Reimplemented to add the child to the set of all subcontrols.
	virtual void addChildControl(AMControl *control);
	/// Removes a child control. Reimplemented to remove the child from the set of all subcontrols.
	virtual void removeChildControl(AMControl *control);

	/// Creates and returns an action that opens the slit.
	virtual AMAction3* createOpenAction();
	/// Creates and returns an action that closes the slit.
	virtual AMAction3* createCloseAction();

signals:
	/// Notifier that the first blade control value has changed.
	void firstBladeValueChanged(double newValue);
	/// Notifier that the first blade has changed.
	void firstBladeChanged(AMControl *newControl);
	/// Notifier that the second blade control value has changed.
	void secondBladeValueChanged(double newValue);
	/// Notifier that the second blade control has changed.
	void secondBladeChanged(AMControl *newControl);
	/// Notifier that the gap control value has changed.
	void gapValueChanged(double newValue);
	/// Notifier that the center control value has changed.
	void centerValueChanged(double newValue);

public slots:
	/// Sets the first blade control.
	void setFirstBlade(AMControl *newControl, BladeOrientation orientation);
	/// Removes the first blade control.
	void removeFirstBlade();
	/// Sets the second blade control.
	void setSecondBlade(AMControl *newControl, BladeOrientation orientation);
	/// Removes the second blade control.
	void removeSecondBlade();

	/// Sets the vertical maximum gap. The 'Open' setpoints for each blade motor are calculated from this value. Opening the slit becomes enabled when this value is set. For blade motors that don't have encoders, this value should be larger than their maximum value.
	void setOpenGapValue(double openGap);
	/// Removes the vertical maximum gap set.
	void removeOpenGapValue();

	/// Sets the horizontal maximum gap. The 'Open' setpoints for each blade motor are calculated from this value. Opening the slit becomes enabled when this value is set. For blade motors that don't have encoders, this value should be larger than their maximum value.
	void setClosedGapValue(double openGap);
	/// Removes the horizontal maximum gap set.
	void removeClosedGapValue();

	/// Opens the slit, if opening is supported.
	void open();
	/// Closes the slit, if closing is supported.
	void close();

protected slots:
	/// Sets the blade open setpoints.
	void setOpenValues(double firstBladeOpenValue, double secondBladeOpenValue);
	/// Sets the blade closed setpoints.
	void setClosedValues(double firstBladeClosedValue, double secondBladeClosedValue);

	/// Updates the gap control.
	void updateGap();
	/// Updates the center control.
	void updateCenter();

protected:
	/// Returns true if the first blade can open, false otherwise.
	bool firstBladeCanOpen() const;
	/// Returns true if the second blade can open, false otherwise.
	bool secondBladeCanOpen() const;
	/// Returns true if the first blade can close, false otherwise.
	bool firstBladeCanClose() const;
	/// Returns true if the second blade can close, false otherwise.
	bool secondBladeCanClose() const;

protected:
	/// The set of all subcontrols.
	AMControlSet *allControls_;

	/// The first blade control.
	AMControl *firstBlade_;
	/// The second blade control.
	AMControl *secondBlade_;
	/// The gap control.
	AMSlitControl *gap_;
	/// The center control.
	AMSlitControl *center_;

	/// The blade control orientation map.
	QMap<AMControl*, double> bladeOrientationMap_;
	/// The blade open value map.
	QMap<AMControl*, double> bladeOpenValueMap_;
	/// The blade closed value map.
	QMap<AMControl*, double> bladeClosedValueMap_;
};

#endif // AMSLIT_H
