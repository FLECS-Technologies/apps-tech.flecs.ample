// MIT License
//
// Copyright (c) 2024 FLECS Technologies GmbH
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

#include "flunder/client.h"

namespace {
std::atomic_bool g_stop = false;
}

static auto signal_handler(
    int) //
    -> void
{
    g_stop = true;
}

namespace flecs {
auto flunder_receive_callback(
    flunder::client_t* client,
    const flunder::variable_t* var) //
    -> void
{
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::fprintf(
        stdout,
        "Received flunder message for topic %s on client %p with length "
        "%zu @%" PRIi64 "\n",
        var->topic().data(),
        client,
        var->len(),
        now);

    if (var->topic() == "/flecs/flunder/cpp/int") {
        const auto i = std::atoll(var->value().data());
        std::fprintf(stdout, "\tValue: %lld\n", i);
    } else if (var->topic() == "/flecs/flunder/cpp/double") {
        const auto d = std::atof(var->value().data());
        std::fprintf(stdout, "\tValue: %lf\n", d);
    } else if (var->topic() == "/flecs/flunder/cpp/string") {
        std::fprintf(stdout, "\tValue: %s\n", var->value().data());
    } else if (var->topic() == "/flecs/flunder/cpp/timestamp") {
        const auto t1 = std::stoll(var->value().data());
        const auto diff = now - t1;
        std::fprintf(stdout, "\tMessage sent @%lld (%lld ns ago)\n", t1, diff);
    }
}

auto flunder_receive_callback_userp(
    flunder::client_t* client,
    const flunder::variable_t* var,
    const void* userp) //
    -> void
{
    const auto timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::fprintf(
        stdout,
        "Received flunder message for topic %s on client %p with length "
        "%zu and userdata %s "
        "@%" PRIi64 "\n",
        var->topic().data(),
        client,
        var->len(),
        static_cast<const char*>(userp),
        timestamp);
}

auto exec_example_loop() //
    -> int
{
    auto flunder_client = flunder::client_t{};

    do {
        if (flunder_client.connect() == -1) {
            std::fprintf(stdout, "Connection could not be established, waiting 5s for reconnect.\n");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }
        std::fprintf(stdout, "Connection was established.\n");

        flunder_client.add_mem_storage("flunder-cpp", "/flecs/flunder/**");

        flunder_client.subscribe("/flecs/flunder/cpp/**", &flunder_receive_callback);
        const char* userdata = "Hello, world!";
        flunder_client.subscribe(
            "/flecs/flunder/external",
            &flunder_receive_callback_userp,
            static_cast<const void*>(userdata));

        while (!g_stop && flunder_client.is_connected()) {
            constexpr auto i = 1234;
            flunder_client.publish("/flecs/flunder/cpp/int", i);

            constexpr auto d = 3.14159;
            flunder_client.publish("/flecs/flunder/cpp/double", d);

            constexpr auto str = "Hello, world!";
            flunder_client.publish("/flecs/flunder/cpp/string", str);

            const auto t = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            flunder_client.publish("/flecs/flunder/cpp/timestamp", t);

            std::this_thread::sleep_for(std::chrono::seconds(5));
        };

        flunder_client.unsubscribe("/flecs/flunder/cpp/**");
        flunder_client.unsubscribe("/flecs/flunder/external");
        flunder_client.disconnect();
    } while (!g_stop);

    return 0;
}
} // namespace flecs

auto main() //
    -> int
{
    auto sa = (struct sigaction){};
    sa.sa_handler = &signal_handler;
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);

    return flecs::exec_example_loop();
}
