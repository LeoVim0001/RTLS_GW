#include "m_anchor.h"
#include "iostream"
#include "rtls_gw.h"
M_Anchor::M_Anchor(QObject *parent) : QThread(parent), recv_index(false)
{
    health_Point=2;
}

M_Anchor::~M_Anchor()
{
    delete udpSocket;
}

void M_Anchor::run()
{
    tag_data.Anchor_ID=ID;
    tag_data.x=x;
    tag_data.y=y;
    tag_data.z=z;

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(port, QUdpSocket::ShareAddress| QUdpSocket::ReuseAddressHint ); //绑定端口
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(read_UDP()), Qt::DirectConnection);

    exec();
}

void M_Anchor::swap_buffer()
{
    bufferLocker.lock();
    recv_index =!recv_index;
    recvBuffer[!recv_index].swap(recvBuffer[recv_index]);
    bufferLocker.unlock();
}

quint64 m_old;
//UDP接收到数据以后回调函数
void M_Anchor::read_UDP()
{

    bufferLocker.lock();
    // quint16 r_cmd;
    //hasPendingDatagrams返回true时表示至少有一个数据包在等待被读取
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

void M_Anchor::dispatch()
{
    swap_buffer();

    QList<QByteArray>::iterator it = recvBuffer[!recv_index].begin();
    while(it != recvBuffer[!recv_index].end())
    {
        char *pbuf;
        pbuf=(*it).data();
        health_Point=2;
    //    if(pbuf[0]!=0x59)
    //        return;
    //    pbuf=pbuf+3;

        if(pbuf[0]!='Y')
        {
            qDebug("derr");
        }
        health_Point=2;
        switch (pbuf[3]) {
        case master_ccp:
        {
            m_ccptopc_t *m_ccptopc;
            m_ccptopc=(m_ccptopc_t *)pbuf;
            //        if (m_ccptopc->M_ID!=ID)
            //          {
            //            qDebug("err");
            //            return;
            //          }
            SeqNum=m_ccptopc->seq_num;
            txtimer=m_ccptopc->tick*TIME_UNITS;//
            //        diff_tick=m_ccptopc->tick-old_tick;
            //        if (diff_tick>2e39) //如果翻转
            //            diff_tick=2e40-diff_tick;
            //  qDebug()<<ID<<SeqNum<<txtimer;
            //         old_tick=m_ccptopc->tick;
    //          qDebug()<<ID<<SeqNum<<m_ccptopc->tick<<(qint64 )m_ccptopc->tick-m_old;//<<m_ccptopc->seq_num;
            m_old=m_ccptopc->tick;
            break;
        }
        case fc_an_tag:
        {
            tagtopc_t *tagtopc;
            tagtopc=(tagtopc_t *)pbuf;

            int tag_id=tagtopc->TAG_ID;
            int seq_num=tagtopc->seq_num;

            if (tag_map.contains(tag_id))  //如果已经有了map,则直接增加map.否则创建map;
            {
                if(RTLS_GW::instance()->tag_seq.value(tag_id)!=seq_num)
                {
                    RTLS_GW::instance()->tag_seq[tag_id]=seq_num;//更新seqnum
                    tag_map.value(tag_id)->clear();
                }
              //  qDebug()<<ID<<tagtopc->TAG_ID<<tagtopc->seq_num<<tagtopc->rssi_first;
                tag_data.rssi_all=tagtopc->rssi_all;
                tag_data.seq_num=tagtopc->seq_num;
                tag_data.rssi_first=tagtopc->rssi_first;
                tag_data.tickes=(tagtopc->tick-2*16405)*4.69035687;//2*16405
                tag_map.value(tag_id)->append(tag_data);//插入基站数据
                qDebug()<<ID<<tagtopc->TAG_ID<<tagtopc->seq_num<<tag_data.tickes;
                RTLS_GW::instance()->location_engine.proc_tag_location(ID,tag_id,seq_num);
            }else
            {
                //RTLS_GW::instance()->Anchor_map[RTLS_GW::instance()->tag_cnt]=new QMultiMap <int,tag_info> ;
                tag_map[tag_id]=new QList <tag_info > ;//新建一个list
                RTLS_GW::instance()->tag_seq[tag_id]=seq_num;
                RTLS_GW::instance()->tag_p_map[tag_id]=new tag_tracking();
            }
            //  qDebug()<<ID<<tagtopc->TAG_ID<<tagtopc->seq_num<<tag_data.tickes;
            break;
        }
        default:
            break;
        }

       QList<QByteArray>::iterator temp = it++;
       recvBuffer[!recv_index].erase(temp);
    }


}

