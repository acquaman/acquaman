#ifndef BIOXASFILTERFLIPPERVIEW_H
#define BIOXASFILTERFLIPPERVIEW_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QLayout>

#include "beamline/BioXAS/BioXASFilterFlipper.h"
#include "ui/BioXAS/BioXASFilterFlipperConfigurationView.h"

class BioXASFilterFlipperView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipperView(BioXASFilterFlipper *filterFlipper, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipperView();

	/// Returns the filter flipper being viewed.
	BioXASFilterFlipper* filterFlipper() const { return filterFlipper_; }

signals:
	/// Notifier that the filter flipper being viewed has changed.
	void filterFlipperChanged(BioXASFilterFlipper *newFlipper);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the filter flipper being viewed.
	void setFilterFlipper(BioXASFilterFlipper *newFlipper);

protected slots:
	/// Updates the filter configuration view.
	void updateFilterConfigurationView();

protected:
	/// The filter flipper being viewed.
	BioXASFilterFlipper *filterFlipper_;

	/// The configuration view.
	BioXASFilterFlipperConfigurationView *configurationView_;
};

#endif // BIOXASFILTERFLIPPERVIEW_H
