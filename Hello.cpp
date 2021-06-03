//
// Created by arans on 3/06/2021.
//
#include <iostream>
#include <zmq.h>
#include <thread>
#include <list>
#include <set>

using namespace std::chrono_literals;

class DataServiceReader {
public:
    DataServiceReader(const std::string &uniqueName,
                      const std::string &serviceName,
                      void *ctx,
                      const std::string &serviceLocatorUrl)
            : _serviceName(serviceName),
              _running(true) {
        _locatorSocket = zmq_socket(ctx, ZMQ_DEALER);
        zmq_setsockopt(_locatorSocket, ZMQ_IDENTITY, &uniqueName, uniqueName.length());
        long timeout = 5;
        zmq_setsockopt(_locatorSocket, ZMQ_SNDTIMEO, &timeout, sizeof(timeout));
        zmq_connect(_locatorSocket, serviceLocatorUrl.c_str());

        _controlSocket = zmq_socket(ctx, ZMQ_PUSH);
        auto internalControlUrl = "inproc://" + uniqueName;
        zmq_bind(_controlSocket, internalControlUrl.c_str());

        _dataSocket = zmq_socket(ctx, ZMQ_PULL);
        zmq_connect(_dataSocket, internalControlUrl.c_str());

        _thread = std::thread([&]() {
            while (_running) {

            }
        });
    }

    ~DataServiceReader() {
        zmq_close(_controlSocket);
        zmq_close(_dataSocket);
        zmq_close(_locatorSocket);
    }

private:
    void *_locatorSocket;
    void *_dataSocket;
    void *_controlSocket;

    const std::string _serviceName;
    std::thread _thread;
    bool _running;

    std::list<uint8_t[]> _payloads;
    std::set<std::string> _locations;

};

class DataServiceWriter {
public:

    DataServiceWriter(const std::string &serviceName,
                      const std::string &dataUrl,
                      void *ctx,
                      const std::string &serviceLocatorUrl)
            : _running(true),
              _serviceName(serviceName),
              _dataUrl(dataUrl) {
        _dataSocket = zmq_socket(ctx, ZMQ_PUSH);
        long val = 1000;
        zmq_setsockopt(_dataSocket, ZMQ_SNDHWM, &val, sizeof(long));
        zmq_bind(_dataSocket, dataUrl.c_str());

        _serviceSocket = zmq_socket(ctx, ZMQ_DEALER);
        zmq_setsockopt(_serviceSocket, ZMQ_IDENTITY, &dataUrl, dataUrl.length());
        zmq_connect(_serviceSocket, serviceLocatorUrl.c_str());

        _thread = std::thread([&]() {
            while (_running) {
                std::string command = "register";
                zmq_send(_serviceSocket, command.c_str(), command.length(), ZMQ_SNDMORE);
                zmq_send(_serviceSocket, _serviceName.c_str(), _serviceName.length(), ZMQ_SNDMORE);
                zmq_send(_serviceSocket, _dataUrl.c_str(), _dataUrl.length(), 0);
                std::this_thread::sleep_for(5s);
            }
        });
    }

    ~DataServiceWriter() {
        _running = false;
        {
            std::string command = "deregister";
            zmq_send(_serviceSocket, command.c_str(), command.length(), ZMQ_SNDMORE);
            zmq_send(_serviceSocket, _serviceName.c_str(), _serviceName.length(), ZMQ_SNDMORE);
            zmq_send(_serviceSocket, _dataUrl.c_str(), _dataUrl.length(), 0);
        }
        zmq_close(_serviceSocket);
        zmq_close(_dataSocket);
        _thread.join();
    }

    void write(const uint8_t *payload, const size_t length) {
        zmq_send(_dataSocket, payload, length, 0);
    }

private:
    void *_dataSocket;
    void *_serviceSocket;

    std::thread _thread;
    bool _running;
    const std::string _dataUrl;
    const std::string _serviceName;
};

void simple_test() {
    std::cout << "Hello World!" << std::endl;
    auto ctx = zmq_ctx_new();
    auto push = zmq_socket(ctx, ZMQ_PUSH);
    zmq_bind(push, "inproc://testest");

    auto pull = zmq_socket(ctx, ZMQ_PULL);
    zmq_connect(pull, "inproc://testest");

    {
        zmq_send(push, "Hello", 5, 0);
    }

    {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        zmq_recvmsg(pull, &msg, 0);
        std::cout << std::string(static_cast<char *>(zmq_msg_data(&msg)), zmq_msg_size(&msg)) << std::endl;
    }

    zmq_close(push);
    zmq_close(pull);
    zmq_ctx_destroy(ctx);
}

int main(int argc, char **argv) {
//    simple_test();

    auto ctx = zmq_ctx_new();
    std::string serviceName = "service-name";
    std::string dataUrl = "tcp://localhost:15000";
    std::string serviceLocatorUrl = "tcp://localhost:19999";

    DataServiceWriter writer(serviceName, dataUrl, ctx, serviceLocatorUrl);

    while (true) {
        std::this_thread::sleep_for(5s);
    }

    return 0;
}