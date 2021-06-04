//
// Created by arans on 3/06/2021.
//

#include "DataServiceWriter.h"
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

DataServiceWriter::DataServiceWriter(std::string serviceName,
                                     const WriteConnectionDetail &connectionDetail,
                                     void *ctx,
                                     const std::string &serviceLocatorUrl)
        : _running(true),
          _serviceName(std::move(serviceName)) {
    _dataSocket = zmq_socket(ctx, ZMQ_PUSH);
    long val = 1000;
    zmq_setsockopt(_dataSocket, ZMQ_SNDHWM, &val, sizeof(long));

    if (connectionDetail.type == IPC) {
        _dataBindUrl = "ipc://" + connectionDetail.address;
        _dataAnnouncementUrl = "ipc://" + connectionDetail.address;
    } else if (connectionDetail.type == INPROC) {
        _dataBindUrl = "inproc://" + connectionDetail.address;
        _dataAnnouncementUrl = "inproc://" + connectionDetail.address;
    } else if (connectionDetail.type == TCP) {
        _dataBindUrl = "tcp://*:" + std::to_string(connectionDetail.listenPort);
        _dataAnnouncementUrl = "tcp://" + connectionDetail.address + ":" + std::to_string(connectionDetail.listenPort);
    } else {
        throw std::invalid_argument("Unsupported protocol type " + std::to_string(connectionDetail.type));
    }
    zmq_bind(_dataSocket, _dataBindUrl.c_str());
    std::cout << "Binding to " << _dataBindUrl << std::endl;

    _serviceSocket = zmq_socket(ctx, ZMQ_DEALER);
    zmq_setsockopt(_serviceSocket, ZMQ_IDENTITY, &_dataAnnouncementUrl, _dataAnnouncementUrl.length());
    zmq_connect(_serviceSocket, serviceLocatorUrl.c_str());

    _thread = std::thread([&]() {
        while (_running) {
            std::string command = "register";
            zmq_send(_serviceSocket, command.c_str(), command.length(), ZMQ_SNDMORE);
            zmq_send(_serviceSocket, _serviceName.c_str(), _serviceName.length(), ZMQ_SNDMORE);
            zmq_send(_serviceSocket, _dataAnnouncementUrl.c_str(), _dataAnnouncementUrl.length(), 0);
            std::this_thread::sleep_for(5s);
        }
    });
}

DataServiceWriter::~DataServiceWriter() {
    _running = false;
    {
        std::string command = "deregister";
        zmq_send(_serviceSocket, command.c_str(), command.length(), ZMQ_SNDMORE);
        zmq_send(_serviceSocket, _serviceName.c_str(), _serviceName.length(), ZMQ_SNDMORE);
        zmq_send(_serviceSocket, _dataAnnouncementUrl.c_str(), _dataAnnouncementUrl.length(), 0);
    }
    zmq_close(_serviceSocket);
    zmq_close(_dataSocket);
    _thread.join();
}

void DataServiceWriter::write(const uint8_t *payload, const size_t length) {
    std::cout << "sending data out" << std::endl;
    zmq_send(_dataSocket, payload, length, 0);
}

