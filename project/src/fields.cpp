#include "fields.h"
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
    if (!p){throw std::invalid_argument("Нет обязательного поля" + key);}
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
    
    const std::string *path=FindField(event,"path");
    if (!path){return false;}

    const std::string normalize_path=NormalizePath(*path);
    const std::string normalize_suffix=NormalizePath(suffix);
    if (normalize_path.compare(normalize_path.size()-normalize_suffix.size(),normalize_suffix.size(),normalize_suffix)==0){return true;} //начиная с , кол символов из сравниваемого слова

    return false;
}


std::string NormalizePath(const std::string& path){
    std::string new_path;
    std::size_t position_temp=0;
    if (path.find("%TEMP%")!=std::string::npos){
        new_path+="\\appdata\\local\\temp";
        position_temp=6;
    }else if (path.find("%TMP%")!=std::string::npos){
        new_path+="\\appdata\\local\\temp";
        position_temp=5;
    }
    for (std::size_t i=position_temp;i<path.size();i++){
        char symbol=path[i];
        if (symbol=='/' || symbol =='\\'){
            if (new_path.empty() || new_path.back() != '\\'){new_path+=symbol;}else{continue;}
        }else{
            symbol=static_cast<char>(std::tolower(static_cast<unsigned char>(symbol)));
            new_path+=symbol;
        }
    }
    return new_path;
}

bool CommandLineContains(const Event& event, const std::string& needle){
    const std::string* cmdline=FindField(event,"cmdline");
    if (!cmdline){return false;};
    std::string normalize_cmd;
    std::string normalize_needle;

    for (std::size_t i=0;i<(*cmdline).size();i++){
        char symbol=(*cmdline)[i];
        symbol=static_cast<char>(std::tolower(static_cast<unsigned char>(symbol)));
        normalize_cmd+=symbol;
    }
    for (std::size_t i=0;i<needle.size();i++){
        char symbol=needle[i];
        symbol=static_cast<char>(std::tolower(static_cast<unsigned char>(symbol)));
        normalize_needle+=symbol;
    }
    if (normalize_cmd.find(normalize_needle) != std::string::npos){return true;}
    return false;

}
}