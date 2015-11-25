#ifndef BIOXASFILTERFLIPPERCONTROL_H
#define BIOXASFILTERFLIPPERCONTROL_H

#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASFilterFlipperFilter.h"

class BioXASFilterFlipperControl : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipperControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipperControl();

protected slots:
	/// Sets the filter for the slide at the given index. Returns true if successful (always).
	bool setSlideFilter(int index, BioXASFilterFlipperFilter *filter);
	/// Adds the filter for the slide at the given index. Returns true if successful (always).
	bool addSlideFilter(int index, BioXASFilterFlipperFilter *filter);
	/// Removes the filter for the slide at the given index. Returns true if successful, false otherwise.
	bool removeSlideFilter(int index);

protected:
	/// The slide filter map.
	QMap<int, BioXASFilterFlipperFilter*> slideFilterMap_;
};

#endif // BIOXASFILTERFLIPPERCONTROL_H
