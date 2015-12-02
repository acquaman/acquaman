#ifndef SGMVATVALVEVIEW_H
#define SGMVATVALVEVIEW_H

#include <QWidget>
#include <QLayout>

#include "source/beamline/SGM/SGMVATValve.h"
#include "source/ui/beamline/AMExtendedControlEditor.h"

class SGMVATValveView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMVATValveView(SGMVATValve *valve, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMVATValveView();

signals:
	/// Notifier that the valve being viewed has changed.
	void valveChanged(SGMVATValve *newControl);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the VAT valve being viewed.
	void setValve(SGMVATValve *newControl);

protected:
	/// The VAT valve being viewed.
	SGMVATValve *valve_;

	/// The VAT valve status editor.
	AMExtendedControlEditor *statusEditor_;
	/// The VAT valve position editor.
	AMExtendedControlEditor *positionEditor_;
	/// The VAT valve speed editor.
	AMExtendedControlEditor *speedEditor_;
};

#endif // SGMVATVALVEVIEW_H
