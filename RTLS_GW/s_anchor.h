#ifndef S_ANCHOR_H
#define S_ANCHOR_H

#include <QObject>
#include <QUdpSocket>
#include <QDebug>
#include <QBuffer>
#include "msg_config.h"
#include "cs_kalman.h"
#include "QList"
#include <QThread.h>
#include <QMutex>

#define MAX_BUFF_LEN 20
typedef struct
{
    uint8_t 	SeqNum;//序列号
    quint16 	M_ID;//主基站的id
    quint64 	tickers;
} ccp_buffer_t;


//typedef struct    // 定义缓存结构
//{
//  quint32   readPos;
//  quint32   writePos;
//  ccp_buffer_t  DATA[MAX_BUFF_LEN];
//}DATAS_BUFF_T;
////结构体成为一个buf


class S_Anchor : public QThread
{
    Q_OBJECT
public:
    explicit S_Anchor(QObject *parent = 0);
    ~S_Anchor();
    void run();
    void proc_sync();

    void get_disa2ma();
    void proc_frame();
    void swap_buffer();
public:
    quint16 ID;
    quint16 port;
    QString IP;
    double x;
    double y;
    double z;
    double pos[3] ;
    CS_Kalman cs_kalman;//ccp校时类
    bool is_sync;
    //QBuffer ccp_buf;//定义最多4个buffer
    ccp_buffer_t ccp_buf;
    double rxtimer;
    tag_info tag_data;
    //  DATAS_BUFF_T   Buff0;
    qint64 dis_a2ma; //计算基站到主基站的距离
    double KI;
    // QList <int> ma_id;
    int health_Point; //监视基站的健康状态
    int ma_id;
    qint64 old_tick;
    qint64 diff_tick;
    qint8 sync_err_cnt;
     int UDP_CNT;
private:
    QByteArray bytebuffer;
    QByteArray datagram;
    QUdpSocket *udpSocket;//UDP 的的类

public:
  QMutex            bufferLocker;
  volatile bool     recv_index;
  QList<QByteArray> recvBuffer[2];

signals:

public slots:
    void read_UDP();
    void dispatch();

};

#endif // S_ANCHOR_H
