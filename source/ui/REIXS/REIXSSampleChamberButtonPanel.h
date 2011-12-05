#ifndef REIXSSAMPLECHAMBERBUTTONPANEL_H
#define REIXSSAMPLECHAMBERBUTTONPANEL_H

#include <QWidget>

namespace Ui {
    class REIXSSampleChamberButtonPanel;
}

class REIXSSampleChamberButtonPanel : public QWidget
{
    Q_OBJECT

public:
    explicit REIXSSampleChamberButtonPanel(QWidget *parent = 0);
    ~REIXSSampleChamberButtonPanel();

protected slots:
	void onStopButtonClicked();

private:
    Ui::REIXSSampleChamberButtonPanel *ui;
};

#endif // REIXSSAMPLECHAMBERBUTTONPANEL_H
