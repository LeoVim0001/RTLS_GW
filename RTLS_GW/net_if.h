#ifndef NET_IF_H
#define NET_IF_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>


//网络接口类,负责和上层服务器的接口
//实现了两个接口40000上报接口rp
//40001 控制接口 通过jason控制数据






class net_if : public QObject
{
	Q_OBJECT
public:
//	friend class RTLS_GW;
    net_if(void);
    ~net_if(void);
	int rp_port;
	int ctrl_port;
	int start_rp_ch(quint16 port);
	int start_ctrl_ch(quint16 port);
	void stop_rp_ch(void);
	void stop_ctrl_ch(void);
	int send_rp_data(char * pbuf, quint32 length);
	int send_ctrl_data(char * pbuf, quint32 length);	
public:
	QByteArray ctrl_da;
    int is_rp_con; //rp 上报端口是否有连接标识
    int is_ctrl_con; //控制端口是否有连接标识
private:
	QTcpServer * rp_tcpServer;  //用来发送结果报文的socker  现在单线程,以后多线程
	QTcpSocket * rp_tcpSocket;//配合tcpserver

	QTcpServer * ctrl_tcpServer;  //实现控制功能的socket
	QTcpSocket * ctrl_tcpSocket;//配合tcpserver
 signals:
    void parse_api(void);
	public slots:
		void new_ctrl_Connect(void);
		void new_rp_Connect(void);
		void rp_data_recv(void);
		void ctrl_data_recv(void);
		void rp_socker_Error(QAbstractSocket::SocketError socketError);
		void ctrl_socker_Error(QAbstractSocket::SocketError socketError);
};

#endif
