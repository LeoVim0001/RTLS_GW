#ifndef M_ANCHOR_H
#define M_ANCHOR_H

#include <QObject>
#include <QUdpSocket>
#include <QDebug>
#include <QRect>
#include <Qthread>
#include <QMutex>
#include "msg_config.h"

class M_Anchor : public QThread
{  
  Q_OBJECT
public:
  explicit M_Anchor(QObject *parent = 0);
  ~M_Anchor();
  void run();
  void add_cpp(quint32 id,quint64 ticks,quint8 seq);
  void swap_buffer();
public:
  quint16 ID;
  quint16 port;
  QString IP;
  double x;
  double y;
  double z;
  quint8 SeqNum;
  //quint64 ticks;
  double txtimer;
  tag_info tag_data;

//  QMap<int, QMultiMap <int,tag_info> *> tag_map; //管理的标签, <tag_id,QMultiMap <seq_num,tag_info>>
  QMap<int, QList <tag_info >* > tag_map; //管理的标签, list 用来存放 某一个tag 的某一个seq_num的所有基站
 // QRect eff_rect;
  qint32 rect[4];
  int health_Point; //监视基站的健康状态
  qint64 old_tick;
  qint64 diff_tick;
private:
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

#endif // M_ANCHOR_H
