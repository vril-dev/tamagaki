#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <unistd.h>

#include "src/control/runtime.hpp"
#include "src/control/server.hpp"
#include "src/ebpf/loader.hpp"
#include "src/ebpf/xdp.hpp"
#include "src/ids/baseline.hpp"
#include "src/ids/monitor.hpp"

namespace {

std::atomic_bool g_stop{false};

void onSignal(int)
{
    g_stop.store(true);
}

void runWAF(std::atomic_bool &stop_requested)
{
    while (!stop_requested.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int healthCheck(const tamagaki::control::Runtime &runtime)
{
    return runtime.healthy() ? 0 : 1;
}

void notifyBootSuccess()
{
    const int rc = std::system("/usr/libexec/tamagaki-notify-boot >/dev/null 2>&1");
    if (rc != 0) {
        std::cerr << "tamagaki: boot success notification skipped (rc=" << rc << ")\n";
    }
}

}  // namespace

int main(int argc, char **argv)
{
    std::string config_path = "/config/tamagaki.yaml";
    std::string socket_path = "/tmp/tamagaki-control.sock";
    bool once = false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--config") == 0 && i + 1 < argc) {
            config_path = argv[++i];
        } else if (std::strcmp(argv[i], "--socket") == 0 && i + 1 < argc) {
            socket_path = argv[++i];
        } else if (std::strcmp(argv[i], "--once") == 0) {
            once = true;
        }
    }

    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    tamagaki::ebpf::Loader loader;
    tamagaki::control::Runtime runtime(loader, config_path);
    runtime.initialize();

    tamagaki::ebpf::XDPController xdp;
    tamagaki::ids::Baseline baseline("/config/baseline.json");
    baseline.load();

    tamagaki::ids::Monitor monitor(xdp, baseline);
    std::atomic_bool stop_requested{false};

    std::thread ids_thread([&]() { monitor.watch(stop_requested); });
    std::thread waf_thread([&]() { runWAF(stop_requested); });
    std::thread control_thread;
    std::optional<tamagaki::control::Server> server;

    if (!once) {
        server.emplace(runtime, xdp, socket_path);
        control_thread = std::thread([&]() { server->run(stop_requested); });
    }

    const int rc = healthCheck(runtime);
    if (rc == 0) {
        notifyBootSuccess();
    }

    if (once) {
        stop_requested.store(true);
        ids_thread.join();
        waf_thread.join();
        return rc;
    }

    while (!g_stop.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    stop_requested.store(true);
    ids_thread.join();
    waf_thread.join();
    control_thread.join();
    return rc;
}
