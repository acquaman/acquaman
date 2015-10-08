#ifndef BIOXASFILTERFLIPPER_H
#define BIOXASFILTERFLIPPER_H

#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASFilterFlipperSlide.h"

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

	/// Returns the list of slides.
	QList<BioXASFilterFlipperSlide*> slides() const { return slides_; }

	/// Returns a string representation of the given status.
	QString statusToString(int status) const;

public slots:
	/// Sets the filter for the slide at the given index, a new filter with the given characteristics.
	void setSlideFilter(int index, AMElement *element, double thickness);

protected slots:
	/// Sets the filter for the slide at the given index, deletes any previous filters.
	void setSlideFilter(int index, BioXASFilterFlipperFilter *newFilter);

protected:
	/// Creates and returns a list of the given number of slides.
	QList<BioXASFilterFlipperSlide*> createSlides(int slideCount);
	/// Returns true if the slides are connected, false otherwise.
	bool slidesConnected() const;

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
	QList<BioXASFilterFlipperSlide*> slides_;
};

#endif // BIOXASFILTERFLIPPER_H
