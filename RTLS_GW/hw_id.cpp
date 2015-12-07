#include "hw_id.h"
#include <stdio.h>
#include <QNetworkInterface>
#include <QFile>
#include "aeshelper.h"

// intrinsics
#if defined(__GNUC__)	// GCC
#include <cpuid.h>
#elif defined(_MSC_VER)	// MSVC
#if _MSC_VER >=1400	// VC2005
#include <intrin.h>
#endif	// #if _MSC_VER >=1400
#else
#error Only supports MSVC or GCC.
#endif	// #if defined(__GNUC__)

void getcpuidex(unsigned int CPUInfo[4], unsigned int InfoType, unsigned int ECXValue)
{
#if defined(__GNUC__)	// GCC
    __cpuid_count(InfoType, ECXValue, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER)	// MSVC
#if defined(_WIN64) || _MSC_VER>=1600	// 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.
    __cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
#else
    if (NULL==CPUInfo)	return;
    _asm{
        // load. 读取参数到寄存器.
        mov edi, CPUInfo;	// 准备用edi寻址CPUInfo
        mov eax, InfoType;
        mov ecx, ECXValue;
        // CPUID
        cpuid;
        // save. 将寄存器保存到CPUInfo
        mov	[edi], eax;
        mov	[edi+4], ebx;
        mov	[edi+8], ecx;
        mov	[edi+12], edx;
    }
#endif
#endif	// #if defined(__GNUC__)
}

void getcpuid(unsigned int CPUInfo[4], unsigned int InfoType)
{
#if defined(__GNUC__)	// GCC
    __cpuid(InfoType, CPUInfo[0],CPUInfo[1],CPUInfo[2],CPUInfo[3]);
#elif defined(_MSC_VER)	// MSVC
#if _MSC_VER>=1400	// VC2005才支持__cpuid
    __cpuid((int*)(void*)CPUInfo, (int)InfoType);
#else
    getcpuidex(CPUInfo, InfoType, 0);
#endif
#endif	// #if defined(__GNUC__)
}


//// 取得CPU序列号（ID）.
////
//// result: 成功时返回字符串的长度（一般为12）。失败时返回0.

void HW_ID::cpu_getid()
{
    unsigned int dwBuf[4];
    unsigned long long ret ;
    //if (NULL == pvendor)	return 0;
    // Function 0: Vendor-ID and Largest Standard Function
    getcpuid(dwBuf, 1);
    ret = dwBuf[3];	// ebx: 序列号后几位.
    ret = ret << 32;
    cpu_id=QString::number(dwBuf[3],16).toUpper();
    cpu_id= cpu_id+QString::number(dwBuf[0],16).toUpper();
     //  qDebug()<<cpu_id;
    //        printf("%x ",dwBuf[0]);
    //        printf("%x ",dwBuf[1]);
    //        printf("%x ",dwBuf[2]);
    //        printf("%x ",dwBuf[3]);
}




void HW_ID::get_mac()
{
    QString  strMac;
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();//获取所有网卡信息
    for (int i = 0; i < ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        if (/* iface.flags().testFlag(QNetworkInterface::IsUp) &&iface.flags().testFlag(QNetworkInterface::IsRunning) && */ !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
        { //获取当前有效网卡//获取所有网卡的地址
            for (int j=0; j<iface.addressEntries().count()-1; j++) //这个循环很重要，读者可以尝试不用这个循环看看获取的是不是当前有效网卡：）
            {
                /* we have an interface that is up, and has an ip address
                         * therefore the link is present
                         * we will only enable this check on first positive,
                         * all later results are incorrect * * */
                strMac = iface.hardwareAddress();
              //  qDebug()<<strMac;
                mac_list.append(strMac);
                //  i = ifaces.count(); //跳出外层循环
                // break;
            }
        }
    }
}








HW_ID::HW_ID()
{

}

int HW_ID::check_sn()
{
    int mac_match=0;
    int cpuid_match=0;
    QFile file("RTLS_GW.lic");

    if (!file.open(QFile::ReadWrite | QFile::Text))
    {

        return 0;
    }
    QString sn_file = QString(file.readAll());
    if ((sn_file.length()>10230)||(sn_file.length()<10))
    {
        return 0;
    }
    AesHelper aes_str;
    QString un_sn= aes_str.aesUncrypt(sn_file);
  //  qDebug()<< aes_str.aesEncrypt(sn_file);

    QStringList sn_list = un_sn.split(";");
    if (sn_list.count()<4)
        return 1;
    if (!(sn_list.at(1)=="YAOGUNG_RTLS"))
        return 2;
    QString sn_mac=sn_list.at(2);

    mac_list.clear();
    get_mac();
    for(int i=0;i<mac_list.count();i++)
    {
        if(mac_list.at(i)==sn_mac)
        {
            mac_match=1;
        }
    }
    QString sn_cpuid=sn_list.at(3);

    cpu_getid();
    if(sn_cpuid==cpu_id)
        cpuid_match=1;
    if(mac_match|cpuid_match)
        return 33;
    else return 0;

}
