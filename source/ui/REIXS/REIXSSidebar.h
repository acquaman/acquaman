#ifndef REIXSSIDEBAR_H
#define REIXSSIDEBAR_H

#include <QWidget>

namespace Ui {
    class REIXSSidebar;
}

class REIXSActionBasedControlEditor;

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
};

#endif // REIXSSIDEBAR_H
