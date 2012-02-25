#ifndef REIXSSIDEBAR_H
#define REIXSSIDEBAR_H

#include <QWidget>

namespace Ui {
    class REIXSSidebar;
}

class REIXSActionBasedControlEditor;
class AMBasicEnumControlEditor;

/// This widget displays the real-time "at a glance" REIXS beamline controls / feedback monitors on the side of the application's main window.
class REIXSSidebar : public QWidget
{
    Q_OBJECT

public:
    explicit REIXSSidebar(QWidget *parent = 0);
    ~REIXSSidebar();

private:
    Ui::REIXSSidebar *ui;

	// Additional UI controls
	REIXSActionBasedControlEditor* beamlineEnergyEditor_, *monoSlitEditor_;
	AMBasicEnumControlEditor* gratingSelector_, *mirrorSelector_;

protected slots:
	// Responding to GUI events
	////////////////////////////

	void onMCPCountsPerSecondChanged(double countsPerSecond);
};

#endif // REIXSSIDEBAR_H
