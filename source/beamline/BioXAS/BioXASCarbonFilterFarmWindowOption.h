#ifndef BIOXASCARBONFILTERFARMWINDOWOPTION_H
#define BIOXASCARBONFILTERFARMWINDOWOPTION_H

#include <QObject>
#include "dataman/AMNumber.h"

class BioXASCarbonFilterFarmWindowOption : public QObject
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASCarbonFilterFarmWindowOption(const QString &name, const AMNumber &setpoint = AMNumber::InvalidError, const AMNumber &minPosition = AMNumber::InvalidError, const AMNumber &maxPosition = AMNumber::InvalidError, const AMNumber &filter = AMNumber::InvalidError, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASCarbonFilterFarmWindowOption();

	/// Returns the current connected state.
	virtual bool isConnected() const { return true; }

	/// Returns the window name.
	QString name() const { return name_; }
	/// Returns the window setpoint position.
	AMNumber setpointPosition() const { return setpointPosition_; }
	/// Returns the window min position.
	AMNumber minPosition() const { return minPosition_; }
	/// Returns the window max position.
	AMNumber maxPosition() const { return maxPosition_; }
	/// Returns the window filter thickness.
	AMNumber filter() const { return filter_; }

	/// Returns true if this window is valid: window name, setpoint, min, max, and filter have been set to valid values. Returns false otherwise.
	bool isValid() const;
	/// Returns true if this window is valid and if the given position is contained within the window's range.
	bool containsPosition(double position) const;

signals:
	/// Notifier that the window name has changed.
	void nameChanged(const QString &newName);
	/// Notifier that the window setpoint position has changed.
	void setpointPositionChanged(const AMNumber &newPosition);
	/// Notifier that the window min position has changed.
	void minPositionChanged(const AMNumber &newPosition);
	/// Notifier that the window max position has changed.
	void maxPositionChanged(const AMNumber &newPosition);
	/// Notifier that the window filter has changed.
	void filterChanged(const AMNumber &newFilter);

public slots:
	/// Sets the window name.
	void setName(const QString &newName);
	/// Sets the window setpoint position.
	void setSetpointPosition(const AMNumber &newPosition);
	/// Sets the window min position.
	void setMinPosition(const AMNumber &newPosition);
	/// Sets the window max position.
	void setMaxPosition(const AMNumber &newPosition);
	/// Sets the window filter.
	void setFilter(const AMNumber &newFilter);

protected:
	/// The window name.
	QString name_;
	/// The window setpoint position.
	AMNumber setpointPosition_;
	/// The window min position.
	AMNumber minPosition_;
	/// The window max position.
	AMNumber maxPosition_;
	/// The window filter.
	AMNumber filter_;
};

#endif // BIOXASCARBONFILTERFARMWINDOWOPTION_H
