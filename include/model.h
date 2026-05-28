#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

enum class ModelType {
    Diode,
    BJT_NPN,
    BJT_PNP,
    NMOS,
    PMOS
};

const char* modelTypeName(ModelType type);

struct DiodeParams {
    double Is = 1e-14;
    double N = 1.0;
    double Rs = 0.0;
};

struct BjtParams {
    double Is = 1e-16;
    double Bf = 100.0;
    double Br = 1.0;
    double Vaf = 100.0;
};

struct MosParams {
    double Vto = 0.7;
    double Kp = 2e-5;
    double Lambda = 0.0;
    double Gamma = 0.0;
};

class DeviceModel {
public:
    DeviceModel() = default;
    DeviceModel(std::string name, ModelType type);

    const std::string& name() const;
    ModelType type() const;

    const DiodeParams& diode() const;
    const BjtParams& bjt() const;
    const MosParams& mos() const;

    DiodeParams& diode();
    BjtParams& bjt();
    MosParams& mos();

    void print(std::ostream& os) const;

private:
    std::string _name;
    ModelType _type = ModelType::Diode;
    DiodeParams _diode;
    BjtParams _bjt;
    MosParams _mos;
};

class ModelLibrary {
public:
    bool parseLine(const std::string& line);

    bool add(DeviceModel model);
    const DeviceModel* find(const std::string& name) const;
    bool contains(const std::string& name) const;

    const DeviceModel& get(const std::string& name) const;

    bool validateDeviceModel(const std::string& modelName, ModelType expected) const;

    size_t size() const;
    void print(std::ostream& os = std::cout) const;

private:
    static ModelType parseModelType(const std::string& typeToken);
    static void applyParameter(DeviceModel& model, const std::string& key,
                               const std::string& valueToken);

    std::unordered_map<std::string, DeviceModel> _models;
};
