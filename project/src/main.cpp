#include <charconv>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <print>
#include <string>
#include <vector>
#include "event_list.h"
#include "parse.h"

int main(int argc, char** argv) {
    nano_edr::EventList list{.capacity = 64};
    std::string log_path;
    bool quiet = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--window-size") {
            if (i + 1 >= argc) {return 2;}

            const std::string value_text = argv[++i];
            std::size_t value = 0;
            const auto [ptr, ec] = std::from_chars(value_text.data(),value_text.data() + value_text.size(),value);
            if (ec != std::errc{} ||ptr != value_text.data() + value_text.size()) {return 2;}
            list.capacity = value;
        } else if (arg == "--quiet") {
            quiet = true;
        } else {
            if (arg.starts_with("--") || !log_path.empty()) {return 2;}
            log_path = arg;
        }
    }

    if (log_path.empty()) {
        std::print(stderr, "использование: nano-edr <журнал.log>\n");
        return 2;
    }

    std::ifstream log(log_path);
    if (!log) {
        std::print(stderr, "не удалось открыть журнал: {}\n", log_path);
        return 2;
    }

    long long lines = 0;
    long long comments = 0;
    std::string line;
    const std::vector<std::string> features{
        "wscript.exe",
        ".locked",
        "certutil.exe",
        "\\Startup\\"
    };
    std::vector<std::pair<std::string, int>> all_type;

    while (std::getline(log, line)) {
        ++lines;

        std::size_t first = line.find_first_not_of(" \t");
        if (first != std::string::npos &&
            (line[first] == '#' || line[first] == ';')) {
            ++comments;
            continue;
        }

        if (nano_edr::IsBlankOrComment(&line)) {continue;}

        nano_edr::Event event;
        if (!nano_edr::ParseEventLine(&line, &event)) {continue;}

        bool known_type = false;
        for (auto& [type_name, count] : all_type) {
            if (type_name == event.type) {
                ++count;
                known_type = true;
                break;
            }
        }
        if (!known_type) {
            all_type.push_back({event.type, 1});
        }

        bool detected = false;
        for (const std::string& feature : features) {
            if (line.find(feature) != std::string::npos) {
                detected = true;
                std::print("[DETECT] строка {}, признак {}: {}\n",lines,feature,line);
            }
        }

        if (detected && !quiet) {
            if (list.size == 1) {
                const nano_edr::Event& one_node = list.head->event;
                std::print("[CTX] -1: ts={} type={} pid={}\n",one_node.ts,one_node.type,one_node.pid);
            } else if (list.size >= 2) {
                const nano_edr::EventNode* context = list.head;
                if (list.size > 2) {
                    for (std::size_t i = 0; i < list.size - 2; ++i) {
                        context = context->next;
                    }
                }

                std::print("[CTX] -2: ts={} type={} pid={}\n",context->event.ts,context->event.type,context->event.pid);
                context = context->next;
                std::print("[CTX] -1: ts={} type={} pid={}\n",context->event.ts,context->event.type,context->event.pid);
            }
        }

        nano_edr::ListPushBack(&list, &event);
    }

    if (!quiet) {
        for (const auto& [type_name, count] : all_type) {
            std::print("тип {}, количество {}\n", type_name, count);
        }
        std::print("строк {}, из них комментариев {}\n", lines, comments);
    }

    return 0;
}
