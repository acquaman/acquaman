#ifndef BIOXASWIGGLERVIEW_H
#define BIOXASWIGGLERVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASWiggler.h"
#include "ui/CLS/CLSControlEditor.h"

class BioXASWigglerView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASWigglerView(BioXASWiggler *wiggler, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASWigglerView();

	/// Returns the wiggler being viewed.
	BioXASWiggler* wiggler() const { return wiggler_; }

signals:
	/// Notifier that the wiggler being viewed has changed.
	void wigglerChanged(BioXASWiggler *newWiggler);

public slots:
	/// Sets the wiggler being viewed.
	void setWiggler(BioXASWiggler *newWiggler);

protected slots:
	/// Updates the gap editor.
	void updateGapEditor();
	/// Updates the gap status editor.
	void updateGapStatusEditor();

protected:
	/// The wiggler being viewed.
	BioXASWiggler *wiggler_;

	/// The gap control editor.
	CLSControlEditor *gapEditor_;
	/// The gap status control editor.
	CLSControlEditor *gapStatusEditor_;
};

#endif // BIOXASWIGGLERVIEW_H
