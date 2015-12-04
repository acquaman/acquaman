#ifndef BIOXASFILTERFLIPPER_H
#define BIOXASFILTERFLIPPER_H

#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASFilterFlipperFilters.h"

class BioXASFilterFlipper : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipper(const QString &deviceName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipper();

	/// Returns true if this component is connected.
	virtual bool isConnected() const;

	/// Returns the slides control.
	AMPVControl* slides() const { return slides_; }
	/// Returns the filters control.
	BioXASFilterFlipperFilters* filters() const { return filters_; }
	/// Returns the slide change status: Done, Changing, Error.
	AMReadOnlyPVControl* slideChangeStatus() const { return slideChangeStatus_; }
	/// Returns the run mode: Auto, User. Should be 'Auto' for normal operations.
	AMPVControl* runMode() const { return runMode_; }
	/// Returns the control that triggers proceeding to the next step of a slide change. Used in 'User' mode.
	AMSinglePVControl* nextStepTrigger() const { return nextStepTrigger_; }
	/// Returns the control that reports whether there is a slide currently in the receiver.
	AMReadOnlyPVControl* receiverStatus() const { return receiverStatus_; }
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

signals:
	/// Notifier that the slides control has changed.
	void slidesChanged(AMPVControl *newControl);
	/// Notifier that the filters control has changed.
	void filtersChanged(BioXASFilterFlipperFilters *newControl);
	/// Notifier that the slide change status has changed.
	void slideChangeStatusChanged(AMReadOnlyPVControl *newControl);
	/// Notifier that the run mode control has changed.
	void runModeChanged(AMPVControl *newControl);
	/// Notifier that the control that triggers the next step in a slide change (in USER mode) has changed.
	void nextStepTriggerChanged(AMSinglePVControl *newControl);
	/// Notifier that the slide receiver status control has changed.
	void receiverStatusChanged(AMReadOnlyPVControl *newControl);
	/// Notifier that the slide receiver stage CW limit control has changed.
	void receiverStageCWLimitChanged(AMReadOnlyPVControl *newControl);
	/// Notifier that the slide receiver stage CCW limit control has changed.
	void receiverStageCCWLimitChanged(AMReadOnlyPVControl *newControl);
	/// Notifier that the cartridge stage CW limit control has changed.
	void cartridgeStageCWLimitChanged(AMReadOnlyPVControl *newControl);
	/// Notifier that the cartridge stage CCW limit control has changed.
	void cartridgeStageCCWLimitChanged(AMReadOnlyPVControl *newControl);
	/// Notifier that the slide cartridge status control has changed for the given slide index.
	void slideCartridgeStatusChanged(int index);

public slots:
	/// Sets the slides control.
	void setSlides(AMPVControl *newControl);
	/// Sets the filters control.
	void setFilters(BioXASFilterFlipperFilters *newControl);
	/// Sets the slide change status control.
	void setSlideChangeStatus(AMReadOnlyPVControl *newControl);
	/// Sets the run mode control.
	void setRunMode(AMPVControl *newControl);
	/// Sets the control that triggers the next step in a slide change (in USER mode).
	void setNextStepTrigger(AMSinglePVControl *newControl);
	/// Sets the slide receiver status control.
	void setReceiverStatus(AMReadOnlyPVControl *newControl);
	/// Sets the receiver stage CW limit status.
	void setReceiverStageCWLimit(AMReadOnlyPVControl *newControl);
	/// Sets the receiver stage CCW limit status.
	void setReceiverStageCCWLimit(AMReadOnlyPVControl *newControl);
	/// Sets the cartridge stage CW limit status.
	void setCartridgeStageCWLimit(AMReadOnlyPVControl *newControl);
	/// Sets the cartridge stage CCW limit status.
	void setCartridgeStageCCWLimit(AMReadOnlyPVControl *newControl);

	/// Sets the cartridge slide status for the slide at the given index.
	void setSlideCartridgeStatus(int index, AMReadOnlyPVControl *status);
	/// Removes the cartridge slide status for the slide at the given index.
	void removeSlideCartridgeStatus(int index);

protected slots:
	/// Updates the slides control with the filters control.
	void updateSlides();
	/// Updates the filters control with the slides control.
	void updateFilters();

private:
	/// The slides control.
	AMPVControl *slides_;
	/// The filters control.
	BioXASFilterFlipperFilters *filters_;
	/// The slide change status: Done, Changing, Error.
	AMReadOnlyPVControl *slideChangeStatus_;
	/// The run mode. Should be 'Auto' for normal operations.
	AMPVControl *runMode_;
	/// The control that triggers proceeding to the next step of a slide change. Used when in 'User' mode.
	AMSinglePVControl *nextStepTrigger_;
	/// The control that reports whether there is a slide currently in the receiver.
	AMReadOnlyPVControl *receiverStatus_;
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
