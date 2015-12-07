#ifndef HW_ID_H
#define HW_ID_H
#include <QStringList>
#include <QString>
class HW_ID
{
public:
    HW_ID();
    int check_sn();
    void get_mac();
    void cpu_getid();
    QString cpu_id;
    QStringList mac_list;
};

#endif // HW_ID_H
