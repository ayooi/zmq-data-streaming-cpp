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