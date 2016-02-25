#ifndef BIOXASBEAMLINECONFIGURATIONVIEW_H
#define BIOXASBEAMLINECONFIGURATIONVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLayout>

class BioXASBeamlineConfigurationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASBeamlineConfigurationView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamlineConfigurationView();

signals:

public slots:

protected slots:

protected:
	/// The button group for the optional detectors.
	QButtonGroup *optionalDetectorsButtonGroup_;
};

#endif // BIOXASBEAMLINECONFIGURATIONVIEW_H
