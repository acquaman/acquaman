#ifndef BIOXASFILTERFLIPPER_H
#define BIOXASFILTERFLIPPER_H

#include <QSignalMapper>

#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASFilterFlipperControl.h"

class BioXASFilterFlipper : public BioXASFilterFlipperControl
{
    Q_OBJECT

public:
	/// Enum providing different flipper statuses.
	enum MoveStatus { Done = 0, Changing, Error };
	/// Enum providing different operating modes.
	enum Mode { Auto = 0, User };

	/// Constructor.
	explicit BioXASFilterFlipper(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipper();

	/// Returns true if this component is connected.
	virtual bool isConnected() const;

	/// Returns the currently loaded slide.
	AMPVControl* currentSlide() const { return currentSlide_; }
	/// Returns the slide change status: Done, Changing, Error.
	AMReadOnlyPVControl* slideChangeStatus() const { return slideChangeStatus_; }
	/// Returns the run mode: Auto, User. Should be 'Auto' for normal operations.
	AMPVControl* runMode() const { return runMode_; }
	/// Returns the control that triggers proceeding to the next step of a slide change. Used in 'User' mode.
	AMSinglePVControl* nextStepTrigger() const { return nextStepTrigger_; }
	/// Returns the control that reports whether there is a slide currently in the receiver.
	AMReadOnlyPVControl* slideReceiverStatus() const { return slideReceiverStatus_; }
	/// Returns the receiver stage CW limit status.
	AMReadOnlyPVControl* receiverStageCWLimit() const { return receiverStageCWLimit_; }
	/// Returns the receiver stage CCW limit status.
	AMReadOnlyPVControl* receiverStageCCWLimit() const { return receiverStageCCWLimit_; }
	/// Returns the cartridge stage CW limit status.
	AMReadOnlyPVControl* cartridgeStageCWLimit() const { return cartridgeStageCWLimit_; }
	/// Returns the cartridge stage CCW limit status.
	AMReadOnlyPVControl* cartridgeStageCCWLimit() const { return cartridgeStageCCWLimit_; }

	/// Returns the cartridge status for the given slide index. Returns 0 for invalid slide index.
	AMReadOnlyPVControl* cartridgeStatusForSlide(int slideIndex);
	/// Returns the filter for the given slide index. Returns 0 for invalid slide index.
	BioXASFilterFlipperFilter* filterForSlide(int slideIndex);

	/// Returns a string representation of the given status.
	QString statusToString(int status) const;
	/// Returns a string representation of the given mode.
	QString modeToString(int mode) const;

signals:
	/// Notifier that the filter for the given slide index has changed.
	void slideFilterChanged(int index);

public slots:
	/// Sets the filter for the slide at the given index, a new filter with the given characteristics.
	void setSlideFilter(int index, const QString &elementSymbol, double thickness);
	/// Removes the filter for the slide at the given index.
	void removeSlideFilter(int index);

protected slots:
	/// Sets the cartridge slide status for the slide at the given index.
	void setSlideCartridgeStatus(int index, AMReadOnlyPVControl *status);
	/// Removes the cartridge slide status for the slide at the given index.
	void removeSlideCartridgeStatus(int index);

protected:
	/// Creates and returns a filter with the given characteristics.
	BioXASFilterFlipperFilter* createFilter(const QString &elementSymbol, double thickness);

protected:
	/// The currently loaded slide.
	AMPVControl *currentSlide_;
	/// The slide change status: Done, Changing, Error.
	AMReadOnlyPVControl *slideChangeStatus_;
	/// The run mode. Should be 'Auto' for normal operations.
	AMPVControl *runMode_;
	/// The control that triggers proceeding to the next step of a slide change. Used when in 'User' mode.
	AMSinglePVControl *nextStepTrigger_;
	/// The control that reports whether there is a slide currently in the receiver.
	AMReadOnlyPVControl *slideReceiverStatus_;

	/// The receiver stage CW motor limit.
	AMReadOnlyPVControl *receiverStageCWLimit_;
	/// The receiver stage CCW motor limit.
	AMReadOnlyPVControl *receiverStageCCWLimit_;

	/// The cartridge stage CW motor limit.
	AMReadOnlyPVControl *cartridgeStageCWLimit_;
	/// The cartridge stage CCW motor limit.
	AMReadOnlyPVControl *cartridgeStageCCWLimit_;

	/// The slide cartridge status map, maps the slide index to whether that slide is currently in the cartridge.
	QMap<int, AMReadOnlyPVControl*> slideCartridgeStatusMap_;
};

#endif // BIOXASFILTERFLIPPER_H
