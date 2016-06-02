#ifndef BIOXASFILTERFLIPPERVIEW_H
#define BIOXASFILTERFLIPPERVIEW_H

#include <QWidget>
#include <QLayout>

class BioXASFilterFlipper;
class BioXASFilterFlipperFiltersConfigurationView;
class CLSControlEditor;

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

	/// Updates the filters editor.
	void updateFiltersEditor();
	/// Updates the slide change status editor.
	void updateChangeStatusEditor();
	/// Updates the filter configuration view.
	void updateFilterConfigurationView();

	/// Refreshes the filters editor.
	void refreshFiltersEditor();

protected:
	/// The filter flipper being viewed.
	BioXASFilterFlipper *filterFlipper_;

	/// The filters editor.
	CLSControlEditor *filtersEditor_;
	/// The slide change status editor.
	CLSControlEditor *changeStatusEditor_;

	/// The configuration view.
	BioXASFilterFlipperFiltersConfigurationView *configurationView_;
};

#endif // BIOXASFILTERFLIPPERVIEW_H
