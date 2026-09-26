/*#include "fields.h"
#include <charconv>
#include <charconv>
#include <stdexcept>
#include "event.h"
const std::string* FindField(const nano_edr::Event& event, const std::string& key){
    for (const nano_edr::Field& field : event.fields){
        if (field.key==key){
            const std::string*p =&field.value;
            return p;
        }
    return nullptr;
    }
}
const std::string& GetRequiredField(const nano_edr::Event& event, const std::string& key){
    const std::string *p=::FindField(event,key);
    if (!p){throw std::invalid_argument("поле не найдено");}
    return *p;

}
bool GetIntField(const nano_edr::Event& event, const std::string& key, uint64_t* out){
    const std::string *p=::FindField(event,key);
    if (!p || !out){return false;}
    else{
        const auto [ptr,ec]=std::from_chars((*p).data(),(*p).data()+(*p).size(),*out);
        if (ec!=std::errc{} || ptr!=(*p).data()+(*p).size()){return false;}
    }
    return true;
}
*/