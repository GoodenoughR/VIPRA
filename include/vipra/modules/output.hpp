#pragma once

#include <concepts>
#include <filesystem>
#include <fstream>

#include "vipra/macros/module.hpp"
#include "vipra/modules/module.hpp"

#include "vipra/geometry/f3d.hpp"

#include "vipra/types/idx.hpp"
#include "vipra/types/state.hpp"
#include "vipra/types/time.hpp"

namespace VIPRA::Modules {
/**
 * @brief Dummy output for use in other concepts
 * 
 */
template <typename module_t>
class Output : public Module<Output<module_t>> {
 public:
  void write(std::filesystem::path const& outputDir) {
    std::filesystem::path filepath = outputDir / _filename;
    std::ofstream         file(filepath);
    if ( ! file.is_open() ) {
      throw std::runtime_error("Could not open file for writing: " + filepath.string());
    }

    file << this->self().to_string();

    file.close();
  }

  template <typename data_t>
  void sim_value(char const* key, data_t&& value) {
    this->self().set_sim_value(key, std::forward<data_t>(value));
  }

  template <typename data_t>
  void timestep_value(char const* key, VIPRA::idx timestep, data_t&& value) {
    this->self().set_timestep_value(key, timestep, std::forward<data_t>(value));
  }

  template <typename data_t>
  void ped_value(VIPRA::idx pedIdx, char const* key, data_t&& value) {
    this->self().set_ped_value(pedIdx, key, std::forward<data_t>(value));
  }

  template <typename data_t>
  void ped_timestep_value(VIPRA::idx pedIdx, VIPRA::idx timestep, char const* key, data_t&& value) {
    this->self().set_ped_timestep_value(pedIdx, timestep, key, std::forward<data_t>(value));
  }

  void set_filename(std::string const& filename) { _filename = filename; }

 private:
  std::string _filename;
};

}  // namespace VIPRA::Modules