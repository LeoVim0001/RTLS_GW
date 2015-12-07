#include "cs_kalman.h"
#include "rtls_gw.h"

//static const double CLOCK_PERIOD_SEC = (0x10000000000) / (64 * 998.4e6);
static const qint64 CLOCK_PERIOD_SEC =(0x10000000000);

CS_Kalman::CS_Kalman(void)
{
   
}

//tof帧调用
double CS_Kalman::syncTOA(double blinkTime)
{
}

//ccp帧调用
void CS_Kalman::process(qint64 timeRxus, qint64 timeTxus, quint8 seqnum)
{
   



      // qDebug()<<"!"<<abs(syncTOA(timeRxus)-timeTxus)/(1.0/499.2e6/128.0);
}
