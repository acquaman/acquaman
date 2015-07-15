#ifndef SGMPERSISTENTVIEW_H
#define SGMPERSISTENTVIEW_H

#include <QWidget>
#include "ui/beamline/AMExtendedControlEditor.h"

class SGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit SGMPersistentView(QWidget *parent = 0);

	virtual ~SGMPersistentView(){}
signals:

public slots:
protected:

	void setupUi();

	AMExtendedControlEditor* energyControlEditor_;
	AMExtendedControlEditor* exitSlitControlEditor_;
	AMExtendedControlEditor* gratingSelectionControlEditor_;

};

#endif // SGMPERSISTENTVIEW_H
