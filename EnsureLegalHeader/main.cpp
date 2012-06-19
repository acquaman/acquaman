#include <QtCore/QCoreApplication>

#include "LegalHeaderChecker.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString oldNotice = "Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.";
    QString newNotice = "Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.";
    LegalHeaderChecker *checker = new LegalHeaderChecker(oldNotice, newNotice);

    Q_UNUSED(checker)

    return a.exec();
}
