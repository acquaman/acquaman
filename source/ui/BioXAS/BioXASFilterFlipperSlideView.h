#ifndef BIOXASFILTERFLIPPERSLIDEVIEW_H
#define BIOXASFILTERFLIPPERSLIDEVIEW_H

#include <QWidget>
#include <QCheckBox>

#include "beamline/BioXAS/BioXASFilterFlipperSlide.h"
#include "ui/BioXAS/BioXASFilterFlipperFilterView.h"

class BioXASFilterFlipperSlideView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASFilterFlipperSlideView(BioXASFilterFlipperSlide *slide, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASFilterFlipperSlideView();

	/// Returns the slide being viewed.
	BioXASFilterFlipperSlide* slide() const { return slide_; }

signals:
	/// Notifier that the slide being viewed has changed.
	void slideChanged(BioXASFilterFlipperSlide *newSlide);

public slots:
	/// Clears the view.
	void clear();
	/// Updates the view.
	void update();
	/// Refreshes the view.
	void refresh();

	/// Sets the slide being viewed.
	void setSlide(BioXASFilterFlipperSlide *newSlide);

protected slots:
	/// Updates the filter view.
	void updateFilterView();
	/// Updates the enabled box.
	void updateEnabledBox();

	/// Updates the slide's enabled state with the user's selection.
	void onEnabledBoxClicked();

protected:
	/// The slide being viewed.
	BioXASFilterFlipperSlide *slide_;

	/// The filter view.
	BioXASFilterFlipperFilterView *filterView_;
	/// The enabled box.
	QCheckBox *enabledBox_;
};

#endif // BIOXASFILTERFLIPPERSLIDEVIEW_H
