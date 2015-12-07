#ifndef TAG_TRACKING_H
#define TAG_TRACKING_H
#include <QObject>
struct tag_pos_t
{
  quint8  msg_type;   //数据类型 为1
  quint8  Precision;   //数据可信度
  quint16 TAG_ID;		//标签ID
  qint32 pos[3];     //坐标值 单位mm
};


class tag_tracking
{
public:
  double R ;//测量噪声
  double Q ;//过程噪声

  double p_last[3];
  int seq_num_old;
  double x_last[3];
  qint32 x_buf[1];
  qint32 y_buf[1];
public:
  tag_tracking();
  ~tag_tracking();
  void do_tag_track(int seq_num,tag_pos_t *tagpos);
};

#endif // TAG_TRACKING_H
