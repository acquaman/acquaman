#ifndef AMACTIONINFODESCRIPTIONVIEW_H
#define AMACTIONINFODESCRIPTIONVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>

#include "actions3/AMActionInfo3.h"

class AMActionInfoDescriptionView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMActionInfoDescriptionView(AMActionInfo3 *info, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMActionInfoDescriptionView();

	/// Returns the action info being viewed.
	AMActionInfo3* info() const { return info_; }

signals:
	/// Notifier that the action info being viewed has changed.
	void infoChanged(AMActionInfo3 *newInfo);

public slots:
	/// Clears the view.
	void clear();
	/// Refreshes the view.
	void refresh();

	/// Sets the action info being viewed.
	void setInfo(AMActionInfo3 *newInfo);

protected slots:
	/// Updates the description label.
	void updateDescriptionLabel();

protected:
	/// The action info being viewed.
	AMActionInfo3 *info_;

	/// The description label.
	QLabel *descriptionLabel_;
};

#endif // AMACTIONINFODESCRIPTIONVIEW_H
