#include "rtls_gw.h"
#include "net_if.h"
#include "QDateTime"
#include <QNetworkInterface>

#include "hw_id.h"


RTLS_GW::RTLS_GW(int argc, char *argv[]) : QCoreApplication(argc, argv)
{
//    if (check_sn()!=33)
//    {
//        qDebug("未注册版本，请联系暚光科技技术支持人员，www.yaoguang.net");
//        return ;
//    }
    QDateTime _time = QDateTime::currentDateTime();//获取系统现在的时间

    loadConfigFile("./anc_config.xml");
    _net_if=new net_if();
    _net_if->start_rp_ch(40000);
    connect(&HP_timer, SIGNAL(timeout()), this, SLOT(check_HP()));
    HP_timer.start(1*10*1000); //如果30秒没有收到基站数据，则检查是否收到数据，如果1分钟以后还没有收到数据则报告错误;


    // location_timer.start(50);//100msyici

}


RTLS_GW * RTLS_GW::instance()
{
    // return qobject_cast<RTLS_GW *>(self);
    return qobject_cast<RTLS_GW *>(QCoreApplication::instance());
}


void RTLS_GW::saveConfigFile(QString filename, QString cfg)
{

}

//读取基站配置文件
void RTLS_GW::loadConfigFile(QString filename)
{
    QFile file(filename);

    if (!file.open(QFile::ReadWrite | QFile::Text))
    {
        qDebug(qPrintable(QString("Error: Cannot read config file %1 %2").arg(filename).arg(file.errorString())));
        return;
    }

    QDomDocument doc;

    doc.setContent(&file, false);

    QDomElement root = doc.documentElement();

    qDebug("\n\n \t\t深圳暚光科技有限公司 实时定位系统\n");
    qDebug("\t \t\twww.yaoguang.net\n");
    printf("______________________________________________________\n");

    if( root.nodeName() == "YG_RTLS" )
    {
        QDomNodeList xml_list = root.childNodes();
        //  QDomNode master_anchor =xml_list.item(0);
        //        if(master_anchor.isNull())
        //            return;
        if (xml_list.item(0).nodeName()!="M_Anchor")//处理主基站
        {
            qDebug()<<"no master_anchor fond in config file";
            return;
        }else
        {
            QDomNode n = xml_list.item(0).firstChild();
            while( !n.isNull() )
            {
                QDomElement e = n.toElement();
                if( !e.isNull() )
                {
                    if( e.tagName() == "anc" )
                    {
                        bool ok;
                        M_Anchor *m_anchor=new M_Anchor();
                        int id = (e.attribute( "ID", "" )).toInt(&ok);
                        if(ok)
                        {
                            m_anchor->ID=id;
                            m_anchor->x=(e.attribute("x", "0.0")).toDouble(&ok);
                            m_anchor->y=(e.attribute("y", "0.0")).toDouble(&ok);
                            m_anchor->z=(e.attribute("z", "0.0")).toDouble(&ok);
                            m_anchor->port=(e.attribute("port", "80")).toUInt(&ok);
                            m_anchor->IP=(e.attribute("IP", ""));
                            m_anchor->rect[0]=(e.attribute("rect_x0", "0")).toInt(&ok);
                            m_anchor->rect[1]=(e.attribute("rect_x1", "0")).toInt(&ok);
                            m_anchor->rect[2]=(e.attribute("rect_y0", "0")).toInt(&ok);
                            m_anchor->rect[3]=(e.attribute("rect_y1", "0")).toInt(&ok);
                            m_anchor->start();
                            MA_Map[id]=m_anchor;//增加主基站的地址
                            qDebug("|Master Anchor | ID :%2d |x : %lf |y : %lf|z : %lf |port : %d |",id, m_anchor->x, m_anchor->y, m_anchor->z, m_anchor->port);
                            // qDebug()<<"Master Anchor:"
                        }
                    }
                }

                n = n.nextSibling();
            }
        }

        //  QDomNode master_anchor =xml_list.item(0);
        if (xml_list.item(1).nodeName()!="S_Anchor")//处理主基站
        {
            qDebug()<<"no slave_anchor fond in config file";
            return;
        }else
        {
            S_Anchor *s_anchor;
            QDomNode n = xml_list.item(1).firstChild();
            while( !n.isNull() )
            {
                QDomElement e = n.toElement();
                if( !e.isNull() )
                {
                    if( e.tagName() == "anc" )
                    {
                        bool ok;
                        s_anchor=new S_Anchor();
                        int id = (e.attribute( "ID", "" )).toInt(&ok);
                        if(ok)
                        {
                            s_anchor->ID=id;
                            s_anchor->x=(e.attribute("x", "0.0")).toDouble(&ok);
                            s_anchor->y=(e.attribute("y", "0.0")).toDouble(&ok);
                            s_anchor->z=(e.attribute("z", "0.0")).toDouble(&ok);
                            s_anchor->port=(e.attribute("port", "80")).toUInt(&ok);
                            s_anchor->IP=(e.attribute("IP", ""));
                            int ma_id=(e.attribute("ma_ID", "0")).toUInt(&ok);
                            //                          int have_ma_id=0;
                            //                          for(int i;i<MA_Map.count()-1;i++)
                            //                            {
                            //                              if MA_Map.value()
                            //                            }
                            if(!MA_Map.contains(ma_id))
                            {
                                qDebug()<<"slave anchor "<<id<<"can not find master anchor "<<ma_id;
                                continue;
                            }
                            s_anchor->ma_id=ma_id;
                            s_anchor->start();
                            SA_Map[id]=s_anchor;//增加副基站的地址
                            printf("|Slave  Anchor | ID :%d |x : %lf |y : %lf | z : %lf |port : %d |\n",id,s_anchor->x,s_anchor->y,s_anchor->z,s_anchor->port);
                        }
                    }
                }

                n = n.nextSibling();
            }
        }

    }else
    {
        qDebug("config file err");
    }


    file.close();

}


void RTLS_GW::check_HP()
{
    //遍历所有的基站

    QMap <quint32,M_Anchor *>::iterator it;
    for (it =MA_Map.begin();it!=MA_Map.end();++it)
    {
//        if(it.value()->health_Point<0)
//        {
//            qDebug()<<it.value()->ID<<"lost";
//        }
//        else
//        {
//            it.value()->health_Point--;
//        }
        it.value()->dispatch();
    }
    QMap <quint32,S_Anchor *>::iterator its;
    for (its=SA_Map.begin();its!=SA_Map.end();++its)
    {
//        if(its.value()->health_Point<0)
//        {
//            qDebug()<<its.value()->ID<<"lost";
//        }
//        else
//        {
//            its.value()->health_Point--;
//        }
        it.value()->dispatch();
    }

}


int RTLS_GW::check_sn()
{

    HW_ID hw_id;
    return hw_id.check_sn();

}
