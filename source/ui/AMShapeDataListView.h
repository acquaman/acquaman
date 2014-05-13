#ifndef AMSHAPEDATALISTVIEW_H
#define AMSHAPEDATALISTVIEW_H

#include <QListView>


class AMSampleCamera;
class AMSampleView;

class AMShapeDataListView : public QListView
{
    Q_OBJECT
public:
    AMShapeDataListView(QWidget* parent = 0);

    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

signals:
    void currentIndexChanged();

protected:
    AMSampleCamera* shapeModel();

};

#endif // AMSHAPEDATALISTVIEW_H
