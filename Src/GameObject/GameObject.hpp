#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <algorithm>
namespace GM {
	class GameObject;
	class Component {
	public:
		
		virtual ~Component() = default;
		virtual void begin(){}
		virtual void update(float dt) {}
		void setOwner(const std::shared_ptr<GameObject>& o) {
			owner = o;
		}
	protected:
		std::weak_ptr<GameObject> owner;

	};
	class Transform {
	public:
		Transform()
		{
		}
		Transform& operator=(const Transform& other) {
			position = other.position;
			rotation = other.rotation;
			scale = other.scale;
			trans = other.trans;
			return *this;
		}
		Transform(const Transform& other)
			: position(other.position),
			rotation(other.rotation),
			scale(other.scale)
			,trans(other.trans)
		{

		}
		glm::vec3 position = glm::vec3(1.0f);
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		glm::mat4 trans = glm::mat4(1.0f);
		void updateMatrix() {
			glm::mat4 rot = glm::toMat4(rotation);
			glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);
			glm::mat4 tra = glm::translate(glm::mat4(1.0f), position);
			trans =  tra * rot * sca;
		}
		
	private:
		
	protected:

	};
	class GameObject : public std::enable_shared_from_this<GameObject> {
	public:
		Transform transform;
		std::string name;
		std::shared_ptr<GameObject> parent();
		void addChild(const std::shared_ptr<GameObject>& child);
		virtual void update(float dt);
		virtual void begin();
		template<typename T, typename... Args>
		std::shared_ptr<T> addComponent(Args&&... args) {
			auto comp = std::make_shared<T>(std::forward<Args>(args)...);
			comp->setOwner(shared_from_this());
			components_.push_back(comp);
			return comp;
		}
		template<typename T>
		std::shared_ptr<T> getComponent() {
			for (auto& c : components_) {
				if (auto t = std::dynamic_pointer_cast<T>(c))
					return t;
			}
			return nullptr;
		}
		~GameObject() {
			childs_.clear();
			components_.clear();
		}
	private:
		
		std::vector<std::shared_ptr<GameObject>> childs_;
		std::vector<std::shared_ptr<Component>> components_;
	protected:
		std::weak_ptr<GameObject> parent_;
	};
}