#ifndef RTLS_GW_H
#define RTLS_GW_H

#include <QObject>
#include <QCoreApplication>
#include "m_anchor.h"
#include "s_anchor.h"
#include <QDomDocument>
#include <QFile>
#include <QTimer>

#include "multiLateration.h"
#include "net_if.h"

#include <tag_tracking.h>



struct tag_p_t //每个tag都会有一份
{
  quint32 tag_id;
  int seq_num;
  tag_tracking tag_track;


};


class RTLS_GW : public QCoreApplication
{
  Q_OBJECT
public:
  //    static M_Anchor * master_anchor[MA_MAX];
  //    static S_Anchor * slave_anchor[SA_MAX];

  QMap<quint32,M_Anchor *> MA_Map;
  QMap<quint32,S_Anchor *> SA_Map;
 // QMultiMap <int,tag_info>  *Anchor_map[64];//管理的基站数量
//  QMap<int, QMultiMap <int,tag_info> *> tag_map; //管理的标签, <tag_id,QMultiMap <seq_num,tag_info>>
  //QMap<int,QMap<int,QMultiMap <int,tag_info> *>> tag_map; //管理的标签, <tag_id,QMap<ma_id, QMQMultiMap <seq_num,tag_info>>>

  QMap<int, int> tag_seq; //管理的标签的seq
  QMap<int, tag_tracking * > tag_p_map;//标签的滤波

  QTimer  HP_timer; //查看基站状态定时器
  net_if *_net_if;
public:
  explicit RTLS_GW(int argc, char *argv[]);
  void saveConfigFile(QString filename, QString cfg);
  void loadConfigFile(QString filename);

  static RTLS_GW *instance();
  MultiLateration location_engine;
private:
   int check_sn();
signals:

public slots:
   void check_HP();
};

#endif // RTLS_GW_H
