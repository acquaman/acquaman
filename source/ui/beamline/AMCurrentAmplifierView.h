#ifndef AMCURRENTAMPLIFIERVIEW_H
#define AMCURRENTAMPLIFIERVIEW_H

#include <QWidget>
#include <QToolButton>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QMenu>

#include "beamline/AMCurrentAmplifier.h"

class AMCurrentAmplifierView : public QWidget
{
    Q_OBJECT

public:
    /// Simple enum holding whether the view should be either basic or advanced looking.
    enum ViewMode { Basic = 0, Advanced = 1 };

    /// Constructor.
    explicit AMCurrentAmplifierView(QWidget *parent = 0);
    /// Destructor.
    virtual ~AMCurrentAmplifierView();

    bool initialized() const;
    AMCurrentAmplifierView::ViewMode viewMode() const;
    QString name() const;

signals:
    void initialized(bool);
    void viewModeChanged(ViewMode newMode);
    void nameChanged(const QString &name);

public slots:
    void setInitialized(bool isInitialized);
    void setViewMode(ViewMode newMode);
    void setName(const QString &newName);
    void showName(bool show);
    void setViewableValuesMax(int newMax);
    void refreshView();

protected slots:
    virtual void onValueComboBoxChanged(const QString &newText) = 0;
    virtual void onMinusClicked() = 0;
    virtual void onPlusClicked() = 0;
    virtual void onCustomContextMenuRequested(QPoint position);

protected:
    bool initialized_;
    ViewMode viewMode_;
    QLabel *name_;
    QToolButton *minus_;
    QToolButton *plus_;
    QComboBox *value_;
    QHBoxLayout *layout_;

};

#endif // AMCURRENTAMPLIFIERVIEW_H
