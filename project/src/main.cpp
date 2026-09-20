// Каркас агента: читает журнал событий построчно и считает строки.
//
// Это заготовка занятия 1.1, а не решение. Детектов она не ищет — их вы
// добавите здесь же, в отмеченном месте ниже. Формат строки детекта, список
// признаков и правило про их порядок заданы в постановке занятия: по ним
// сравниваются эталоны.
//
// Весь код лежит в main, и на этом занятии так и надо: функции появятся
// на занятии 1.2, ссылки — на 1.3. Разбор аргументов, коды возврата и флаг
// --quiet — часть задания.
//
// Запуск:
//   nano-edr <журнал.log>
#include <cstdio>
#include <fstream>
#include <print>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    // Аргументы разбираются грубо: путь к журналу и ничего больше. Остальное,
    // включая --quiet, добавляется по заданию.
    std::string log_path;
    bool quiet = false;
    for (int i=1;i<argc;i++){
        const std::string arg=argv[i];
        if (arg=="--quiet"){
            quiet=true;

        } else if (log_path.empty()) {
            log_path=arg;
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
    std::vector<std::pair<std::string, int>> detected_features;
    while (std::getline(log, line)) {
        // Счётчик увеличивается до всех проверок: он считает строки файла,
        // а не события. Номер, посчитанный по событиям, бесполезен — по нему
        // нельзя открыть файл и посмотреть.
        ++lines;
        if (!line.empty() && line[0] == '#') {
            ++comments;
            continue;
        }
        std::size_t type_start = 0;
        std::size_t type_end = line.size();
        const std::size_t type_pos = line.find("type=");
        if (type_pos != std::string::npos) {
            type_start = type_pos + 5;
            type_end = line.find(' ', type_start);
            if (type_end == std::string::npos) {
                type_end = line.size();
            }
        }

        const std::string type_define=line.substr(type_start,type_end-type_start+1);
        bool flag=false;
        for (auto& [name_type,count]:detected_features){
            if (name_type==type_define){
                ++count;
                flag = true;
                break;
            }
        }
        if (!flag){
            detected_features.push_back({type_define,1});
        }
        
        // Строки-комментарии в журнале начинаются с '#'. Они не события,
        // и детекта по ним быть не должно.
        
        for (const auto& feature:features){
        if (line.find(feature) !=std::string::npos){
            std::print("[DETECT] строка {}, признак {}: {}\n",lines,feature,line);
        }
        }
        // Проверка признаков и печать детекта. Номер строки, который нужен
        // в выводе, — это lines.
    }
    if (!quiet){
       for (auto& [type_name,count]: detected_features){
        std::print("тип {}, количество {}\n",type_name,count);
    }

    std::print("строк {}, из них комментариев {}\n", lines, comments);
    return 0; 
    }
    
}
