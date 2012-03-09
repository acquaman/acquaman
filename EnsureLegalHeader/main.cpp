#include <QtCore/QCoreApplication>

#include "LegalHeaderChecker.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    LegalHeaderChecker *checker = new LegalHeaderChecker();

    return a.exec();
}
