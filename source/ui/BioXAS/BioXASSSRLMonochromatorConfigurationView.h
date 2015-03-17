#ifndef BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLayout>

#include "beamline/BioXAS/BioXASSSRLMonochromator.h"

class BioXASSSRLMonochromatorConfigurationView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASSSRLMonochromatorConfigurationView(BioXASSSRLMonochromator *mono, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASSSRLMonochromatorConfigurationView();

signals:
	/// Notifier that the mono being viewed has changed.
	void monoChanged(BioXASSSRLMonochromator *newMono);

public slots:
	/// Sets the mono being viewed.
	void setMono(BioXASSSRLMonochromator *newMono);

protected slots:
	/// Updates the view when the bragg motor offset has changed.
	void onBraggMotorOffsetChanged();
	/// Updates the view when the bragg encoder move type has changed.
	void onBraggMotorEncoderMoveTypeChanged();
	/// Updates the view when the bragg motor settling time has changed.
	void onBraggMotorSettlingTimeChanged();

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

	/// Display for the bragg motor degree offset.
	QDoubleSpinBox *braggOffset_;
	/// Display for the bragg motor encoder move type.
	QComboBox *braggEncoderMoveType_;
	/// Display for the bragg motor move settling time.
	QDoubleSpinBox *braggSettlingTime_;

};

#endif // BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
