#ifndef AMSAMPLECONTAINERVIEW_H
#define AMSAMPLECONTAINERVIEW_H

#include <QListView>

class AMSampleContainer;
class AMSampleEthanView;

/// This class is a view for a list of AMSampleEthans
/// uses index changed and setCurrentSelection to coordinate
/// selected sample with AMShapeDataSet
class AMSampleContainerView : public QListView
{
    Q_OBJECT
public:
    /// Constructors
    AMSampleContainerView(QWidget* parent = 0);
    AMSampleContainerView(AMSampleContainer* sampleContainer, QWidget *parent = 0);

    /// set the sample container to visualize
    void setSampleContainer(AMSampleContainer* sampleContainer);

    /// called when the current index is changed
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

public slots:
    /// sets the current selection
    void setCurrentSelection(int);

signals:
    /// signal emitted when the current index has been changed
    void indexChanged(int);

protected:
    /// initialization
    void init();
protected:
    /// the sample container
    AMSampleContainer* sampleContainer_;
    /// the view for individual samples
    AMSampleEthanView* sampleView_;
};

#endif // AMSAMPLECONTAINERVIEW_H
