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