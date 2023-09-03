#pragma once
#include "IPlugParameter.h"
#include "json.hpp"
#include <fstream>

BEGIN_IPLUG_NAMESPACE
using namespace nlohmann;
class SA_ConfigSaver
{
public:
  SA_ConfigSaver() 
  {

  }
  json js;
  void save(const IParam* p) {
    js.push_back(p->Value());
    
  }
  
  ~SA_ConfigSaver()
  {
    std::ofstream output("config.json");
    output  << js << std::endl;
    output.close();
  }

private:

};

class SA_ConfigLoader
{
  
public:
  SA_ConfigLoader()
  {
    std::ifstream input("config.json");
    
    js = json::parse(input);
    size = js.size();
    input.close();
  }
  nlohmann::json js;
  int size;
  void load(IParam* p, const int& index ) {
    if (index >= size) {
      return;
    }
    p->Set(js[index].get<double>());

  }

  ~SA_ConfigLoader()
  {
    
  }

private:

};
END_IPLUG_NAMESPACE
