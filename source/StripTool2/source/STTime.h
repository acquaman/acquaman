#ifndef STTIME_H
#define STTIME_H

#include <QObject>
#include <QStringList>
#include <QDebug>

class STTime : public QObject
{
    Q_OBJECT

public:
    enum Units { mSeconds = 0, Seconds = 1, Minutes = 2, Hours = 3};
    explicit STTime(int value = 0, Units units = Seconds, QObject *parent = 0);
    virtual ~STTime();

    int value() const;
    Units units() const;

    static QStringList unitsList();

    static double convertValue(int initialValue, STTime::Units initialUnits, STTime::Units finalUnits);
    static QString unitsToString(STTime::Units units);
    static STTime::Units stringToUnits(const QString &string);

signals:
    void valueChanged(int newValue);
    void unitsChanged(STTime::Units newUnits);

public slots:
    void setValue(int newValue);
    void setUnits(STTime::Units newUnits);

protected:
    int value_;
    Units units_;

};

#endif // STTIME_H
