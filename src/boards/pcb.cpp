#include <mirte-board.hpp>
#include <pcbs/v08.hpp>
Mirte_Board_pcb::Mirte_Board_pcb(std::shared_ptr<Mirte_Board> mcu,
                                 std::string version) {
  this->mcu = mcu;
  this->version = version;
  std::cout << "Mirte_Board_pcb::Mirte_Board_pcb" << version << std::endl;
  auto v = try_parse_double(version).value_or(0.8);
  if (v == 0.8) {
    std::cout << "0.88888888888888" << version << std::endl;
    this->connectors = mirte_pico_pcb_map08;
  } else if (v == 0.6) {
    this->connectors = mirte_pico_pcb_map06;
  } else {
    std::cerr << "Unknown PCB version: " << version << std::endl;
  }
}

std::map<std::string, int>
Mirte_Board_pcb::resolveConnector(std::string connector) {

  const auto pins = this->connectors.at(connector);
  std::map<std::string, int> resolved_pins;
  for (auto pin : pins) {
    resolved_pins[pin.first] = mcu->resolvePin(pin.second);
  }
  return resolved_pins;
}

int Mirte_Board_pcb::resolvePin(std::string pin) {
  return mcu->resolvePin(pin);
}
