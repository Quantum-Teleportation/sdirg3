#ifndef HELPER_H
#define HELPER_H

#include <sstream>
#include <string>

#define DEFINE_BASE(name)                                                      \
	class name {                                                               \
	public:                                                                    \
		inline std::string Serialize() const {                                 \
			std::stringstream out;                                             \
			out << "[" #name "]\n";                                            \
			Serialize_(out);                                                   \
			out << "[/" #name "]\n";                                           \
			return out.str();                                                  \
		}                                                                      \
                                                                               \
		virtual ~name() = 0;                                                   \
                                                                               \
	private:                                                                   \
		virtual void Serialize_(std::stringstream &) const = 0;                \
	};                                                                         \
	inline name::~name() {}

// struct Configurer {
//     enum {
//         INTEGER,
//         REAL,
//         STRING,
//         ENUMERATION,
//         REFERENCE
//     } type;

//     std::variant<std::vector<std::string>, > ;
// };

#endif