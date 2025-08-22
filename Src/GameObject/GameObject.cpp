#include "GameObject.hpp"

namespace GM {
	std::shared_ptr<GameObject> GameObject::parent() {
		return parent_.lock();
	}
	void GameObject::addChild(const std::shared_ptr<GameObject>& child) {
		child->parent_ = shared_from_this();
		childs_.push_back(child);
	}
	void GameObject::update(float dt) {
		for (auto& co : components_) {
			co->update(dt);
		}
		for (auto& ch : childs_) {
			ch->update(dt);
		}

	}
	void GameObject::begin() {
		for (auto& co : components_) {
			co->begin();

		}
		for (auto& ch : childs_) {
			ch->begin();
		}
	}
}