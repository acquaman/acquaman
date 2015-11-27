#ifndef BIOXASFILTERFLIPPERCONFIGURATIONVIEW_H
#define BIOXASFILTERFLIPPERCONFIGURATIONVIEW_H

#include <QLabel>
#include <QFormLayout>

#include "beamline/BioXAS/BioXASFilterFlipper.h"
#include "ui/BioXAS/BioXASFilterFlipperFilterView.h"

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
	/// Refreshes the view.
	void refresh();

	/// Sets the filter flipper being viewed.
	void setFilterFlipper(BioXASFilterFlipper *newFlipper);

protected:

protected:
	/// The filter flipper being viewed.
	BioXASFilterFlipper *filterFlipper_;

	/// The main layout.
	QFormLayout *layout_;
	/// The list of filter views.
	QList<QWidget*> filterViews_;
};

#endif // BIOXASFILTERFLIPPERCONFIGURATIONVIEW_H
