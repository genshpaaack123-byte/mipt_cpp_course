#include "agent_rules.h"
#include <print>
namespace nano_edr{
const char* SeverityName(Severity severity){
    switch (severity){
        case Severity::kLow:
            return "LOW";
        case Severity::kMedium:
            return "MEDIUM";
        case Severity::kHigh:
            return "HIGH";
        case Severity::kCritical:
            return "CRITICAL";
        default:
            return "?";
    }
}

// Прогоняет событие по таблице и печатает сработавшие правила. В строке детекта
// после [DETECT] один пробел, дальше три поля через два пробела: важность,
// идентификатор и пара ts/pid, внутри которой пробел один:
//
//     [DETECT] high  script_host_from_temp  ts=1730000003000 pid=1101
//
// Возвращает число детектов. Таблица передаётся указателем и длиной, а не
// контейнером: контейнеры начинаются с лекции 13, а массив с длиной — то, что
// C++ умел с самого начала, и то, во что превращается любой контейнер на
// границе с C.
size_t CheckRules(const Event& event, const Rule* rules, size_t rule_count){
    std::size_t detect=0;
    for (std::size_t i=0;i<rule_count;i++){
        if (rules[i].check(event)){
            detect+=1;
            std::print("[DETECTED] {}  {}  ts={} pid={}",SeverityName(rules[i].severity),rules[i].id,event.ts,event.pid);
        }
    }
    return detect;
}
}