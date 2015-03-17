#ifndef BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
#define BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H

#include <QWidget>
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

protected:
	/// The mono being viewed.
	BioXASSSRLMonochromator *mono_;

};

#endif // BIOXASSSRLMONOCHROMATORCONFIGURATIONVIEW_H
