#include <parse.h>
#include <string>
#include "event.h"

namespace nano_edr {
bool ParsePair(const std::string* pair,nano_edr::Field* out){
    std::size_t pos=(*pair).find('=');
    if (pos==std::string::npos){
        return false;
    }
    std::string key=(*pair).substr(0,pos);
    std::string value=(*pair).substr(pos+1);
    if (!value.empty() && value[0]=='\"'){
        if (value[value.size()-1]!='\"'){return false;}
        value=value.substr(1,value.size()-2);
    }
    if (key==""){
        return false;
    }
    *out={key,value};
    return true;
}
bool IsBlankOrComment(const std::string* line){
    if ((*line).empty()){return true;}
    std::size_t pos=0;
    while (pos<(*line).size() && ((*line)[pos]==' ' || (*line)[pos]=='\t')){pos++;}
    if ((*line)[pos]=='#' || (*line)[pos]==';'){return true;}else{return false;}
}
bool ParseEventLine(const std::string* line,nano_edr::Event* out){
    
    if (IsBlankOrComment(line)){return false;}
    bool has_ts=false;
    bool has_type=false;
    std::size_t pos=0;
    int ts_count=0;
    while (pos<(*line).size()) {  
        while (pos<(*line).size()&& ((*line)[pos]==' ' || (*line)[pos]=='\t')){pos++;}
        if (pos>=(*line).size()){break;}
        std::size_t end=pos;
        bool quotes=false;

        while (end<(*line).size()){
            if ((*line)[end]=='\"'){quotes=!quotes;}
            if (!quotes && ((*line)[end]==' ' || (*line)[end]=='\t')){break;}
            
            end++;
        }
        
        std::string part_of_line=(*line).substr(pos,end-pos);
        
        nano_edr::Field field;
        if (!ParsePair(&part_of_line,&field)){return false;}
        if (field.key=="ts"){
            ++ts_count;
            if (ts_count>1){out->fields.push_back(field);}
            else{out->ts=field.value;has_ts=true;}
        }else if (field.key=="type"){out->type=field.value;has_type=true;}
        else if (field.key=="pid"){out->pid=field.value;}
        else{out->fields.push_back(field);}
        pos=end;
    }
    return has_ts && has_type;


}
}