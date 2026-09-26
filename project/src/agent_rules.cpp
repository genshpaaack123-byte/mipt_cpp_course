#include "event.h"
#include "fields.h"
#include "rules.h"

namespace nano_edr{

bool ScriptHostFromTemp(const Event& event){
    if (!IsProcessStart(event)){return false;}
    const std::string path=GetRequiredField(event,"image");
    if (!PathEndsWith(event,"wscript.exe") || !PathEndsWith(event,"cscript.exe")){return false;}
    if (!(CommandLineContains(event,"\\appdata\\local\\temp\\") || CommandLineContains(event,"\\windows\\temp\\"))){return false;}
    return true; 
}
bool LolbinDownload(const Event& event){
    if (!IsProcessStart(event)){return false;}
    const std::string path=GetRequiredField(event,"image");
    if (!(PathEndsWith(event,"certutil.exe") || PathEndsWith(event,"bitsadmin.exe"))){return false;}
    if (!(CommandLineContains(event,"urlcache")
     || CommandLineContains(event,"transfer")
      || CommandLineContains(event,"http:")
       || CommandLineContains(event,"https:"))){return false;}
    return true; 
}
bool HiddenPowershell(const Event& event){
    if (!IsProcessStart(event)){return false;}
    const std::string path=GetRequiredField(event,"image");
    if (!(PathEndsWith(event,"powershell.exe") || PathEndsWith(event,"pwsh.exe"))){return false;}
    if (!(CommandLineContains(event,"-w hidden")
     || CommandLineContains(event,"-windowstyle")
      || CommandLineContains(event,"-enc")
       || CommandLineContains(event,"-encodedcommand"))){return false;}
    return true; 
}
bool AutostartWrite(const Event& event){
    if (!IsFileWrite(event)){return false;}
    const std::string path=GetRequiredField(event,"path");
    if (!PathEndsWith(event,"\\start menu\\programs\\startup\\")){return false;}
    return true; 
}
bool RansomExtension(const Event& event){
    if (!IsFileWrite(event)){return false;}
    const std::string path=GetRequiredField(event,"path");
    if (!PathEndsWith(event,".locked")){return false;}
    return true; 
}

constexpr Rule kRules[] = {
    {"script_host_from_temp", ScriptHostFromTemp, Severity::kHigh},
    {"lolbin_download", LolbinDownload, Severity::kHigh},
    {"hidden_powershell", HiddenPowershell, Severity::kMedium},
    {"autostart_write", AutostartWrite, Severity::kHigh},
    {"ransom_extension", RansomExtension, Severity::kCritical}
};
const Rule* AgentRules(){
    return kRules;
};
size_t AgentRuleCount(){
    return sizeof(kRules)/sizeof(kRules[0]);
}
}