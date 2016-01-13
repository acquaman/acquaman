#ifndef BIOXASFILTERFLIPPERFILTERSCONFIGURATIONVIEW_H
#define BIOXASFILTERFLIPPERFILTERSCONFIGURATIONVIEW_H

#include <QLabel>
#include <QFormLayout>

class BioXASFilterFlipperFilters;

class BioXASFilterFlipperFiltersConfigurationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipperFiltersConfigurationView(BioXASFilterFlipperFilters *filters, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipperFiltersConfigurationView();

	/// Returns the filter flipper being viewed.
	BioXASFilterFlipperFilters* filters() const { return filters_; }

signals:
	/// Notifier that the filter flipper being viewed has changed.
	void filtersChanged(BioXASFilterFlipperFilters *newFlipper);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the filters being viewed.
	void setFilters(BioXASFilterFlipperFilters *newFilters);

protected slots:
	/// Clears the view.
	void clear();

protected:
	/// The filters being viewed.
	BioXASFilterFlipperFilters *filters_;

	/// The main layout.
	QFormLayout *layout_;
	/// The list of filter views.
	QList<QWidget*> filterViews_;
};

#endif // BIOXASFILTERFLIPPERFILTERSCONFIGURATIONVIEW_H
