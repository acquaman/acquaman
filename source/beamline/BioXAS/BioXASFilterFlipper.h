#ifndef BIOXASFILTERFLIPPER_H
#define BIOXASFILTERFLIPPER_H

#include <QSignalMapper>

#include "beamline/AMPVControl.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASFilterFlipperSlide.h"

class BioXASFilterFlipper : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Enum providing different flipper statuses.
	enum Status { Done = 0, Changing, Error };
	/// Enum providing different operating modes.
	enum Mode { Auto = 0, User };

	/// Constructor.
    explicit BioXASFilterFlipper(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipper();

	/// Returns true if this component is connected.
	virtual bool isConnected() const;

	/// Returns the status control.
	AMReadOnlyPVControl* statusControl() const { return statusControl_; }
	/// Returns the run mode control.
	AMPVControl* runModeControl() const { return runModeControl_; }
	/// Returns the 'go to next step' control, used when in User Mode.
	AMPVControl* goToNextStepControl() const { return goToNextStepControl_; }
	/// Returns the current slide control.
	AMPVControl* currentSlideControl() const { return currentSlideControl_; }
	/// Returns the current slide status control--whether there is a current slide in the receiver.
	AMReadOnlyPVControl *currentSlideStatusControl() const { return currentSlideStatusControl_; }

	/// Returns the list of slides.
	QList<BioXASFilterFlipperSlide*> slides() const { return slides_; }

	/// Returns the current slide.
	BioXASFilterFlipperSlide* currentSlide() const { return currentSlide_; }
	/// Returns the slides_ index of the current slide. Returns -1 if there is no current slide.
	int currentSlideIndex() const { return slides_.indexOf(currentSlide_); }

	/// Returns the slide with the given filter.
	BioXASFilterFlipperSlide* slideWithFilter(BioXASFilterFlipperFilter *filter) const;

	/// Returns a string representation of the given status.
	QString statusToString(int status) const;
	/// Returns a string representation of the given mode.
	QString modeToString(int mode) const;

signals:
	/// Notifier that the current slide has changed.
	void currentSlideChanged(BioXASFilterFlipperSlide *newSlide);

public slots:
	/// Sets the filter for the slide at the given index, a new filter with the given characteristics.
	void setSlideFilter(int index, AMElement *element, double thickness);

protected slots:
	/// Sets the current slide.
	void setCurrentSlide(BioXASFilterFlipperSlide *currentSlide);
	/// Sets the filter for the slide at the given index, deletes any previous filters.
	void setSlideFilter(int index, BioXASFilterFlipperFilter *newFilter);

	/// Updates the current slide to correspond to the latest current slide control value.
	void updateCurrentSlide();

	/// Handles removing the current slide if it is disabled.
	void onCurrentSlideEnabledChanged();

protected:
	/// Creates and returns a list of the given number of slides.
	QList<BioXASFilterFlipperSlide*> createSlides(int slideCount);
	/// Returns true if the slides are connected, false otherwise.
	bool slidesConnected() const;

	/// Creates and returns a filter with the given characteristics.
	BioXASFilterFlipperFilter* createFilter(AMElement *element, double thickness);

protected:
	/// The status control.
	AMReadOnlyPVControl *statusControl_;

	/// The run mode control.
	AMSinglePVControl *runModeControl_;
	/// The 'go to next step' control, used when in User Mode.
	AMSinglePVControl *goToNextStepControl_;

	/// The current slide control.
	AMPVControl *currentSlideControl_;
	/// The current slide status control--whether there is a current slide in the receiver.
	AMReadOnlyPVControl *currentSlideStatusControl_;

	/// The current slide.
	BioXASFilterFlipperSlide *currentSlide_;

	/// The list of filters.
	QList<BioXASFilterFlipperSlide*> slides_;
};

#endif // BIOXASFILTERFLIPPER_H
