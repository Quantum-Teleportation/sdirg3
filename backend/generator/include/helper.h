#ifndef HELPER_H
#define HELPER_H

#include <sstream>
#include <string_view>
#include <unordered_map>

namespace terraformer {

class BaseHelper {
	std::string_view name_;

public:
	BaseHelper(std::string_view name) : name_(name) {}

	inline std::string Serialize() const {
		std::stringstream out;
		out << "[" << name_.data() << "]\n";
		Serialize_(out);
		out << "[/" << name_.data() << "]\n";
		return out.str();
	}

	virtual ~BaseHelper() = 0;

private:
	virtual void Serialize_(std::stringstream &) const = 0;
};

inline BaseHelper::~BaseHelper() {}

template <class Derived> static BaseHelper *defaultCreator() {
	return static_cast<BaseHelper *>(new Derived);
}

class HelperFactory {
	typedef BaseHelper *(*Creator)();
	std::unordered_map<std::string, Creator> reestr_;
	HelperFactory() {}
	~HelperFactory() {}

public:
	HelperFactory(const HelperFactory &) = delete;
	HelperFactory &operator=(const HelperFactory &) = delete;

	static HelperFactory &GetInstance() {
		static HelperFactory f;
		return f;
	}

	inline bool RegisterClass(const std::string_view name, Creator creator) {
		reestr_[name.data()] = creator;
		return true;
	}

	inline BaseHelper *CreateClassByName(const std::string_view name) const {
		using namespace std::string_literals;

		if (auto it = reestr_.find(name.data()); it != reestr_.end()) {
			return (it->second)();
		}
		throw std::runtime_error("not registered class "s + name.data());
		return nullptr;
	}
};

// nu nado
#define REGISTER_CLASS(name)                                                   \
	namespace {                                                                \
	bool ___XXX_##name##_XXX___ = HelperFactory::GetInstance().RegisterClass(  \
		#name, defaultCreator<name>);                                          \
	}

#define CREATE_CLASS(name)                                                     \
	HelperFactory::GetInstance().CreateClassByName(#name);

} // namespace terraformer

#endif