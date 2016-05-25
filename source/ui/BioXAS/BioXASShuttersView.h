#ifndef BIOXASSHUTTERSVIEW_H
#define BIOXASSHUTTERSVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLayout>

#include "beamline/BioXAS/BioXASShutters.h"

class CLSControlEditor;

class BioXASShuttersView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASShuttersView(BioXASShutters *shutters, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASShuttersView();

	/// Returns the shutters being viewed.
	BioXASShutters* control() const { return shutters_; }

signals:
	/// Notifier that the shutters being viewed have changed.
	void controlChanged(BioXASShutters *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the shutters being viewed.
	void setControl(BioXASShutters *newControl);
	/// Sets the main editor title.
	void setTitle(const QString &newTitle);

protected slots:
	/// Updates the shutters editor.
	void updateShuttersEditor();
	/// Updates the child views.
	void updateChildViews();

protected:
	/// The shutters control being viewed.
	BioXASShutters *shutters_;

	/// The main shutters editor.
	CLSControlEditor *shuttersEditor_;
	/// The shutters child controls views.
	QGroupBox *childViews_;
	/// The shutters child controls views layout.
	QVBoxLayout *childViewsLayout_;
};

#endif // BIOXASSHUTTERSVIEW_H
