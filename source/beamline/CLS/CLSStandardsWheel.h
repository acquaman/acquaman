#ifndef CLSSTANDARDSWHEEL_H
#define CLSSTANDARDSWHEEL_H

#include <QObject>

#include "beamline/CLS/CLSMAXvMotor.h"

/// This is a wheel element.  For the time being it only holds the name and position on the wheel.
class CLSStandardsWheelElement : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Builds a wheel element with the given name and position (should be between 0 and 360).
	explicit CLSStandardsWheelElement(const QString &name = "", double position = 0.0, QObject *parent = 0);
	/// Destructor.
	~CLSStandardsWheelElement();

	/// Returns the name.
	const QString &name() const { return name_; }
	/// Returns the position.
	double position() const { return position_; }

signals:
	/// Notifier that the name has changed.
	void nameChanged(const QString &);
	/// Notifier that the position has changed.
	void positionChanged(double);

public slots:
	/// Sets the name of the element.
	void setName(const QString &newName);
	/// Sets the position of the element.
	void setPosition(double newPosition);

protected:
	/// The name of the wheel element.
	QString name_;
	/// The position of this element.
	double position_;
};

/// This class encapsulates a reference wheel and provides a means of interacting with it using it's concept rather than the raw control.
class CLSStandardsWheel : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Requires a name and the base PV name for the control.
	explicit CLSStandardsWheel(const QString &name, const QString &basePVName, QObject *parent = 0);
	/// Destructor.
	~CLSStandardsWheel();

	/// Returns the name of the element at \param index.
	const QString &name(int index) const { return wheelElements_.at(index)->name(); }

	/// Returns the list of elements.
	QList<CLSStandardsWheelElement *> wheelElements() const { return wheelElements_; }
	/// Returns the element at the given index.
	CLSStandardsWheelElement *wheelElementAt(int index) const { return wheelElements_.at(index); }

signals:

public slots:
	/// Sets the name of a given element.
	void setName(int index, const QString &newName);

	/// Moves the wheel to the specified index.
	void moveToIndex(int index);

protected:
	/// The control that moves the motor.
	CLSMAXvMotor *wheel_;

	/// The list of elements for this wheel.
	QList<CLSStandardsWheelElement *> wheelElements_;
};

#endif // CLSSTANDARDSWHEEL_H
