#include "../include/model.h"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "../utils/string_utils.hpp"
#include "../utils/value_parser.hpp"

const char* modelTypeName(ModelType type) {
    switch (type) {
        case ModelType::Diode:   return "Diode";
        case ModelType::BJT_NPN: return "BJT_NPN";
        case ModelType::BJT_PNP: return "BJT_PNP";
        case ModelType::NMOS:    return "NMOS";
        case ModelType::PMOS:    return "PMOS";
        default:                 return "Unknown";
    }
}

DeviceModel::DeviceModel(std::string name, ModelType type)
    : _name(std::move(name)), _type(type) {}

const std::string& DeviceModel::name() const { return _name; }

ModelType DeviceModel::type() const { return _type; }

const DiodeParams& DeviceModel::diode() const { return _diode; }

const BjtParams& DeviceModel::bjt() const { return _bjt; }

const MosParams& DeviceModel::mos() const { return _mos; }

DiodeParams& DeviceModel::diode() { return _diode; }

BjtParams& DeviceModel::bjt() { return _bjt; }

MosParams& DeviceModel::mos() { return _mos; }

void DeviceModel::print(std::ostream& os) const {
    os << _name << " (" << modelTypeName(_type) << ") ";
    switch (_type) {
        case ModelType::Diode:
            os << "Is=" << _diode.Is << " N=" << _diode.N << " Rs=" << _diode.Rs;
            break;
        case ModelType::BJT_NPN:
        case ModelType::BJT_PNP:
            os << "Is=" << _bjt.Is << " Bf=" << _bjt.Bf << " Br=" << _bjt.Br
               << " Vaf=" << _bjt.Vaf;
            break;
        case ModelType::NMOS:
        case ModelType::PMOS:
            os << "Vto=" << _mos.Vto << " Kp=" << _mos.Kp
               << " Lambda=" << _mos.Lambda << " Gamma=" << _mos.Gamma;
            break;
    }
}

ModelType ModelLibrary::parseModelType(const std::string& typeToken) {
    if (equals_ignore_case(typeToken, "D")) {
        return ModelType::Diode;
    }
    if (equals_ignore_case(typeToken, "NPN")) {
        return ModelType::BJT_NPN;
    }
    if (equals_ignore_case(typeToken, "PNP")) {
        return ModelType::BJT_PNP;
    }
    if (equals_ignore_case(typeToken, "NMOS")) {
        return ModelType::NMOS;
    }
    if (equals_ignore_case(typeToken, "PMOS")) {
        return ModelType::PMOS;
    }
    throw std::runtime_error("Unknown model type: " + typeToken);
}

void ModelLibrary::applyParameter(DeviceModel& model, const std::string& key,
                                const std::string& valueToken) {
    const double value = parse_value(valueToken);
    if (value == -1) {
        throw std::runtime_error("Invalid model parameter value: " + valueToken);
    }

    switch (model.type()) {
        case ModelType::Diode:
            if (equals_ignore_case(key, "IS")) {
                model.diode().Is = value;
            } else if (equals_ignore_case(key, "N")) {
                model.diode().N = value;
            } else if (equals_ignore_case(key, "RS")) {
                model.diode().Rs = value;
            } else {
                throw std::runtime_error("Unknown diode model parameter: " + key);
            }
            break;
        case ModelType::BJT_NPN:
        case ModelType::BJT_PNP:
            if (equals_ignore_case(key, "IS")) {
                model.bjt().Is = value;
            } else if (equals_ignore_case(key, "BF")) {
                model.bjt().Bf = value;
            } else if (equals_ignore_case(key, "BR")) {
                model.bjt().Br = value;
            } else if (equals_ignore_case(key, "VAF") || equals_ignore_case(key, "VA")) {
                model.bjt().Vaf = value;
            } else {
                throw std::runtime_error("Unknown BJT model parameter: " + key);
            }
            break;
        case ModelType::NMOS:
        case ModelType::PMOS:
            if (equals_ignore_case(key, "VTO") || equals_ignore_case(key, "VT0")) {
                model.mos().Vto = value;
            } else if (equals_ignore_case(key, "KP")) {
                model.mos().Kp = value;
            } else if (equals_ignore_case(key, "LAMBDA")) {
                model.mos().Lambda = value;
            } else if (equals_ignore_case(key, "GAMMA")) {
                model.mos().Gamma = value;
            } else {
                throw std::runtime_error("Unknown MOSFET model parameter: " + key);
            }
            break;
    }
}

DeviceModel ModelLibrary::makeBuiltin(const std::string& name, ModelType type) {
    DeviceModel model(name, type);
    switch (type) {
        case ModelType::Diode:
            break;
        case ModelType::BJT_NPN:
            break;
        case ModelType::BJT_PNP:
            model.bjt().Bf = 50.0;
            break;
        case ModelType::NMOS:
            model.mos().Vto = 0.7;
            model.mos().Kp = 2e-5;
            break;
        case ModelType::PMOS:
            model.mos().Vto = -0.7;
            model.mos().Kp = 1e-5;
            break;
    }
    return model;
}

void ModelLibrary::installBuiltinDefaults() {
    const char* builtinNames[] = {"DMOD", "NPN", "PNP", "NMOS", "PMOS"};
    const ModelType builtinTypes[] = {
        ModelType::Diode, ModelType::BJT_NPN, ModelType::BJT_PNP,
        ModelType::NMOS, ModelType::PMOS};

    for (size_t i = 0; i < sizeof(builtinNames) / sizeof(builtinNames[0]); ++i) {
        const std::string name(builtinNames[i]);
        if (!contains(name)) {
            add(makeBuiltin(name, builtinTypes[i]));
        }
    }
}

bool ModelLibrary::parseLine(const std::string& line) {
    std::stringstream iss(line);
    std::string dotModel;
    iss >> dotModel;
    if (!equals_ignore_case(dotModel, ".model")) {
        std::cerr << "Not a .model line: " << line << std::endl;
        return false;
    }

    std::string modelName;
    std::string typeToken;
    if (!(iss >> modelName >> typeToken)) {
        std::cerr << "Incomplete .model line: " << line << std::endl;
        return false;
    }

    try {
        DeviceModel model(modelName, parseModelType(typeToken));

        std::string token;
        while (iss >> token) {
            if (token == "(") {
                continue;
            }
            if (token == ")") {
                break;
            }

            const auto eq = token.find('=');
            if (eq == std::string::npos) {
                std::cerr << "Expected key=value in .model line: " << token << std::endl;
                return false;
            }

            std::string key = token.substr(0, eq);
            std::string valueToken = token.substr(eq + 1);
            applyParameter(model, key, valueToken);
        }

        return add(std::move(model));
    } catch (const std::exception& ex) {
        std::cerr << "Failed to parse .model line: " << ex.what() << std::endl;
        return false;
    }
}

bool ModelLibrary::add(DeviceModel model) {
    const std::string& name = model.name();
    if (name.empty()) {
        std::cerr << "Model name cannot be empty" << std::endl;
        return false;
    }
    if (_models.count(name)) {
        std::cerr << "Duplicate model definition: " << name << std::endl;
        return false;
    }
    _models.emplace(name, std::move(model));
    return true;
}

const DeviceModel* ModelLibrary::find(const std::string& name) const {
    const auto it = _models.find(name);
    if (it == _models.end()) {
        return nullptr;
    }
    return &it->second;
}

bool ModelLibrary::contains(const std::string& name) const {
    return _models.find(name) != _models.end();
}

const DeviceModel& ModelLibrary::get(const std::string& name) const {
    const DeviceModel* model = find(name);
    if (model == nullptr) {
        throw std::runtime_error("Unknown model: " + name);
    }
    return *model;
}

bool ModelLibrary::validateDeviceModel(const std::string& modelName,
                                       ModelType expected) const {
    if (modelName.empty()) {
        return expected == ModelType::Diode;
    }

    const DeviceModel* model = find(modelName);
    if (model == nullptr) {
        return false;
    }
    return model->type() == expected;
}

size_t ModelLibrary::size() const {
    return _models.size();
}

void ModelLibrary::print(std::ostream& os) const {
    os << "Models: " << _models.size() << std::endl;
    for (const auto& entry : _models) {
        os << "  ";
        entry.second.print(os);
        os << std::endl;
    }
}
