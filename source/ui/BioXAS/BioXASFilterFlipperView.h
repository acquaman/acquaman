#ifndef BIOXASFILTERFLIPPERVIEW_H
#define BIOXASFILTERFLIPPERVIEW_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QLayout>

class BioXASFilterFlipper;
class BioXASFilterFlipperFiltersConfigurationView;
class AMExtendedControlEditor;

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
	/// Refreshes the view.
	void refresh();

	/// Sets the filter flipper being viewed.
	void setFilterFlipper(BioXASFilterFlipper *newFlipper);

protected slots:
	/// Clears the view.
	void clear();

	/// Updates the slides editor.
	void updateSlidesEditor();
	/// Updates the filters editor.
	void updateFiltersEditor();
	/// Updates the slide change status editor.
	void updateChangeStatusEditor();
	/// Updates the run mode editor.
	void updateRunModeEditor();
	/// Updates the filter configuration view.
	void updateFilterConfigurationView();

protected:
	/// The filter flipper being viewed.
	BioXASFilterFlipper *filterFlipper_;

	/// The slides editor.
	AMExtendedControlEditor *slidesEditor_;
	/// The filters editor.
	AMExtendedControlEditor *filtersEditor_;
	/// The slide change status editor.
	AMExtendedControlEditor *changeStatusEditor_;
	/// The run mode editor.
	AMExtendedControlEditor *runModeEditor_;

	/// The configuration view.
	BioXASFilterFlipperFiltersConfigurationView *configurationView_;
};

#endif // BIOXASFILTERFLIPPERVIEW_H
