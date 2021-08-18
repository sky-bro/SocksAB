#include "address.h"

Address::Address() : m_addr_type(NOTYPE) {}

Address::Address(QHostAddress addr, quint16 port) {
    update_from_ip_port(addr, port);
}

Address::Address(const std::string &data) { update_from_data(data); }

bool Address::update_from_ip_port(QHostAddress addr, quint16 port) {
    std::string portNs(2, '\0');
    qToBigEndian(port, reinterpret_cast<uchar *>(&portNs[0]));
    if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
        m_addr = addr;
        m_port = port;
        m_addr_type = IPV4;
        uint32_t ipv4Address = qToBigEndian(addr.toIPv4Address());
        m_data = char(IPV4) +
                 std::string(reinterpret_cast<char *>(&ipv4Address), 4) +
                 portNs;
        return true;
    } else if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
        m_addr = addr;
        m_port = port;
        m_addr_type = IPV6;
        m_data =
            char(IPV6) +
            std::string(reinterpret_cast<char *>(addr.toIPv6Address().c), 16) +
            portNs;
        return true;
    }
    m_addr_type = NOTYPE;
    return false;
}

int Address::update_from_data(const std::string &data) {
    qInfo() << "parsing address from data:" << QByteArray(data.data());
    int t_addr_type = data[0];
    if (t_addr_type == IPV4) {
        // ATYPE | IPV4 | PORT
        if (data.length() >= 7) {
            m_addr.setAddress(qFromBigEndian(
                *reinterpret_cast<const quint32 *>(data.data() + 1)));
            m_port = qFromBigEndian(
                *reinterpret_cast<const uint16_t *>(data.data() + 5));
            m_addr_type = IPV4;
            m_data = data.substr(0, 7);
            qInfo() << "IPV4: " << m_addr << ":" << m_port;
            return 7;
        }
    } else if (t_addr_type == IPV6) {
        // ATYPE | IPV6 | PORT
        if (data.length() >= 19) {
            m_addr.setAddress(data.data() + 1);
            m_port = qFromBigEndian(
                *reinterpret_cast<const uint16_t *>(data.data() + 17));
            m_addr_type = IPV6;
            m_data = data.substr(0, 19);
            qInfo() << "IPV6: " << m_addr << ":" << m_port;
            return 19;
        }
    } else if (t_addr_type == HOST) {
        // ATYPE | HOSTNAME_LEN | HOSTNAME | PORT
        if (data.length() > 2) {
            uint8_t hostname_len = data[1];
            if (data.length() >= 2ul + hostname_len) {
                m_port = qFromBigEndian(*reinterpret_cast<const uint16_t *>(
                    data.data() + 2 + hostname_len));
                m_hostname = QString(data.substr(2, hostname_len).data());
                qInfo() << "hostname_len: " << hostname_len
                        << ", m_hostname: " << m_hostname
                        << ", m_port: " << m_port;
                m_addr_type = HOST;
                m_data = data.substr(0, 4 + hostname_len);
                return 4 + hostname_len;
            }
        }
    }
    m_addr_type = NOTYPE;
    return 0;
}
