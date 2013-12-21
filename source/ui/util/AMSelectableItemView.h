#ifndef AMSELECTABLEITEMVIEW_H
#define AMSELECTABLEITEMVIEW_H

#include <QWidget>

#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>

/// Base class for a selectable item (either an emission line or absorption edge).  Provides a simple base class for simple items that are selectable.
class AMSelectableItemView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds the typical view for the selectable item.
 	virtual ~AMSelectableItemView();
	explicit AMSelectableItemView(QWidget *parent = 0);
	/// Constructor.  Passes in the text and default checked state of the view.
	AMSelectableItemView(const QString &text, bool checked = false, QWidget *parent = 0);

	/// Returns the current state of the check box used for selecting.
	bool isItemSelected() const { return checkBox_->isChecked(); }

signals:
	/// Notifier that the item has been selected or not.  Passes the state.
	void itemSelected(bool);

public slots:
	/// Sets the text for the name label.
	void setText(const QString &text);
	/// Sets the state of the check box.
	void setSelected(bool isSelected);

protected:
	/// The check box used for selecting the item.
	QCheckBox *checkBox_;
	/// The layout holding all the widgets.
	QHBoxLayout *itemLayout_;
};

#endif // AMSELECTABLEITEMVIEW_H
