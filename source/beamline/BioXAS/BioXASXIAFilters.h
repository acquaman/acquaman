#ifndef BIOXASXIAFILTERS_H
#define BIOXASXIAFILTERS_H

#include <QObject>
#include <QSignalMapper>

#include "beamline/CLS/CLSBiStateControl.h"

// Setpoints.
#define BIOXAS_XIA_FILTERS_POSITION_OUT -50
#define BIOXAS_XIA_FILTERS_POSITION_IN 50
#define BIOXAS_XIA_FILTERS_POSITION_TOLERANCE 0.1

// Error codes.
#define BIOXAS_XIA_FILTERS_NOT_CONNECTED 29348721
#define BIOXAS_XIA_FILTERS_ALREADY_MOVING 29348722
#define BIOXAS_XIA_FILTERS_INVALID_SETPOINT 29348723
#define BIOXAS_XIA_FILTERS_MOVE_FAILED 29348724

class BioXASXIAFilters : public QObject
{
    Q_OBJECT

public:
	/// Enum class for different filter properties.
	class Filter { public: enum Position { Out = 0, In }; };

	/// Constructor.
    explicit BioXASXIAFilters(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFilters();

	/// Returns true if the filters are connected, false otherwise.
	bool isConnected() const { return connected_; }

	/// Returns the first filter control.
	AMControl* filter1() const { return filter1_; }
	/// Returns the second filter control.
	AMControl* filter2() const { return filter2_; }
	/// Returns the third filter control.
	AMControl* filter3() const { return filter3_; }
	/// Returns the fourth filter control.
	AMControl* filter4() const { return filter4_; }

signals:
	/// Notifier that the current connection state has changed.
	void connectedChanged(bool isConnected);

protected slots:
	/// Sets the current connection state.
	void setConnected(bool isConnected);

	/// Handles updating the current connection state.
	void onConnectedChanged();

protected:
	/// The current connection state.
	bool connected_;

	/// Controls for the first filter.
	CLSBiStateControl *filter1_;
	/// Control for the second filter.
	CLSBiStateControl *filter2_;
	/// Control for the third filter.
	CLSBiStateControl *filter3_;
	/// Control for the fourth filter.
	CLSBiStateControl *filter4_;

};

#endif // BIOXASXIAFILTERS_H