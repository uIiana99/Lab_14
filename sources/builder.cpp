#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>
#include <boost/process.hpp>

namespace bp = boost::process;
namespace po = boost::program_options;

//  Определяем глобальные переменные
int timeFlag = 0;
std::string command;
std::string build = "Debug";
std::string flag;

void run() {
    bp::group g;

    //  MAKE
    command = "cmake -H. -B_builds -DCMAKE_INSTALL_PREFIX=_install -DCMAKE_BUILD_TYPE=";
    command += build;
    bp::child c_1(command, bp::std_out > stdout, g);
    //  BUILD
    command = "cmake --build _build";
    bp::child c_2(command, bp::std_out > stdout, g);
    //  FLAG
    if (flag.size()) {
        command = "cmake --build _build --target ";
        command += flag;
        bp::child c_3(command, bp::std_out > stdout, g);
    }

    //  CHECK TIME
    if (timeFlag) {
        if (!g.wait_for(std::chrono::seconds(timeFlag))) {
            g.terminate();
            throw std::runtime_error("ERROR\tOUT OF TIME");
        }
    }

    //  MAKE
    c_1.wait();
    if (c_1.exit_code())
        throw std::runtime_error("ERROR\tMAKE");

    //  BUILD
    c_2.wait();
    if (c_2.exit_code())
        throw std::runtime_error("ERROR\tBUILD");

    //  FLAG
    if (flag.size()) {
        c_3.wait();
        if (c_3.exit_code())
            throw std::runtime_error("ERROR\tBUILD WITH FLAG");
    }
}

int main(int argc, char const *argv[]) {
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
        ("help", "выводим вспомогательное сообщение")
        ("config", po::value<std::string>(), "указываем конфигурацию сборки (по умолчанию Debug)")
        ("install", "добавляем этап установки (в директорию _install)")
        ("pack", "добавляем этап упакови (в архив формата tar.gz)")
        ("timeout", po::value<int>(), "указываем время ожидания (в секундах)");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
        } else if (vm.count("config")) {
            build = vm["config"].as<std::string>();
            run();
        } else if (vm.count("install")) {
            flag = "install";
            run();
        } else if (vm.count("pack")) {
            flag = "pack";
            run();
        } else if (vm.count("timeout")) {
            timeFlag = vm["timeout"].as<int>();
            run();
        } else {
            run();
        }

    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
