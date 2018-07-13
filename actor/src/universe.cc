#include "universe.h"

#include <utility>

namespace kin {


bool Universe::AddSystem(std::unique_ptr<System> system) {
    if (system.get() == nullptr) {
        throw std::invalid_argument("Universe::AddSystem() : "
            "Passed unique_ptr contained nullptr. "
            "Was it std::move()'d correctly");
    }
    size_t initial_size = systems_.size();
    systems_[system->id()] = std::move(system);
    return systems_.size() == initial_size;
}

bool Universe::AddActor(std::unique_ptr<Actor> actor) {
    if (actor.get() == nullptr) {
        throw std::invalid_argument("Universe::AddActor() : "
            "Passed unique_ptr contained nullptr. "
            "Was it std::move()'d correctly");
    }
    size_t initial_size = actors_.size();
    actors_[actor->id()] = std::move(actor);
    return actors_.size() == initial_size;
}


}  // namespace kin


extern "C" bool kin_Universe_AddSystem(kin::Universe *uni, kin::System *system){
    return uni->AddSystem(std::unique_ptr<kin::System>(system));
}

extern "C" bool kin_Universe_AddActor(kin::Universe *uni, kin::Actor *actor) {
    return uni->AddActor(std::unique_ptr<kin::Actor>(actor));
}
