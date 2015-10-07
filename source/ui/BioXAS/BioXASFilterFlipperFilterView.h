#ifndef BIOXASFILTERFLIPPERFILTERVIEW_H
#define BIOXASFILTERFLIPPERFILTERVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QSpinBox>
#include <QLayout>
#include <QLabel>

#include "beamline/BioXAS/BioXASFilterFlipperFilter.h"

class BioXASFilterFlipperFilterView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipperFilterView(BioXASFilterFlipperFilter *filter, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipperFilterView();

	/// Returns the filter being viewed.
	BioXASFilterFlipperFilter* filter() const { return filter_; }

signals:
	/// Notifier that the filter being viewed has changed.
	void filterChanged(BioXASFilterFlipperFilter *newFilter);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the filter being viewed.
	void setFilter(BioXASFilterFlipperFilter *newFilter);

protected slots:
	/// Updates the default label.
	void updateDefaultLabel();
	/// Updates the element button.
	void updateElementButton();
	/// Updates the thickness box.
	void updateThicknessBox();

protected:
	/// The filter being viewed.
	BioXASFilterFlipperFilter *filter_;

	/// The label displayed by default, when no filter is set.
	QLabel *defaultLabel_;
	/// The element button.
	QToolButton *elementButton_;
	/// The thickness spinbox.
	QSpinBox *thicknessBox_;
};

#endif // BIOXASFILTERFLIPPERFILTERVIEW_H
