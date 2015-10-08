#ifndef BIOXASFILTERFLIPPERCONFIGURATIONVIEW_H
#define BIOXASFILTERFLIPPERCONFIGURATIONVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASFilterFlipper.h"
#include "ui/BioXAS/BioXASFilterFlipperSlideView.h"

class BioXASFilterFlipperConfigurationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipperConfigurationView(BioXASFilterFlipper *filterFlipper, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipperConfigurationView();

	/// Returns the filter flipper being viewed.
	BioXASFilterFlipper* filterFlipper() const { return filterFlipper_; }

signals:
	/// Notifier that the filter flipper being viewed has changed.
	void filterFlipperChanged(BioXASFilterFlipper *newFlipper);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the filter flipper being viewed.
	void setFilterFlipper(BioXASFilterFlipper *newFlipper);

protected:
	/// Creates and returns a new slide view for the given slide.
	BioXASFilterFlipperSlideView* createSlideView(BioXASFilterFlipperSlide *slide);
	/// Creates and returns a new slide view for the given slide, includes a slide index label.
	QWidget* createIndexedSlideView(int index, BioXASFilterFlipperSlideView *slideView);

protected:
	/// The filter flipper being viewed.
	BioXASFilterFlipper *filterFlipper_;

	/// The main layout.
	QVBoxLayout *layout_;
	/// The indexed slide view <---> slide view mapping.
	QMap<QWidget*, BioXASFilterFlipperSlideView*> indexedSlideViewMapping_;
};

#endif // BIOXASFILTERFLIPPERCONFIGURATIONVIEW_H
