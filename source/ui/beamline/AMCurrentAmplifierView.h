//#ifndef AMCURRENTAMPLIFIERVIEW_H
//#define AMCURRENTAMPLIFIERVIEW_H

//#include <QWidget>
//#include <QComboBox>
//#include <QToolButton>
//#include <QLayout>
//#include <QDebug>
//#include <QMenu>

//#include "beamline/AMCurrentAmplifier.h"

//class AMCurrentAmplifierView : public QWidget
//{
//    Q_OBJECT

//public:
//    /// Simple enum holding whether the view should be either basic or advanced looking.
//    enum ViewMode { Basic = 0, Advanced = 1 };
//    /// Enum holding whether the view should show value in terms of sensitivity or gain.
//    enum ValueMode { Sensitivity = 0, Gain = 1};

//    /// Constructor. Takes in instance of AMCurrentAmplifier.
//    explicit AMCurrentAmplifierView(AMCurrentAmplifier *amplifier, ValueMode mode, QWidget *parent = 0);
//    virtual ~AMCurrentAmplifierView();

//    /// Returns the pointer to the AMCurrentAmplifier this view is visualizing.
//    AMCurrentAmplifier *currentAmplifier() const;

//    /// Returns the current view mode.
//    AMCurrentAmplifierView::ViewMode viewMode() const;
//    /// Returns the current value mode.
//    AMCurrentAmplifierView::ValueMode valueMode() const;

//    /// Sets the current view mode.
//    void setViewMode(ViewMode newMode);
//    /// Sets the current value mode.
//    void setValueMode(ValueMode newMode);

//signals:
//    /// Notifier that the view mode has changed. Passes the new view mode.
//    void viewModeChanged(AMCurrentAmplifierView::ViewMode);
//    /// Notifier that the value mode has changed. Passes the new value mode.
//    void valueModeChanged(AMCurrentAmplifierView::ValueMode);

//public slots:

//protected slots:
//    void onValueModeSelected(ValueMode newMode);

//    /// Handles passing changes in the value combo box to the amplifier.
//    virtual void onValueComboBoxChanged(int index) = 0;
//    /// Handles passing changes in the units combo box to the amplifier.
//    virtual void onUnitsComboBoxChanged(int index) = 0;
//    /// Handles setting the value combo box when the amplifier's value has been changed independently.
//    virtual void onValueChanged(int value) = 0;
//    /// Handles setting the units combo box when the amplifier's units have been changed independently.
//    virtual void onUnitsChanged(QString units) = 0;

//    /// Builds a popup menu for switching view modes.
//    virtual void onCustomContextMenuRequested(QPoint pos);

//protected:
//    /// The pointer to the current amplifier this view manages.
//    AMCurrentAmplifier *amplifier_;

//    /// Flag holding whether or not the view is basic or advanced.
//    ViewMode viewMode_;
//    /// Flag holding whether or not the sensitivity or gain value is displayed.
//    ValueMode valueMode_;

//    /// Combo box holding the amplifier value.
//    QComboBox *value_;
//    /// Combo box holding the amplifier units.
//    QComboBox *units_;

//    /// The tool button for the minus button.
//    QToolButton *minus_;
//    /// The tool button for the plus button.
//    QToolButton *plus_;


//};

//#endif // AMCURRENTAMPLIFIERVIEW_H
