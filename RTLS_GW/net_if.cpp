#include "net_if.h"


net_if::net_if(void)
{
	rp_port = 0;
	ctrl_port = 0;
    is_ctrl_con=0;
    is_rp_con=0;
	this->rp_tcpServer = new QTcpServer(this);
	this->ctrl_tcpServer=new QTcpServer(this);
}


net_if::~net_if(void)
{
	delete this->rp_tcpServer ;
	delete this->ctrl_tcpServer;
}


int net_if::start_rp_ch(quint16 port)
{
	rp_tcpServer->listen(QHostAddress::Any,port);
	connect(this->rp_tcpServer,SIGNAL(newConnection()),this,SLOT(new_rp_Connect()));
	return 0;
}


int net_if::start_ctrl_ch(quint16 port)
{
	ctrl_tcpServer->listen(QHostAddress::Any,port);
	connect(this->ctrl_tcpServer,SIGNAL(newConnection()),this,SLOT(new_ctrl_Connect()));
	return 0;
}

void net_if::stop_ctrl_ch()
{
    is_ctrl_con=0;
	if(this->ctrl_tcpSocket != NULL) {
		this->ctrl_tcpSocket->close();
	}
	if(this->ctrl_tcpSocket != NULL) {
		this->ctrl_tcpSocket->close();
	}
}


void net_if::stop_rp_ch()
{
    is_rp_con=0;
	if(this->ctrl_tcpSocket != NULL) {
		this->ctrl_tcpSocket->close();
	}
	if(this->ctrl_tcpSocket != NULL) {
		this->ctrl_tcpSocket->close();
	}
}



int net_if::send_rp_data(char * pbuf, quint32 length)
{
    if (this->is_rp_con)
    {
    this->rp_tcpSocket->write(pbuf,length);
    return 0;
    }
    return -1;
}


int net_if::send_ctrl_data(char * pbuf, quint32 length)
{
    if (this->is_ctrl_con)
    {
	this->ctrl_tcpSocket->write(pbuf,length);
	return 0;
    }
    return -1;
}


void net_if::rp_data_recv(void)
{
	//暂时不做任何处理
}


void net_if::ctrl_data_recv(void)
{
	ctrl_da=this->ctrl_tcpSocket->readAll();
	emit parse_api();
	
	//send_rp_data(ctrl_da.data(),ctrl_da.size());
}

//新的socket连接
void net_if::new_ctrl_Connect(void)
{
    if (is_ctrl_con) //如果已经连接则断开从新连接
    {
        this->ctrl_tcpSocket->disconnectFromHost();
    }
    this->ctrl_tcpSocket=this->ctrl_tcpServer->nextPendingConnection();
	connect(this->ctrl_tcpSocket,SIGNAL(readyRead()),this,SLOT(ctrl_data_recv()));
   // connect(this->ctrl_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(ctrl_socker_Error(QAbstractSocket::SocketError)));
	connect(this->ctrl_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(ctrl_socker_Error(QAbstractSocket::SocketError))); 
    is_ctrl_con=1;
}

//新的socket连接
void net_if::new_rp_Connect(void)
{
    if (is_rp_con) //如果已经连接则断开从新连接
    {
        this->rp_tcpSocket->disconnectFromHost();
    }
    this->rp_tcpSocket=this->rp_tcpServer->nextPendingConnection();
	connect(this->rp_tcpSocket,SIGNAL(readyRead()),this,SLOT(rp_data_recv()));
    connect(this->rp_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(rp_socker_Error(QAbstractSocket::SocketError)));
    is_rp_con=1;
}



void net_if::rp_socker_Error(QAbstractSocket::SocketError socketError)
{
    is_rp_con=0;
}


void net_if::ctrl_socker_Error(QAbstractSocket::SocketError socketError)
{
    is_ctrl_con=0;
}
