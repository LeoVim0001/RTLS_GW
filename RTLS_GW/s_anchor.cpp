#include "s_anchor.h"
#include "rtls_gw.h"
#include "m_anchor.h"




//const double time_unit=64.0*998400000;// 1/ (64 * 998.4e6)=15.65ps

S_Anchor::S_Anchor(QObject *parent) : QThread(parent) ,recv_index(false)
{
    UDP_CNT=0;
    sync_err_cnt=10;
    health_Point=2;
    is_sync=true;    
}

S_Anchor::~S_Anchor()
{
    delete udpSocket;
}

void S_Anchor::run()
{
    tag_data.Anchor_ID=ID;
    tag_data.x=x;
    tag_data.y=y;
    tag_data.z=z;

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(port, QUdpSocket::ShareAddress| QUdpSocket::ReuseAddressHint ); //绑定端口
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(read_UDP()), Qt::DirectConnection);

    get_disa2ma();
    pos[0]=x;
    pos[1]=y;
    pos[2]=z;

}

void S_Anchor::proc_sync()
{
    //double syncTxTime, syncRxTime;
    if(is_sync)
        return;
    //先同步到id为0的基站
    if (RTLS_GW::instance()->MA_Map.contains(ccp_buf.M_ID))
    {
        if (ccp_buf.SeqNum==RTLS_GW::instance()->MA_Map.value(ccp_buf.M_ID)->SeqNum)
        {

            rxtimer=ccp_buf.tickers*TIME_UNITS;
            double timeTxus=RTLS_GW::instance()->MA_Map.value(ccp_buf.M_ID)->txtimer;

            if(sync_err_cnt<10)//如果同步错误，下次尝试强制同步
            {
                if(abs(cs_kalman.syncTOA(rxtimer)-timeTxus)>500)
                {
//                     printf("%d, %d sync err\n",ccp_buf.SeqNum,ID);//打印同步失败的基站的id
                    //printf("S%d\n",abs((cs_kalman.syncTOA(rxtimer)-timeTxus)));
                    sync_err_cnt++;
                    is_sync=true;//同步出错，放弃同步
                    return;
                }
            }
           // printf("%d sync ok\n",ID);//打印同步失败的基站的id
            sync_err_cnt=0;
            cs_kalman.process(rxtimer,RTLS_GW::instance()->MA_Map.value(ccp_buf.M_ID)->txtimer,ccp_buf.SeqNum);
            is_sync=true;
            //qDebug()<<ID<<"s";
        }
    }
}

void S_Anchor::swap_buffer()
{
    bufferLocker.lock();
    recv_index =!recv_index;
    recvBuffer[!recv_index].swap(recvBuffer[recv_index]);
    bufferLocker.unlock();
}
//UDP接收到数据以后回调函数
void S_Anchor::read_UDP()
{


    // quint16 r_cmd;
    //hasPendingDatagrams返回true时表示至少有一个数据包在等待被读取

//    char data_buf[1024];
//    memset(data_buf,0,sizeof(data_buf));
//    int data_p=0;
    bufferLocker.lock();
    while(udpSocket->hasPendingDatagrams())
    {
        //pendingDatagramSize为返回第一个在等待读取报文的size，resize函数是把datagram的size归一化到参数size的大小一样
        datagram.resize(udpSocket->pendingDatagramSize());
        //将读取到的不大于datagram.size()大小数据输入到datagram.data()中，datagram.data()返回的是一个字节数组中存储
        //数据位置的指针
        udpSocket->readDatagram(datagram.data(), datagram.size());
        recvBuffer[recv_index].append(datagram);
    }
    bufferLocker.unlock();
}

void S_Anchor::dispatch()
{
    swap_buffer();

    QList<QByteArray>::iterator it = recvBuffer[!recv_index].begin();
    while(it != recvBuffer[!recv_index].end())
    {
        char *pbuf;
        pbuf=(*it).data();

        if(pbuf[0]!='Y')
        {
           printf("%d,%d.%d\n",ID,pbuf[0],datagram.size());
        }

        health_Point=2;
        switch (pbuf[3])
        {
        case fc_an_ccp:   //副基站收到的ccp帧
        {

            anchor_ccptopc_t *anchor_ccptopc;
            anchor_ccptopc=(anchor_ccptopc_t *)pbuf;

            //        if(anchor_ccptopc->rssi_first<-91)
            //       {
            //          //  printf("rssi_err %d",anchor_ccptopc->rssi_first);
            //            qDebug()<<"rssi_err %d"<<anchor_ccptopc->rssi_first;
            //            return;
            //        }
    //        diff_tick=anchor_ccptopc->tick-ccp_buf.tickers;
            //        if (diff_tick>2e39) //如果翻转
            //            diff_tick=2e40-diff_tick;
    //         qDebug()<<ID<<anchor_ccptopc->seq_num<<anchor_ccptopc->tick<<diff_tick;
    //        if(anchor_ccptopc->M_ID!=ma_id) //如果接受到的不是设定的主基站的ccp帧，则不做任何处理
    //            return;
            ccp_buf.M_ID=anchor_ccptopc->M_ID;
            ccp_buf.tickers=anchor_ccptopc->tick;
            ccp_buf.SeqNum=anchor_ccptopc->seq_num;
            is_sync=false;
            proc_sync();//尝试去同步
        }
            break;

        case fc_an_tag:
        {
            tagtopc_t *tagtopc;
            tagtopc=(tagtopc_t *)pbuf;

            proc_sync();//若果之前没有同步成功再次尝试去同步
            double anchorTOAtime_raw=tagtopc->tick;
            double toaticks=cs_kalman.syncTOA(anchorTOAtime_raw);

            quint32 tag_id=tagtopc->TAG_ID;
            quint8 seq_num=tagtopc->seq_num;
            if (RTLS_GW::instance()->MA_Map.value(ma_id)->tag_map.contains(tag_id))  //如果已经有了map,则直接增加map.否则创建map;
            {
                if(RTLS_GW::instance()->tag_seq.value(tag_id)!=seq_num)
                {
                    RTLS_GW::instance()->tag_seq[tag_id]=seq_num;//更新seqnum
                    RTLS_GW::instance()->MA_Map.value(ma_id)->tag_map.value(tag_id)->clear();
                    //qDebug()<<seq_num<<"clear";
                }
                tag_data.tickes=toaticks*4.69035687+dis_a2ma;//
                RTLS_GW::instance()->MA_Map.value(ma_id)->tag_map.value(tag_id)->append(tag_data);//插入基站数据
              //   qDebug()<<ID<<tagtopc->TAG_ID<<tagtopc->seq_num<<tagtopc->rssi_all<<tagtopc->rssi_first;
                  qDebug()<<ID<<tagtopc->TAG_ID<<tagtopc->seq_num<<tag_data.tickes<<toaticks;
                RTLS_GW::instance()->location_engine.proc_tag_location(ma_id,tag_id,seq_num);

            }else
            {
                //RTLS_GW::instance()->Anchor_map[RTLS_GW::instance()->tag_cnt]=new QMultiMap <int,tag_info> ;
                //RTLS_GW::instance()->MA_Map.value(ma_id)->tag_map[tag_id]=new QMultiMap <int,tag_info>;//新建一个tag值
                RTLS_GW::instance()->MA_Map.value(ma_id)->tag_map[tag_id]=new QList <tag_info > ;//新建一个list
                RTLS_GW::instance()->tag_seq[tag_id]=seq_num;
                RTLS_GW::instance()->tag_p_map[tag_id]=new tag_tracking();
            }
            break;
        }


        default:
            break;
        }

        QList<QByteArray>::iterator temp = it++;
        recvBuffer[!recv_index].erase(temp);
    }
}


void S_Anchor::get_disa2ma() //计算主基站到副基站的距离
{
    dis_a2ma= sqrt((x-RTLS_GW::instance()->MA_Map.value(ma_id)->x)*(x-RTLS_GW::instance()->MA_Map.value(ma_id)->x)\
                   +(y-RTLS_GW::instance()->MA_Map.value(ma_id)->y)*(y-RTLS_GW::instance()->MA_Map.value(ma_id)->y)\
                   +(z-RTLS_GW::instance()->MA_Map.value(ma_id)->z)*(z-RTLS_GW::instance()->MA_Map.value(ma_id)->z));

}
