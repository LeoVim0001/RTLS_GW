#include <QCoreApplication>
#include "RTLS_GW.h"

int main(int argc, char *argv[])
{
    RTLS_GW a(argc, argv);
    return a.QCoreApplication::exec();
}
