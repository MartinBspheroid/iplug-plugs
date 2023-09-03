
#include <string>
#include <vector>
class Tikka_Presets
{
public:
  struct Preset {
    
    std::string name;
    int lenght;
    int pulses;
  };
  Tikka_Presets()
  {
    addBasicPresets();
  }
  std::vector<Preset> presets;
  ~Tikka_Presets()
  {
  }
  void addPreset(int pulses, int len, std::string name) {
    Preset p;
    p.name = name;
    p.lenght = len;
    p.pulses = pulses;
    presets.push_back(p);
  }
private:

  void addBasicPresets() {

    addPreset(4, 12, "Fandango");
    addPreset(2, 3, "Afro-Cuban");
    addPreset(2, 5, "Khafif-e-ramal");
    addPreset(3, 4, "Calypso Cumbia");
    addPreset(3, 5, "Khafif-e-ramal");
    addPreset(3, 7, "Ruchenitza");
    addPreset(3, 8, "Tresillo");
    addPreset(4, 7, "Ruchenitza2");
    addPreset(4, 9, "Aksak");
    addPreset(4, 11, "Outside Now");
    addPreset(5, 6, "York-Samai");
    addPreset(5, 7, "Nawakhat");
    addPreset(5, 8, "Cinquillo");
    addPreset(5, 9, "Agsag-Samai ");
    addPreset(5, 11, "Moussorgsky");
    addPreset(5, 12, "Venda");
    addPreset(5, 16, "Bossa-Nova");
    addPreset(7, 8, "Tuareg Bendir");
    addPreset(7, 12, "Mpre Ashanti");
    addPreset(7, 16, "Ghana clap");
    addPreset(9, 16, "Central Africa");
    addPreset(11, 24, "Aka Pygmies");
    addPreset(13, 24, "Aka Pygmies Sangha");
  }
};
