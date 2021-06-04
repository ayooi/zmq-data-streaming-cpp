//
// Created by arans on 3/06/2021.
//
#include <iostream>
#include <zmq.h>
#include <thread>
#include <list>
#include <set>
#include <vector>
#include "DataServiceWriter.h"
#include "DataServiceReader.h"
#include "WriterConnectionDetail.h"

using namespace std::chrono_literals;

void simple_test() {
    std::cout << "Hello World!" << std::endl;
    auto ctx = zmq_ctx_new();
    auto push = zmq_socket(ctx, ZMQ_PUSH);
    zmq_bind(push, "inproc://testest");

    auto pull = zmq_socket(ctx, ZMQ_PULL);
    zmq_connect(pull, "inproc://testest");
    std::string payload = "Hello";
    auto s = reinterpret_cast<const uint8_t *>(payload.c_str());
    {
        zmq_send(push, s, payload.length(), 0);
    }

    {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        zmq_msg_recv(&msg, pull, 0);
        std::cout << std::string(static_cast<char *>(zmq_msg_data(&msg)), zmq_msg_size(&msg)) << std::endl;
        zmq_close(&msg);
    }

    zmq_close(push);
    zmq_close(pull);
    zmq_ctx_destroy(ctx);
}

int main(int argc, char **argv) {
    auto ctx = zmq_ctx_new();
    std::string serviceName = "service-name";
    std::string dataUrl = "tcp://localhost:15001";
    std::string serviceLocatorUrl = "tcp://localhost:19999";

    DataServiceWriter writer(serviceName, parse(dataUrl), ctx, serviceLocatorUrl);
    DataServiceReader reader("random-name",
                             serviceName,
                             ctx,
                             serviceLocatorUrl);

    std::string payload = "payload";

    for (int i = 0; i < 5; i++) {
        auto s = reinterpret_cast<const uint8_t *>(payload.c_str());
        writer.write(s, payload.length());
    }
    std::this_thread::sleep_for(1s);

    std::list<std::vector<uint8_t>> vec;
    reader.takeAll(vec);
    for (const auto &item : vec) {
        std::cout << std::string(item.data(), item.data() + item.size()) << std::endl;
    }
    return 0;
}