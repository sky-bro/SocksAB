#ifndef ADDRESS_H
#define ADDRESS_H

#include <QHostAddress>
#include <QtEndian>
#include <iostream>

class Address {
  public:
    enum ATYP { IPV4 = 1, NOTYPE = 2, HOST = 3, IPV6 = 4 };
    ATYP m_addr_type;
    QHostAddress m_addr;
    QString m_hostname;
    quint16 m_port;
    // ATYP | IPv4/IPv6 | PORT
    // ATYP | HOSTNAME_LEN | HOSTNAME | PORT
    std::string m_data;
    Address();
    Address(QHostAddress addr, quint16 port);
    Address(const std::string &data);

    bool update_from_ip_port(QHostAddress addr, quint16 port);

    int update_from_data(const std::string &data);

    friend QDebug operator<<(QDebug os, const Address &address) {
        if (address.m_addr_type == IPV4 || address.m_addr_type == IPV6) {
            return os << address.m_addr.toString() + ":" +
                             QString::number(address.m_port);
        } else if (address.m_addr_type == HOST) {
            return os << address.m_hostname + ":" +
                             QString::number(address.m_port);
        }
        return os << "NOADDR";
    }
};

#endif  // ADDRESS_H
