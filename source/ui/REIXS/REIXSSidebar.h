#ifndef REIXSSIDEBAR_H
#define REIXSSIDEBAR_H

#include <QWidget>

namespace Ui {
    class REIXSSidebar;
}

class AMBasicControlEditor;

class REIXSSidebar : public QWidget
{
    Q_OBJECT

public:
    explicit REIXSSidebar(QWidget *parent = 0);
    ~REIXSSidebar();

private:
    Ui::REIXSSidebar *ui;

	// Additional UI controls
	AMBasicControlEditor* beamlineEnergyEditor_, *monoSlitEditor_;
};

#endif // REIXSSIDEBAR_H
