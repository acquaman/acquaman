#ifndef SGMBEAMONCONTROL_H
#define SGMBEAMONCONTROL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
class AMControl;
class SGMBeamOnControl : public QWidget
{
    Q_OBJECT
public:
    explicit SGMBeamOnControl(AMControl* beamStatusControl,
			      AMControl* beamOnControl,
			      AMControl* beamOffControl,
			      QWidget *parent = 0);

signals:

public slots:
protected slots:
	void onRequiredControlsConnectionChanged(bool isConnected);

	void onBeamOnButtonPushed();

	void onBeamOffButtonPushed();

	void onBeamStatusPVChanged();
protected:
	void setupUi();

	void setupData();

	void setupConnections();

	QString beamStatusText();

	QLabel* beamStatusLabel_;
	QPushButton* beamOnButton_;
	QPushButton* beamOffButton_;

	AMControl* beamStatusControl_;
	AMControl* beamOnControl_;
	AMControl* beamOffControl_;

	bool connectedOnce_;
};

#endif // SGMBEAMONCONTROL_H
