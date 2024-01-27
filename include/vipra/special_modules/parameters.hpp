#pragma once

#include <map>

#include <nlohmann/json.hpp>

#include "vipra/concepts/input.hpp"
#include "vipra/concepts/parameter_input.hpp"
#include "vipra/concepts/parameters.hpp"

#include "vipra/macros/module.hpp"
#include "vipra/modules.hpp"
#include "vipra/types/parameter.hpp"

// TODO(rolland): Check that all required parameters are provided
namespace VIPRA {
template <Concepts::parameter_qualified_input input_t>
class Parameters {
 public:
  VIPRA_MODULE_TYPE(PARAMETERS)

  explicit Parameters<input_t>(input_t&& input) : _input(input) {}

  /**
   * @brief Registers a parameter for a module
   * 
   * @tparam module_t 
   * @tparam param_t 
   * @param module 
   * @param moduleName 
   * @param paramName 
   * @param param 
   */
  void register_param(VIPRA::Modules::Type module, const std::string& moduleName,
                      const std::string& paramName, ParameterType param) {
    if (contains(module, moduleName, paramName))
      throw std::runtime_error("Parameter: " + paramName + " For Module: " + moduleName +
                               " Registered Twice, Check for Duplicate Parameter Names");
    _params[module][moduleName][paramName] = param;
  }

  /**
   * @brief Returns the value of the parameter if it exists, otherwise throws an error
   * 
   * @tparam data_t 
   * @param module 
   * @param moduleName 
   * @param paramName 
   * @return data_t 
   */
  template <typename data_t>
  [[nodiscard]] auto get_param(VIPRA::Modules::Type module, const std::string& moduleName,
                               const std::string& paramName) const -> data_t {
    std::string moduleStr = to_string(module);
    std::transform(moduleStr.begin(), moduleStr.end(), moduleStr.begin(), ::tolower);

    if (!contains(module, moduleName, paramName))
      throw std::runtime_error("Parameter: " + paramName + " For Module: " + moduleName + " Not Registered");

    auto value = _input.template get<Parameter<data_t>>(moduleStr, paramName);
    if (!value.has_value()) {
      if (_params.at(module).at(moduleName).at(paramName) == ParameterType::REQUIRED)
        throw std::runtime_error("Required Parameter: " + paramName + " For Module: " + moduleName +
                                 " Not Provided In Input");
    }

    return randomize_parameter(value.value());
  }

 private:
  input_t                                                                                     _input;
  std::map<VIPRA::Modules::Type, std::map<std::string, std::map<std::string, ParameterType>>> _params;

  [[nodiscard]] constexpr auto randomize_parameter(auto&& paramVal) const {
    // TODO(rolland): randomize parameter, currently just pass through
    return paramVal.value;
  }

  [[nodiscard]] auto contains(VIPRA::Modules::Type module, const std::string& moduleName,
                              const std::string& paramName) const -> bool {
    return _params.contains(module) && _params.at(module).contains(moduleName) &&
           _params.at(module).at(moduleName).contains(paramName);
  }
};

CHECK_MODULE(ParamModule, Parameters<Concepts::DummyInput>)
}  // namespace VIPRA