#ifndef BIOXASFILTERFLIPPER_H
#define BIOXASFILTERFLIPPER_H

#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASFilterFlipperFilter.h"

class BioXASFilterFlipper : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Enum providing different flipper statuses.
	enum Status { Done = 0, Changing, Error };

	/// Constructor.
    explicit BioXASFilterFlipper(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipper();

	/// Returns true if this component is connected.
	virtual bool isConnected() const;

	/// Returns the status control.
	AMReadOnlyPVControl* statusControl() const { return status_; }
	/// Returns the run mode control.
	AMPVControl* runModeControl() const { return runMode_; }
	/// Returns the 'go to next step' control, used when in User Mode.
	AMPVControl* goToNextStepControl() const { return goToNextStep_; }
	/// Returns the current slide control.
	AMPVControl* currentSlideControl() const { return currentSlide_; }
	/// Returns the current slide status control--whether there is a current slide in the receiver.
	AMReadOnlyPVControl *currentSlideStatusControl() const { return currentSlideStatus_; }
	/// Returns the 'in cartridge' status control for slide 1.
	AMReadOnlyPVControl *slide1CartridgeStatusControl() const { return slide1CartridgeStatus_; }
	/// Returns the 'in cartridge' status control for slide 2.
	AMReadOnlyPVControl *slide2CartridgeStatus() const { return slide2CartridgeStatus_; }
	/// Returns the 'in cartridge' status control for slide 3.
	AMReadOnlyPVControl *slide3CartridgeStatus() const { return slide3CartridgeStatus_; }
	/// Returns the 'in cartridge' status control for slide 4.
	AMReadOnlyPVControl *slide4CartridgeStatus() const { return slide4CartridgeStatus_; }
	/// Returns the 'in cartridge' status control for slide 5.
	AMReadOnlyPVControl *slide5CartridgeStatus() const { return slide5CartridgeStatus_; }
	/// Returns the 'in cartridge' status control for slide 6.
	AMReadOnlyPVControl *slide6CartridgeStatus() const { return slide6CartridgeStatus_; }
	/// Returns the 'in cartridge' status control for slide 7.
	AMReadOnlyPVControl *slide7CartridgeStatus() const { return slide7CartridgeStatus_; }
	/// Returns the 'in cartridge' status control for slide 8.
	AMReadOnlyPVControl *slide8CartridgeStatus() const { return slide8CartridgeStatus_; }
	/// Returns the 'in cartridge' status control for slide 9.
	AMReadOnlyPVControl *slide9CartridgeStatus() const { return slide9CartridgeStatus_; }
	/// Returns the 'in cartridge' status control for slide 10.
	AMReadOnlyPVControl *slide10CartridgeStatus() const { return slide10CartridgeStatus_; }

	/// Returns the list of filters.
	QList<BioXASFilterFlipperFilter*> filters() const { return filters_; }

	/// Returns a string representation of the given status.
	QString statusToString(int status) const;

signals:
	/// Notifier that the filter at a given index has changed.
	void filterChanged(int index, BioXASFilterFlipperFilter *newFilter);

public slots:
	/// Sets the filter at the given index, a new filter with the given characteristics.
	void setFilter(int index, AMElement *element, double thickness);

protected slots:
	/// Sets the filter at the given index. Deletes the filter being replaced, if the filter flipper is the parent.
	void setFilter(int index, BioXASFilterFlipperFilter *newFilter);

protected:
	/// Creates and returns a filter with the given characteristics.
	BioXASFilterFlipperFilter* createFilter(AMElement *element, double thickness);

protected:
	/// The status control.
	AMReadOnlyPVControl *status_;

	/// The run mode control.
	AMSinglePVControl *runMode_;
	/// The 'go to next step' control, used when in User Mode.
	AMSinglePVControl *goToNextStep_;

	/// The current slide control.
	AMPVControl *currentSlide_;
	/// The current slide status control--whether there is a current slide in the receiver.
	AMReadOnlyPVControl *currentSlideStatus_;

	/// The 'in cartridge' status control for slide 1.
	AMReadOnlyPVControl *slide1CartridgeStatus_;
	/// The 'in cartridge' status control for slide 2.
	AMReadOnlyPVControl *slide2CartridgeStatus_;
	/// The 'in cartridge' status control for slide 3.
	AMReadOnlyPVControl *slide3CartridgeStatus_;
	/// The 'in cartridge' status control for slide 4.
	AMReadOnlyPVControl *slide4CartridgeStatus_;
	/// The 'in cartridge' status control for slide 5.
	AMReadOnlyPVControl *slide5CartridgeStatus_;
	/// The 'in cartridge' status control for slide 6.
	AMReadOnlyPVControl *slide6CartridgeStatus_;
	/// The 'in cartridge' status control for slide 7.
	AMReadOnlyPVControl *slide7CartridgeStatus_;
	/// The 'in cartridge' status control for slide 8.
	AMReadOnlyPVControl *slide8CartridgeStatus_;
	/// The 'in cartridge' status control for slide 9.
	AMReadOnlyPVControl *slide9CartridgeStatus_;
	/// The 'in cartridge' status control for slide 10.
	AMReadOnlyPVControl *slide10CartridgeStatus_;

	/// The list of filters.
	QList<BioXASFilterFlipperFilter*> filters_;
};

#endif // BIOXASFILTERFLIPPER_H
