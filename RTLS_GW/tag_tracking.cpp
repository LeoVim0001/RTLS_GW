#include "tag_tracking.h"

tag_tracking::tag_tracking()
{
    //    Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
    //    R:测量噪声，R增大，动态响应变慢，收敛稳定性变好
   R = 0.2;//测量噪声
   Q = 0.02;//过程噪声
   x_buf[0]=0;
   y_buf[0]=0;
//   double kg[3]={0,0,0};
   seq_num_old=50;
}

tag_tracking::~tag_tracking()
{

}



//double  tag_tracking::KalmanFilter1(int seq_num,const double ResrcData )
//{

//  /*-------------------------------------------------------------------------------------------------------------*/
//  /*
//    Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
//    R:测量噪声，R增大，动态响应变慢，收敛稳定性变好
//    */


//  static	double x_last;
//  double x_mid;
//  double x_now;
//  static double p_last;
//  double p_mid ;
//  double p_now;
//  double kg;
//  static int seq_num_old1=0;
//  if ((-seq_num_old1+256+seq_num)%256>20)
//    {
//      p_last=ResrcData;
//    }
//  seq_num_old1=seq_num;
//  x_mid=x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
//  p_mid=p_last+Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=噪声
//  kg=p_mid/(p_mid+R); //kg为kalman filter，R为噪声
//  x_now=x_mid+kg*(ResrcData-x_mid);//估计出的最优值
//  p_now=(1-kg)*p_mid;//最优值对应的covariance
//  p_last = p_now; //更新covariance值
//  x_last = x_now; //更新系统状态值
//  return x_now;
//}

void tag_tracking::do_tag_track(int seq_num,tag_pos_t *tagpos)
{

  /*-------------------------------------------------------------------------------------------------------------*/
  /*
  Q:过程噪声，Q增大，动态响应变快，收敛稳定性变坏
  R:测量噪声，R增大，动态响应变慢，收敛稳定性变好
  */



  //double x_mid;
  double x_now[3];

  double p_mid[3];
  //double p_now[3];
  double kg[3];

  if (abs(-seq_num_old+256+seq_num)%256>5)
    {
      x_last[0]=tagpos->pos[0];
      x_last[1]=tagpos->pos[1];
      p_last[0]=1;
      p_last[1]=1;

      x_buf[0]=tagpos->pos[0];
      y_buf[0]=tagpos->pos[1];
//      x_last[2]=tagpos->pos[2];
    }
  seq_num_old=seq_num;

// if((tagpos->pos[0]-x_buf[0])>2000)
// {
//     tagpos->pos[0]=x_buf[0]+2000;
// }
// if((tagpos->pos[1]-y_buf[0])>2000)
// {
//     tagpos->pos[1]=y_buf[0]+2000;
// }
 x_buf[0]=tagpos->pos[0];
 y_buf[0]=tagpos->pos[1];


  //x_mid=x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
  p_mid[0]=p_last[0]+Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=噪声
  p_mid[1]=p_last[1]+Q;
//  p_mid[2]=p_last[2]+Q;
  kg[0]=p_mid[0]/(p_mid[0]+R); //kg为kalman filter，R为噪声
  kg[1]=p_mid[1]/(p_mid[1]+R);
  if (kg[0]>=1)
      kg[0]=0.99;
  if (kg[1]>=1)
      kg[1]=0.99;
//  kg[2]=p_mid[2]/(p_mid[2]+R);
  x_now[0]=x_last[0]+kg[0]*(tagpos->pos[0]-x_last[0]);//估计出的最优值
  x_now[1]=x_last[1]+kg[1]*(tagpos->pos[1]-x_last[1]);
//  x_now[2]=x_last[2]+kg[2]*(tagpos->pos[2]-x_last[2]);
//  p_now=(1-kg)*p_mid;//最优值对应的covariance
//  p_last = p_now; //更新covariance值
   p_last[0]=(1-kg[0])*p_mid[0];
   p_last[1]=(1-kg[1])*p_mid[1];
//   p_last[2]=(1-kg[2])*p_mid[2];
//  x_last = x_now; //更新系统状态值
//  return x_now;
   x_last[0]=x_now[0];
   x_last[1]=x_now[1];
//   x_last[2]=x_now[2];
   tagpos->pos[0]=x_now[0];
   tagpos->pos[1]=x_now[1];
//   tagpos->pos[2]=x_now[2];


}
