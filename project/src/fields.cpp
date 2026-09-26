#include "fields.h"
#include <algorithm>
#include <charconv>
#include <stdexcept>
#include "event.h"
#include <cctype>
namespace nano_edr{

const std::string* FindField(const Event& event, const std::string& key){
    for (const Field& field: event.fields){
        if (field.key==key){
            const std::string *p=&field.value;
            return p;
        }
    }
    return nullptr;
}


const std::string& GetRequiredField(const Event& event, const std::string& key){
    const std::string *p=nano_edr::FindField(event,key);
    if (!p){throw std::invalid_argument("Нет обязательного поля");}
    return *p;
}


bool GetIntField(const Event& event, const std::string& key, uint64_t* out){
    const std::string *p=nano_edr::FindField(event,key);
    if (!p){return false;} //если поле отсутствует
    else{ 
        uint64_t temp_out; //чтобы не записать кал по указателю
        const auto [ptr,ec]=std::from_chars((*p).data(),(*p).data()+(*p).size(),temp_out);
        if (ec!=std::errc() || (ptr!=(*p).data()+(*p).size())){return false;} //учитывает переполнение или если не разобралось
        *out=temp_out;
        return true;
    }
}

uint64_t GetIntField(const Event& event, const std::string& key,uint64_t fallback){
    const std::string *p=nano_edr::FindField(event,key);
    if (!p){return fallback;} //если поле отсутствует
    else{ 
        uint64_t temp_fallback;
        const auto [ptr,ec]=std::from_chars((*p).data(),(*p).data()+(*p).size(),temp_fallback);
        if (ec!=std::errc() || (ptr!=(*p).data()+(*p).size())){return fallback;} //учитывает переполнение или если не разобралось
        return temp_fallback;
    }
}

bool IsProcessStart(const Event& event){
    if (event.type=="process_start"){return true;}
    return false;
}
bool IsFileWrite(const Event& event){
        if (event.type=="file_write" || event.type=="file_create" || event.type=="file_move"){return true;}
    return false;
}

bool IsNetConnect(const Event& event){
            if (event.type=="net_connect"){return true;}
    return false;
}

bool PathEndsWith(const Event& event, const std::string& suffix){
    bool fl=false; //флаг наличия ключа
    for (const Field& field: event.fields){
        if (field.key=="path" || field.key=="image" || field.key=="to"){
            fl=true;
            if ((nano_edr::NormalizePath(field.value)).find(nano_edr::NormalizePath(suffix))==std::string::npos){return false;}else{return true;}
        }
    }
    if (fl==false){return false;}
}


std::string NormalizePath(const std::string& path){
    std::string new_path;
    std::size_t position_temp=0;
    if (path.find("%TEMP%")!=std::string::npos){
        new_path+="/appdata/local/temp";
        position_temp=6;
    }else if (path.find("%TMP%")!=std::string::npos){
        new_path+="/appdata/local/temp";
        position_temp=5;
    }
    for (int i=position_temp;i<path.size();i++){
        char symbol=path[i];
        symbol=static_cast<char>(std::tolower(static_cast<unsigned char>(symbol)));
        if (symbol=='\\' || symbol=='/'){
            symbol='/';
            if (new_path.empty() || new_path[-1]!='/'){new_path+=symbol;}else{continue;}
        }else{new_path+=symbol;}
    }
    return new_path;
}

bool CommandLineContains(const Event& event, const std::string& needle){
    bool fl=false; //флаг наличия ключа
    for (const Field& field: event.fields){
        if (field.key=="cmdline"){
            fl=true;
            if ((field.value).find(needle)==std::string::npos){return false;}else{return true;}
        }
    }
    if (fl==false){return false;}
}
}