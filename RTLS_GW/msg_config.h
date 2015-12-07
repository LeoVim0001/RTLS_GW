#ifndef MSG_CONFIG
#define MSG_CONFIG
#include <QObject>
#include <stdint.h>
//#define TIME_UNITS          (1.0/499.2e6/128.0)
#define TIME_UNITS          1

const double cMps = 299702547;  // Speed of Light m/s
const double s2d = (1.0 / 499.2e6 / 128.0); // 15.65e-12

#define MA_MAX  30
#define SA_MAX  300

//typedef unsigned char  uint8;

#define fc_an_ccp 0x60
#define fc_an_tag 0x70
#define master_ccp 0x61

#pragma pack (1)
typedef struct
{
        quint8  	title[3];
        quint8 		type;
        qint8			rssi_all;
        qint8			rssi_first;
        quint8    		device_type;
        quint8 		rev;
        quint64 		M_ID:16;
        quint64  		seq_num:8;
        quint64 		tick:40;
        quint8   	end[13];
} anchor_ccptopc_t;

typedef struct
{
    quint8  	title[3];
    quint8 		type;
    qint8			rssi_all;
    qint8			rssi_first;
    quint8   		voltage;
    quint8    		hight;
    quint64 		TAG_ID:16;
    quint64  		seq_num:8;
    quint64 		tick:40;
    quint8   	end[13];
}tagtopc_t;

typedef struct
{
    quint8  	title[3];
    quint8 		type;
    qint8			rssi_all;
    qint8			rssi_first;
    quint8    		device_type;
    quint8 		rev;
    quint64 		ID:16;
    quint64  		seq_num:8;
    quint64 		tick:40;
    quint8   	end[13];
} m_ccptopc_t;


struct tag_info
{
  quint16 Anchor_ID; //基站类ID
  qint8	rssi_all;
  qint8	rssi_first;
  qint64 tickes ;
  qint64 x;
  qint64 y;
  qint64 z;
  qint8 seq_num;
};



struct DATA_F
{
  uint8_t SOF; //
  uint16_t	length;
  int8_t	* data;
};



#pragma pack () /*取消指定对齐，恢复缺省对齐*/


#endif // MSG_CONFIG

