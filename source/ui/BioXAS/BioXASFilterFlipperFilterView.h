#ifndef BIOXASFILTERFLIPPERFILTERVIEW_H
#define BIOXASFILTERFLIPPERFILTERVIEW_H

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QLayout>

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
	/// Refreshes the view.
	void refresh();

	/// Sets the filter being viewed.
	void setFilter(BioXASFilterFlipperFilter *newFilter);

protected slots:
	/// Clears the view.
	void clear();

	/// Updates the element line edit.
	void updateElementLineEdit();
	/// Updates the thickness box.
	void updateThicknessBox();

	/// Updates the filter element with the user's input.
	void onElementLineEditChanged();
	/// Updates the filter thickness with the user's selection.
	void onThicknessBoxValueChanged();

protected:
	/// The filter being viewed.
	BioXASFilterFlipperFilter *filter_;

	/// The element line edit.
	QLineEdit *elementLineEdit_;
	/// The thickness spinbox.
	QSpinBox *thicknessBox_;
};

#endif // BIOXASFILTERFLIPPERFILTERVIEW_H
