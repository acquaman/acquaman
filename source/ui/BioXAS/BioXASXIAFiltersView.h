#ifndef BIOXASXIAFILTERSVIEW_H
#define BIOXASXIAFILTERSVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "beamline/BioXAS/BioXASXIAFilters.h"

class BioXASXIAFiltersView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASXIAFiltersView(BioXASXIAFilters *filters, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXIAFiltersView();

signals:
	/// Notifier that the filters being viewed have changed.
	void filtersChanged(BioXASXIAFilters *newFilters);

public slots:
	/// Sets the filters being viewed.
	void setFilters(BioXASXIAFilters *newFilters);

protected:
	/// The filters being viewed.
	BioXASXIAFilters *filters_;

};

#endif // BIOXASXIAFILTERSVIEW_H
