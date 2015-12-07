#ifndef MULTILATERATION_H
#define MULTILATERATION_H

#include "s_anchor.h"
#include <math.h>
#include <armadillo>
#include <QObject>
#include <QDebug>
//#include <kalman.h>

//const double cMps = 299702547;  // 光速 m/s
//const double s2d = (1.0 / 499.2e6 / 128.0); // 计数器对应的时间 15.65e-12
//#define  s2d   (1.0 / 499.2e6 / 128.0)



//用于计算坐标值;计算完成以后将计算出的结构传回去
class MultiLateration
{

public:
  MultiLateration();
  //void set_net_if(net_if * my_net_if);
 // int calc_position(int tag_id,int seq_num,QList <tag_info> * tag_data_list);
  bool get_4a_postion(QList <tag_info> * tag_data_list); //计算4个基站坐标的位置;
  //  //  bool get_5a_postion(QList <tag_info> * tag_data_list);//计算5个或者5个以上的基站坐标位置
  void deal_data();//分析和处理数据
  void proc_tag_location(int ma_id,int tag_id,int seq_num);
public:

  QList <tag_info>  *tag_data_list;
  // net_if  *net_work_interface;
  // kalman kalman_ft[3];
};



#endif // MULTILATERATION_H
