#ifndef AMSHAPEDATALISTVIEW_H
#define AMSHAPEDATALISTVIEW_H

#include <QListView>


class AMShapeDataSet;
class AMSampleEthanView;

class AMShapeDataListView : public QListView
{
    Q_OBJECT
public:
    AMShapeDataListView(QWidget* parent = 0);

    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

signals:
    void currentIndexChanged();

protected:
    AMShapeDataSet* shapeModel();

protected:
    AMSampleEthanView* sampleView_;
};

#endif // AMSHAPEDATALISTVIEW_H
