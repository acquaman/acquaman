#ifndef AMIMAGEPROPERTYEDITOR_H
#define AMIMAGEPROPERTYEDITOR_H

#include <QWidget>

namespace Ui {
	class AMImagePropertyEditor;
}

class AMImagePropertyEditor : public QWidget
{
	Q_OBJECT

public:
	explicit AMImagePropertyEditor(QWidget *parent = 0);
	~AMImagePropertyEditor();

protected:
	Ui::AMImagePropertyEditor* ui_;
};

#endif // AMIMAGEPROPERTYEDITOR_H
