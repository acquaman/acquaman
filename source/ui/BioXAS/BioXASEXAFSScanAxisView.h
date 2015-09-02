#ifndef BIOXASEXAFSSCANAXISVIEW_H
#define BIOXASEXAFSSCANAXISVIEW_H

#include <QWidget>
#include <QLabel>
#include <QButtonGroup>

#include "dataman/AMScanAxis.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"

class BioXASEXAFSScanAxisView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASEXAFSScanAxisView(AMScanAxis *scanAxis, const QString &title = QString(), QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASEXAFSScanAxisView();

	/// Returns the scan axis being viewed.
	AMScanAxis* scanAxis() const { return scanAxis_; }

signals:
	/// Notifier that the scan axis being viewed has changed.
	void scanAxisChanged(AMScanAxis *newAxis);

public slots:
	/// Sets the title.
	void setTitle(const QString &newTitle);
	/// Sets the scan axis being viewed.
	void setScanAxis(AMScanAxis *newAxis);

	/// Clears the view.
	void clear();
	/// Updates the view with the latest scan axis information.
	void update();
	/// Clears the view, then updates it.
	void refresh();

protected slots:
	/// Sets the regions as locked.
	void setRegionsLocked(bool locked);
	/// Sets the main regions view.
	void setRegionsView(QWidget *newRegionsView);

	/// Clears the regions view.
	void clearRegionsView();

	/// Updates the lock regions button.
	void updateLockRegionsButton();
	/// Updates the add region button.
	void updateAddRegionButton();
	/// Updates the regions view.
	void updateRegionsView();

protected:
	/// Creates and returns a view for displaying information about the given list of regions.
	QWidget* createRegionsView(const QList<AMScanAxisEXAFSRegion*> &regions);
	/// Creates and returns a view for displaying information about the given EXAFS region.
	AMEXAFSScanAxisElementView* createEXAFSRegionView(AMScanAxisEXAFSRegion *exafsRegion);

	/// Returns the region view corresponding to the given delete button, 0 if no view found.
	QWidget* regionViewByButton(QAbstractButton *deleteButton);

protected:
	/// The scan axis being viewed.
	AMScanAxis *scanAxis_;
	/// Flag indicating whether the regions are locked.
	bool regionsLocked_;
	/// The list of region views.
	QList<AMEXAFSScanAxisElementView*> regionViews_;
	/// The delete buttons group.
	QButtonGroup *deleteButtonsGroup_;

	/// The widget title.
	QLabel *titleLabel_;
	/// The regions locked button.
	QToolButton *lockRegionsButton_;
	/// The add region button.
	QToolButton *addRegionButton_;
	/// The regions view.
	QWidget *regionsView_;
	/// The regions layout.
	QVBoxLayout *regionsLayout_;
};

#endif // BIOXASEXAFSSCANAXISVIEW_H
