#ifndef BIOXASXIAFILTERSVIEW_H
#define BIOXASXIAFILTERSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "beamline/BioXAS/BioXASXIAFilters.h"

class AMExtendedControlEditor;

class BioXASXIAFiltersView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXIAFiltersView(BioXASXIAFilters *filters, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFiltersView();

	/// Returns the XIA filters being viewed.
	BioXASXIAFilters* filters() const { return filters_; }

signals:
	/// Notifier that the filters being viewed have changed.
	void filtersChanged(BioXASXIAFilters *newFilters);

public slots:
	/// Sets the filters being viewed.
	void setFilters(BioXASXIAFilters *newFilters);

protected:
	/// The filters being viewed.
	BioXASXIAFilters *filters_;

	/// The editor for the first filter.
	AMExtendedControlEditor *editor1_;
	/// The editor for the second filter.
	AMExtendedControlEditor *editor2_;
	/// The editor for the third filter.
	AMExtendedControlEditor *editor3_;
	/// The editor for the fourth filter.
	AMExtendedControlEditor *editor4_;

};

#endif // BIOXASXIAFILTERSVIEW_H
