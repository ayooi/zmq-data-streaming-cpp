//
// Created by arans on 4/06/2021.
//

#ifndef ZMQ_DATA_STREAM_CPP_WRITERCONNECTIONDETAILS_H
#define ZMQ_DATA_STREAM_CPP_WRITERCONNECTIONDETAILS_H

#include <string>
#include <stdexcept>

enum WriteConnectionType {
    TCP, INPROC, IPC
};

struct WriteConnectionDetail {
    WriteConnectionType type;
    std::string address;
    long listenPort;
};

static WriteConnectionDetail parse(const std::string &url) {
    std::size_t npos = url.find_first_of(':');
    if (npos == std::string::npos) {
        throw std::invalid_argument(url + " does not contain at least one :");
    }
    auto protocolString = url.substr(0, npos + 3);
    WriteConnectionType type;
    if (protocolString == "tcp://") {
        type = TCP;
        size_t addressStart = npos + 3;
        std::size_t lastPos = url.find(':', addressStart);
        if (lastPos == std::string::npos) {
            throw std::invalid_argument(url + " does not specify a listen port");
        }
        std::string address = url.substr(addressStart, (lastPos - addressStart));
        auto portString = url.substr(lastPos + 1, url.length());
        long port;
        try {
            port = std::stoi(portString);
        } catch (const std::exception &e) {
            throw std::invalid_argument(portString + " is not convertible to a number");
        }
        return WriteConnectionDetail{type, address, port};
    } else if (protocolString == "inproc://") {
        type = INPROC;
        size_t addressStart = npos + 3;
        std::string address = url.substr(addressStart, url.length());
        return WriteConnectionDetail{type, address, 0};
    } else if (protocolString == "ipc://") {
        type = IPC;
        size_t addressStart = npos + 3;
        std::string address = url.substr(addressStart, url.length());
        return WriteConnectionDetail{type, address, 0};
    } else {
        throw std::invalid_argument(protocolString + " does match tcp://, inproc://, or ipc://");
    }
}

#endif //ZMQ_DATA_STREAM_CPP_WRITERCONNECTIONDETAILS_H
