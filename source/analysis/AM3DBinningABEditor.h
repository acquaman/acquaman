#ifndef AM3DBINNINGABEDITOR_H
#define AM3DBINNINGABEDITOR_H

#include <QWidget>

class AM3DBinningAB;

class AM3DBinningABEditor : public QWidget
{
	Q_OBJECT
public:
	explicit AM3DBinningABEditor(AM3DBinningAB *analysisBlock, QWidget *parent = 0);

signals:

public slots:

};

#endif // AM3DBINNINGABEDITOR_H
