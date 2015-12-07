
#include "multiLateration.h"
#include "rtls_gw.h"
#include "m_anchor.h"
#include "tag_tracking.h"

using namespace arma;

int m_tag_id;
////上报给gis服务器的数据格式


tag_pos_t tag_pos;
MultiLateration::MultiLateration()
{

}




////void MultiLateration::set_net_if(net_if * my_net_if)
////{
////    net_work_interface=my_net_if;
////}

bool MultiLateration::get_4a_postion(QList <tag_info> * tag_data_list)
{
    colvec ddoa(3),delta(3);
    colvec R(3);
    colvec raw_xyz(3);
    mat result1, result2;
    mat S(3,3);
    mat Sw,SwTSw;
    mat tmp,mm;
    vec  a, b, c, delta2rsd1, delta2rsd2;
    double rs1;
    // int AnchorID[32];
    int i;

    for(i=0;i<(tag_data_list->count()-1);i++)
    {

        S(i,0)=tag_data_list->at(i+1).x-tag_data_list->at(0).x; //计算x坐标差值
        S(i,1)=tag_data_list->at(i+1).y-tag_data_list->at(0).y;//计算y坐标差值
        S(i,2)=tag_data_list->at(i+1).z-tag_data_list->at(0).z;//计算z坐标差值
        ddoa[i]=tag_data_list->at(i+1).tickes-tag_data_list->at(0).tickes;

        R[i]=S(i,0)*S(i,0)+S(i,1)*S(i,1)+S(i,2)*S(i,2);//R*R=X*X+Y*Y+Z*Z
        delta[i]=R[i]-ddoa[i]*ddoa[i];
    }


    raw_xyz[0]=tag_data_list->at(0).x;
    raw_xyz[1]=tag_data_list->at(0).y;
    raw_xyz[2]=tag_data_list->at(0).z;
    tmp=(S.st()*S);
    mm=pinv(tmp);
    Sw =mm * S.st();
    SwTSw = Sw.st() * Sw;

    a = 4 - 4 * (ddoa.st()*SwTSw*ddoa);
    b = 4 * ddoa.st() * SwTSw * delta;
    c = -1 * delta.st() * SwTSw * delta;

    double t = b(0)*b(0) - 4 * a(0)*c(0);
    if (t < 0)
    {

//        result1.set_size(1, 1);
//        result1(0, 0) = 0;
        qDebug("no result");
        rs1 = (-b(0) ) / (2 * a(0));
        delta2rsd1 = delta - 2 * rs1*ddoa;
        result1 = (raw_xyz + ((Sw*delta2rsd1) / 2)).st();
        tag_pos.pos[0]=(result1.at(0,0));
        tag_pos.pos[1]=(result1.at(0,1));
        tag_pos.pos[2]=(result1.at(0,2));

    }
    else
    {
        rs1 = (-b(0) + sqrt(t)) / (2 * a(0));
        delta2rsd1 = delta - 2 * rs1*ddoa;
        result1 = (raw_xyz + ((Sw*delta2rsd1) / 2)).st();
        tag_pos.pos[0]=(result1.at(0,0));
        tag_pos.pos[1]=(result1.at(0,1));
        tag_pos.pos[2]=(result1.at(0,2));
        //  cout<< result1.row(0) << endl;

        //	cout<<result1.row(0)*0.469035687 << endl;
    }
    return true;
}


////bool MultiLateration::get_5a_postion(QList <tag_info> * tag_data_list)
////{
////    Q_ASSERT( tag_data_list->count()>=5);
////    int AnchorID[32];
////    int BSN=tag_data_list->count();
////    mat Ga (BSN-1,4);
////    colvec h (BSN-1);
////    mat Q=eye<mat>(BSN-1,BSN-1);
////    colvec  Za(4);
////    mat B =Q;
////    mat FI (BSN-1,BSN-1);
////    int j=0;
////    double acc_pos[3]={0};//xyz 坐标累加器
////    for(int i=0;i<BSN;i++)
////    {
////        AnchorID[i]=tag_data_list->at(i).Anchor_ID; //读取基站的id 通过基站的id索引到真实的基站id
////    }
////    for (int k = 0; k < BSN-1; k++)
////    {
////        Ga(k,0)=-myAnchor[AnchorID[k+1]]->pos[0]+myAnchor[AnchorID[j]]->pos[0]; //计算x坐标差值
////        Ga(k,1)=-myAnchor[AnchorID[k+1]]->pos[1]+myAnchor[AnchorID[j]]->pos[1];//计算y坐标差值
////        Ga(k,2)=-myAnchor[AnchorID[k+1]]->pos[2]+myAnchor[AnchorID[j]]->pos[2];//计算z坐标差值

////        qint64 tt=-tag_data_list->at(k+1).tickes+tag_data_list->at(j).tickes; //计算时间差;
////        Ga(k,3)=tt;
////        h(k)= (Ga(k,3)*Ga(k,3)-myAnchor[AnchorID[k+1]]->Ki+myAnchor[AnchorID[0]]->Ki)/2;

////    }

////    //%第一次LS：

////    Za = inv(Ga.st()*inv(Q)*Ga)*Ga.st()*inv(Q)*h;
////    //  % 利用这个粗略估计值计算B：
////    for (int i = 0; i < BSN-1; i++)
////    {
////        B(i,i) = sqrt(pow((myAnchor[AnchorID[i+1]]->pos[0] - Za(0)),2) + pow((myAnchor[AnchorID[i+1]]->pos[1] - Za(1)),2)+pow((myAnchor[AnchorID[i+1]]->pos[2] - Za(2)),2));
////    }

////    //% FI:
////    FI= B*Q*B;

////    mat Zm=inv(Ga.st()*inv(FI)*Ga)*Ga.st()*inv(FI )*h;

////    acc_pos[0]=Za.at(0);
////    acc_pos[1]=Za.at(1);
////    acc_pos[2]=Za.at(2);
////    //cout<<"1 "<< acc_pos[0]*0.469035687<<" ,"<<acc_pos[1]*0.469035687<<" ,"<< acc_pos[2]*0.469035687<<endl;
////    tag_pos.pos[0]=acc_pos[0]*469.035687;
////    tag_pos.pos[1]=acc_pos[1]*469.035687;
////    tag_pos.pos[2]=acc_pos[2]*469.035687;

////    return 1;

////}




//double R = 0.2;//测量噪声
//double Q = 0.02;//过程噪声

//double  KalmanFilter1(const double ResrcData )
//{

//  /*-------------------------------------------------------------------------------------------------------------*/
//  /*
//    Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
//    R:测量噪声，R增大，动态响应变慢，收敛稳定性变好
//    */


//  static	double x_last;
//  double x_mid;
//  double x_now;
//  static double p_last=ResrcData;
//  double p_mid ;
//  double p_now;
//  double kg;
//  x_mid=x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
//  p_mid=p_last+Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=噪声
//  kg=p_mid/(p_mid+R); //kg为kalman filter，R为噪声
//  x_now=x_mid+kg*(ResrcData-x_mid);//估计出的最优值
//  p_now=(1-kg)*p_mid;//最优值对应的covariance
//  p_last = p_now; //更新covariance值
//  x_last = x_now; //更新系统状态值
//  return x_now;
//}

//double  KalmanFilter2(const double ResrcData )
//{

//  /*-------------------------------------------------------------------------------------------------------------*/
//  /*
//    Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
//    R:测量噪声，R增大，动态响应变慢，收敛稳定性变好
//    */

//  //double R = 0.1;//测量噪声
//  //double Q = 0.001;//过程噪声
//  static	double x_last;
//  double x_mid;
//  double x_now;
//  static double p_last=ResrcData;
//  double p_mid ;
//  double p_now;
//  double kg;
//  x_mid=x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
//  p_mid=p_last+Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=噪声
//  kg=p_mid/(p_mid+R); //kg为kalman filter，R为噪声
//  x_now=x_mid+kg*(ResrcData-x_mid);//估计出的最优值
//  p_now=(1-kg)*p_mid;//最优值对应的covariance
//  p_last = p_now; //更新covariance值
//  x_last = x_now; //更新系统状态值
//  return x_now;
//}

//int MultiLateration::calc_position(int tag_id,int seq_num,QList <tag_info> * tag_data_list)
//{
//  m_tag_id=tag_id;
//  // QList <tag_info>  tag_data_list_sort;
//  //       qDebug()<<tag_data_list->size();
//  //     double dfff[4];
//  //    for(int i=0;i<4;i++)
//  //   {
//  //      dfff[tag_data_list->at(i).Anchor_ID]=tag_data_list->at(i).tickes;

//  //    }
// //  qDebug() <<dfff[0]-dfff[1]<<","<<dfff[2]-dfff[1]<<","<<dfff[3]-dfff[2]<<","<<dfff[3]-dfff[0]<<","<<dfff[3]-dfff[1]<<","<<dfff[2]-dfff[0];

//  if (tag_data_list->size()==4)
//    {

//      if( get_4a_postion(tag_data_list)==0)
//        return 0;
//      //       // if(tag_pos.pos[2]<2000)

//      //             {
//      //          qDebug()<<"ID:"<<1<<" x:"<<tag_pos.pos[0]<<" y:"<<tag_pos.pos[1]<<" z: "<<tag_pos.pos[2];
//      ////        }
//      //        tag_data_list->append(tag_data_list->at(0));
//      //        tag_data_list->removeFirst();
//      //        get_4a_postion(tag_data_list);
//      ////       // if(tag_pos.pos[2]<2000)
//      //             {
//      //          qDebug()<<"ID:"<<2<<" x:"<<tag_pos.pos[0]<<" y:"<<tag_pos.pos[1]<<" z: "<<tag_pos.pos[2];
//      //        }
//      //        tag_data_list->append(tag_data_list->at(0));
//      //        tag_data_list->removeFirst();
//      //        get_4a_postion(tag_data_list);
//      //       //if(tag_pos.pos[2]<2000)
//      //             {
//      //          qDebug()<<"ID:"<<3<<" x:"<<tag_pos.pos[0]<<" y:"<<tag_pos.pos[1]<<" z: "<<tag_pos.pos[2];
//      //        }
//      //        tag_data_list->append(tag_data_list->at(0));
//      //        tag_data_list->removeFirst();
//      //        get_4a_postion(tag_data_list);
//      //        if(tag_pos.pos[2]<2000)
//      //             {
//        //        qDebug()<<"ID:"<<4<<" x:"<<tag_pos.pos[0]<<" y:"<<tag_pos.pos[1]<<" z: "<<tag_pos.pos[2];
//      //        }

//    }else if(tag_data_list->size()>4)
//    {
//      //        if (get_5a_postion(tag_data_list)==0)
//      //            return;
//    }
//  //      tag_pos.pos[0]=KalmanFilter1(seq_num,tag_pos.pos[0]);
//  //      tag_pos.pos[1]=KalmanFilter2(seq_num,tag_pos.pos[1]);
//  //   tag_pos.pos[2]=KalmanFilter3(tag_pos.pos[2]);
//  //      tag_pos.pos[0]=kalman_ft[0].do_kalman_filter(1,tag_pos.pos[0]);
//  //      tag_pos.pos[1]=kalman_ft[1].do_kalman_filter(1,tag_pos.pos[1]);
//  //    tag_pos.pos[2]=kalman_ft[2].do_kalman_filter(1,tag_pos.pos[2]);


//  // net_work_interface->send_rp_data(( char *)&tag_pos,sizeof(tag_pos)); //发送上报数据

//}

void MultiLateration::proc_tag_location(int ma_id,int tag_id,int seq_num)
{
    //anchor_count=RTLS_GW::instance()->MA_Map.value(ma_id)->tag_map.value(tag_id)->values(seq_num).count();
    tag_data_list=RTLS_GW::instance()->MA_Map.value(ma_id)->tag_map.value(tag_id);
    // qDebug()<<tag_data_list->count();
    tag_pos.TAG_ID=tag_id;
    tag_pos.msg_type=0x01;
    tag_pos.Precision=1;
    m_tag_id=tag_id;
    if(tag_data_list->count()>=4)
    {
        //qDebug()<<seq_num;
//        get_4a_postion(tag_data_list);
//        qDebug()<<seq_num<<"ID:"<<tag_pos.TAG_ID<<" x:"<<tag_pos.pos[0]<<" y:"<<tag_pos.pos[1]<<" z: "<<tag_pos.pos[2];
//        tag_data_list->swap(0,1);
//        get_4a_postion(tag_data_list);
//        qDebug()<<seq_num<<"ID:"<<tag_pos.TAG_ID<<" x:"<<tag_pos.pos[0]<<" y:"<<tag_pos.pos[1]<<" z: "<<tag_pos.pos[2];
//        tag_data_list->swap(0,2);
//        get_4a_postion(tag_data_list);
//        qDebug()<<seq_num<<"ID:"<<tag_pos.TAG_ID<<" x:"<<tag_pos.pos[0]<<" y:"<<tag_pos.pos[1]<<" z: "<<tag_pos.pos[2];
//        tag_data_list->swap(0,3);
//        get_4a_postion(tag_data_list);
//        qDebug()<<seq_num<<"ID:"<<tag_pos.TAG_ID<<" x:"<<tag_pos.pos[0]<<" y:"<<tag_pos.pos[1]<<" z: "<<tag_pos.pos[2];

              if(!(get_4a_postion(tag_data_list)))
                {
                  tag_data_list->clear();
                  return;
                }
        tag_data_list->clear();
        RTLS_GW::instance()->MA_Map.value(ma_id)->tag_map.value(tag_id)->clear();//若果成功清除现在和以前不到4个的数据
              M_Anchor * _ma=RTLS_GW::instance()->MA_Map.value(ma_id);
              if ((_ma->rect[0]>tag_pos.pos[0])||(_ma->rect[1]<tag_pos.pos[0])||(_ma->rect[2]>tag_pos.pos[1])||(_ma->rect[3]<tag_pos.pos[1]))
                {
                  qDebug()<<ma_id<<"ID:"<<tag_id<<" x:"<<tag_pos.pos[0]<<" y:"<<tag_pos.pos[1]<<" z: "<<tag_pos.pos[2]<<"out rect";
                  return;
                }

//            if(tag_pos.pos[1]<0)
//            {
//                qDebug("ww");
//            }
        if((tag_pos.pos[2]>-1000)&&(tag_pos.pos[2]<3000))
        {
            RTLS_GW::instance()->tag_p_map[tag_id]->do_tag_track(seq_num,&tag_pos);
            RTLS_GW::instance()->_net_if->send_rp_data(( char *)&tag_pos,sizeof(tag_pos)); //发送上报数据)
            qDebug()<<seq_num<<"ID:"<<tag_pos.TAG_ID<<" x:"<<tag_pos.pos[0]<<" y:"<<tag_pos.pos[1]<<" z: "<<tag_pos.pos[2];
        }
    }


}


//void MultiLateration::deal_data()
//{
//  //  int cnt;
//  //  int first_key,last_key,cur_key;
//  //  int ok_key;
//  //  int tag_cnt=RTLS_GW::instance()->tag_cnt;
//  //  for (cnt=0;cnt<tag_cnt;cnt++)
//  //    {
//  //      if(RTLS_GW::instance()-> Anchor_map[cnt]->isEmpty())
//  //        continue;

//  //      first_key=RTLS_GW::instance()->Anchor_map[cnt]->firstKey();//获取最早的seq
//  //      last_key=RTLS_GW::instance()->Anchor_map[cnt]->lastKey();//获取最新的seq
//  //      ok_key=first_key;
//  //      if (RTLS_GW::instance()->Anchor_map[cnt]->count()>254) //如果数据超过seq最大值,一定错误
//  //        {
//  //          RTLS_GW::instance()-> Anchor_map[cnt]->clear();
//  //          return;
//  //        }
//  //      if(last_key<255)//如果没有翻转;
//  //        {

//  //          // 找出能计算的数据并且计算
//  //          for(cur_key=first_key;cur_key<=last_key;cur_key++)
//  //            {
//  //              tag_data_list=RTLS_GW::instance()->Anchor_map[cnt]->values(cur_key);
//  //              // cout<<first_key<<"<<<"<<last_key<<"count"<<tag_data_list.count()<<endl;

//  //              if(tag_data_list.count()>=4) //如果满足计算条件
//  //                {
//  //                  ok_key=cur_key;
//  //                  //计算数据
//  //                  calc_position(RTLS_GW::instance()->tag_map.key(cnt),cur_key,&tag_data_list);
//  //                }
//  //            }

//  //          //删除老的不满足条件的数据或者已经计算过的数据;
//  //          for(cur_key=first_key;cur_key<=(ok_key);cur_key++)
//  //            {

//  //              RTLS_GW::instance()->Anchor_map[cnt]->remove(cur_key);//删除数据
//  //            }

//  //        }else //seq已经翻转
//  //        {
//  //          RTLS_GW::instance()->Anchor_map[cnt]->clear();
//  //        }


//  //      // 找出能计算的数据并且计算
//  //      //for(cur_key=first_key;cur_key!=last_key;cur_key++)
//  //      //{
//  //      //	if (cur_key>=256)  //处理seqnum 翻转的问题;
//  //      //		cur_key=0;
//  //      //	tag_data_list=Anchor_map[cnt]->values(cur_key);
//  //      //	if(tag_data_list.count()>=2) //如果满足计算条件
//  //      //	{
//  //      //		ok_key=cur_key;
//  //      //		//计算数据
//  //      //		MultiLateration(&tag_data_list);
//  //      //	}
//  //      //}
//  //      ////删除老的不满足条件的数据或者已经计算过的数据;
//  //      //for(cur_key=first_key;cur_key!=(ok_key);cur_key++)
//  //      //{
//  //      //	if (cur_key>=256)  //处理seqnum 翻转的问题;
//  //      //		cur_key=0;
//  //      //	Anchor_map[cnt]->remove(cur_key);//删除数据
//  //      //}

//  //    }
//}


