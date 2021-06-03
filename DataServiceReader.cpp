//
// Created by arans on 3/06/2021.
//

#include "DataServiceReader.h"

class DataServiceReader {
public:
    DataServiceReader(const std::string &uniqueName,
                      std::string serviceName,
                      void *ctx,
                      const std::string &serviceLocatorUrl)
            : _serviceName(std::move(serviceName)),
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

        _queryThread = std::thread([&]() {
            zmq_pollitem_t pollitems[] = {
                    {_locatorSocket, 0, ZMQ_POLLIN, 0}
            };
            std::string command = "query";
            while (_running) {
                zmq_send(_locatorSocket, command.c_str(), command.length(), ZMQ_SNDMORE);
                zmq_send(_locatorSocket, _serviceName.c_str(), _serviceName.length(), 0);

                int rc = zmq_poll(pollitems, 1, 10000);
                if (rc == -1) {
                    break;
                }
                std::set<std::string> locations;
                for (; pollitems[0].revents & ZMQ_POLLIN;
                       zmq_poll(pollitems, 1, 0)) {
                    zmq_msg_t msgs;
                    zmq_msg_init(&msgs);
                    zmq_msg_recv(&msgs, _locatorSocket, 0);
                    auto location = std::string(static_cast<char *>(zmq_msg_data(&msgs)), zmq_msg_size(&msgs));
                    locations.insert(location);
                    zmq_msg_close(&msgs);
                }
                doConnects(locations);
            }
        });

        _processingThread = std::thread([&]() {
            while (_running) {
                zmq_msg_t msg;
                zmq_msg_init(&msg);
                zmq_msg_recv(&msg, _dataSocket, 0);

                size_t size = zmq_msg_size(&msg);
                auto dataPtr = static_cast<uint8_t *>(zmq_msg_data(&msg));
                std::vector<uint8_t> payload(dataPtr, dataPtr + size);
                _payloads.push_back(payload);

                zmq_msg_close(&msg);
            }
        });
    }

    void doConnects(const std::set<std::string> &incoming) {
        std::list<std::string> toConnect;
        for (auto const &l: incoming) {
            if (_locations.count(l) == 0) {
                toConnect.push_back(l);
            }
        }

        std::list<std::string> toDisconnect;
        for (const auto &l : _locations) {
            if (incoming.count(l) == 0) {
                toDisconnect.push_back(l);
            }
        }

        if (!toDisconnect.empty() || !toConnect.empty()) {
            for (const auto &disconnect : toDisconnect) {
                zmq_disconnect(_dataSocket, disconnect.c_str());
                std::cout << "[Reader] Disconnecting from old provider " << disconnect << std::endl;
            }

            for (const auto &connect : toConnect) {
                zmq_connect(_dataSocket, connect.c_str());
                std::cout << "[Reader] Connecting to new provider " << connect << std::endl;
            }
            std::string ignoreMe = "###TheIgnoreMeMessage###";
            zmq_send(_controlSocket, ignoreMe.c_str(), ignoreMe.length(), 0);
        }
    }

    void takeAll(std::list<std::vector<uint8_t>> &res) {
        res.clear(); // paranoid
        res.swap(_payloads);
    }

    ~DataServiceReader() {
        _running = false;
        zmq_close(_controlSocket);
        zmq_close(_dataSocket);
        zmq_close(_locatorSocket);
        _queryThread.join();
        _processingThread.join();
    }

private:
    void *_locatorSocket;
    void *_dataSocket;
    void *_controlSocket;

    const std::string _serviceName;
    std::thread _queryThread;
    std::thread _processingThread;
    bool _running;

    std::list<std::vector<uint8_t>> _payloads;
    std::set<std::string> _locations;

};