#include "bjt.h"

#include <utility>

namespace {
std::vector<std::string> makeNodes(const std::string& collector,
                                   const std::string& base,
                                   const std::string& emitter,
                                   const std::string& substrate) {
    if (substrate.empty()) {
        return {collector, base, emitter};
    }
    return {collector, base, emitter, substrate};
}
} // namespace

BJT::BJT(std::string n, std::string collector, std::string base, std::string emitter,
         std::string model, std::string substrate)
    : Element(std::move(n), makeNodes(collector, base, emitter, substrate)),
      _model(std::move(model)) {}

ElementType BJT::getType() const {
    return ElementType::BJT;
}

double BJT::getValue() const {
    return 0.0;
}

const std::string& BJT::getModel() const {
    return _model;
}

const std::string& BJT::collector() const {
    return _nodes[0];
}

const std::string& BJT::base() const {
    return _nodes[1];
}

const std::string& BJT::emitter() const {
    return _nodes[2];
}

bool BJT::hasSubstrate() const {
    return _nodes.size() == 4;
}

const std::string& BJT::substrate() const {
    if (!hasSubstrate()) {
        throw std::logic_error(
            "BJT::substrate(): device has no substrate terminal (C-B-E only)");
    }
    return _nodes[3];
}