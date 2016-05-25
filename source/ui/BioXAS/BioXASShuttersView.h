#ifndef BIOXASSHUTTERSVIEW_H
#define BIOXASSHUTTERSVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLayout>

#include "beamline/CLS/CLSShutters.h"

class CLSControlEditor;

class BioXASShuttersView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASShuttersView(CLSShutters *shutters, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASShuttersView();

	/// Returns the shutters being viewed.
	CLSShutters* control() const { return shutters_; }

signals:
	/// Notifier that the shutters being viewed have changed.
	void controlChanged(CLSShutters *newControl);

public slots:
	/// Refreshes the view.
	void refresh();

	/// Sets the shutters being viewed.
	void setControl(CLSShutters *newControl);
	/// Sets the main editor title.
	void setTitle(const QString &newTitle);

protected slots:
	/// Updates the shutters editor.
	void updateShuttersEditor();
	/// Updates the child views.
	void updateChildViews();

protected:
	/// The shutters control being viewed.
	CLSShutters *shutters_;

	/// The main shutters editor.
	CLSControlEditor *shuttersEditor_;
	/// The shutters child controls views.
	QGroupBox *childViews_;
	/// The shutters child controls views layout.
	QVBoxLayout *childViewsLayout_;
};

#endif // BIOXASSHUTTERSVIEW_H
