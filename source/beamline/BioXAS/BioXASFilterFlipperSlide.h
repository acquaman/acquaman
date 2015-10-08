#ifndef BIOXASFILTERFLIPPERSLIDE_H
#define BIOXASFILTERFLIPPERSLIDE_H

#include "beamline/AMControl.h"
#include "beamline/BioXAS/BioXASBeamlineComponent.h"
#include "beamline/BioXAS/BioXASFilterFlipperFilter.h"

class BioXASFilterFlipperSlide : public BioXASBeamlineComponent
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipperSlide(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipperSlide();

	/// Returns true if this component is connected, false otherwise.
	virtual bool isConnected() const;

	/// Returns the slide's filter.
	BioXASFilterFlipperFilter* filter() const { return filter_; }
	/// Returns whether the slide has a filter.
	bool hasFilter() const { return (filter_ != 0); }
	/// Returns whether the slide is enabled.
	bool enabled() const { return enabled_; }
	/// Returns the slide's cartridge status control.
	AMControl* cartridgeStatusControl() const { return cartridgeStatus_; }

signals:
	/// Notifier that the slide's filter has changed.
	void filterChanged(BioXASFilterFlipperFilter *newFilter);
	/// Notifier that the slide's enabled state has changed.
	void enabledChanged(bool enabled);
	/// Notifier that the slide's cartridge status control has changed.
	void cartridgeStatusControlChanged(AMControl *statusControl);

public slots:
	/// Sets the slide's filter.
	void setFilter(BioXASFilterFlipperFilter *newFilter);
	/// Sets the slide's enabled state.
	void setEnabled(bool isEnabled);
	/// Sets the slide's cartridge status control.
	void setCartridgeStatusControl(AMControl *newControl);

protected:
	/// The slide's filter.
	BioXASFilterFlipperFilter *filter_;
	/// The slide's enabled state.
	bool enabled_;
	/// The slide's cartridge status control.
	AMControl *cartridgeStatus_;
};

#endif // BIOXASFILTERFLIPPERSLIDE_H
