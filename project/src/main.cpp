
#include <charconv>
#include "parse.h"
#include "event_list.h"
#include <cstdio>
#include <fstream>
#include <print>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    nano_edr::EventList list{.capacity = 64};
    
    std::string log_path;
    bool quiet = false;
    for (int i=1;i<argc;i++){
        std::string arg=argv[i];
        if (arg=="--window-size"){
            if (i+1>=argc){return 2;}
            if (i+1<argc){
                std::string string_size=argv[i+1];
                std::size_t value;
                auto [ptr,ec]=std::from_chars(string_size.data(),string_size.data()+string_size.size(),value);
                //if (value!=0){list.capacity=value;}
                i++;
                if (!(ec==std::errc{}) || !(ptr==string_size.data()+string_size.size())){
                    return 2;
                }else{list.capacity=value;}

            }
        }
        else if (arg=="--quiet"){
            quiet=true;}
        else {
        if (arg.starts_with("--")) {
            return 2;
        }

        if (!log_path.empty()) {
            return 2;
        }

        log_path = arg;
        }
    }
    
    if (log_path.empty()){
        std::print(stderr, "использование: nano-edr <журнал.log>\n");
        return 2;
    }

    std::ifstream log(log_path);
    if (!log) {
        std::print(stderr, "не удалось открыть журнал: {}\n", argv[1]);
        return 2;
    }

    long long lines = 0;
    long long comments = 0;
    std::string line;
    std::vector<std::string> features{
            "wscript.exe",
            ".locked",
            "certutil.exe",
            "\\Startup\\"
    };
    std::vector<std::pair<std::string, int>> all_type;
    while (std::getline(log, line)) {
        nano_edr::Event event;
        ++lines;
       std::size_t first = line.find_first_not_of(" \t");

        if (first != std::string::npos &&
            (line[first] == '#' || line[first] == ';')) {
            ++comments;
            continue;
        }

        if (nano_edr::IsBlankOrComment(&line)) {
            continue;
        }
        if(!nano_edr::ParseEventLine(&line,&event)){
            if (line.find('#')){comments++;}
            continue;}
        bool flag=false;
        for (auto& [this_type,count]:all_type){
            if (this_type==event.type){flag=true;count++;break;}
        }
        if (!flag){all_type.push_back({event.type,1})}
        for (const std::string& feature : features){
            if (!(line.find(feature)==std::string::npos)){
                std::print("[DETECT] строка {}, признак {}: {}\n",lines,feature,line);
            }
        }
        const nano_edr::EventNode* i=list.head;
        if (list.size>2){
            while (!(i->next->next==nullptr)){
                i=i->next;
                std::print<<
            }
            if (list.tail!=nullptr){
                std::print<<
            }
        }
        nano_edr::ListPushBack(&list, &event);

    }
    if (!quiet){
       /*for (auto& [type_name,count]: detected_features){
        std::print("тип {}, количество {}\n",type_name,count);
    }*/

    std::print("строк {}, из них комментариев {}\n", lines, comments);
    return 0; 
    }
    
}