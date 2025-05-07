#ifndef HELPER_H
#define HELPER_H

#include <functional>
#include <iostream>
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
		out << "[" << name_ << "]\n";
		out << "name = " << GetName() << "\n";
		Serialize_(out);
		out << "[/" << name_ << "]\n";
		return out.str();
	}

	virtual ~BaseHelper() = 0;

private:
	virtual std::string_view GetName() const = 0;
	virtual void Serialize_(std::stringstream &) const = 0;
};

#define DEFINE_CLASSNAME(name)                                                 \
	std::string_view GetName() const override { return #name; }

inline BaseHelper::~BaseHelper() {}

template <class Base, class Derived> static Base *defaultCreator() {
	return static_cast<Base *>(new Derived);
}

template <typename Base> class HelperFactory {
	typedef Base *(*Creator)();
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
		std::cout << "REGISTER:" << std::endl;
		std::cout << name << std::endl;
		std::cout << "END REGISTER:" << std::endl;
		reestr_[name.data()] = creator;
		return true;
	}

	inline Base *CreateClassByName(const std::string_view name) const {
		using namespace std::string_literals;
		std::cout << "CREATE:" << std::endl;
		for (const auto &[k, v] : reestr_) {
			std::cout << k << std::endl;
		}
		std::cout << "END CREATE:" << std::endl;

		if (auto it = reestr_.find(name.data()); it != reestr_.end()) {
			return (it->second)();
		}
		throw std::runtime_error("not registered class "s + name.data());
		return nullptr;
	}
};

// nu nado
#define REGISTER_CLASS(base, name)                                             \
	namespace {                                                                \
	bool ___XXX_##name##_XXX___ =                                              \
		HelperFactory<base>::GetInstance().RegisterClass(                      \
			#name, defaultCreator<base, name>);                                \
	}

#define CREATE_CLASS(base, name)                                               \
	static_cast<name *>(                                                       \
		HelperFactory<base>::GetInstance().CreateClassByName(#name))

#define CREATE_CLASS_BY_STRING(base, name)                                     \
	HelperFactory<base>::GetInstance().CreateClassByName(name)

} // namespace terraformer

#endif