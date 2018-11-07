#pragma once
#include <bits/stdc++.h>
#include <cxxabi.h>
#define tell(args...) {\
    std::string s_args = #args;\
    replace(s_args.begin(), s_args.end(), ',', ' ');\
    std::stringstream arg_names(s_args);\
    std::istream_iterator<std::string> it(arg_names);\
    std::cerr << "[" << ++counter << ", "<< __LINE__ << ", " << __FILE__ << "] ";\
    __tell(it,args);\
}
int counter = 0;
template<class T>
std::string get_type(const T& a) {
    int stt;
    char* tmp = abi::__cxa_demangle(typeid(a).name(), 0, 0, &stt);
    std::string res(tmp);
    free(tmp);
    return res;
}
template<class T>
void __tell(const std::string& name, const T& a) {
    std::cerr << "[" << get_type(a) << "] " << name << " = " << a;
}
template<class T>
void __tell(const std::istream_iterator<std::string>& a_name, const T& a) {
    __tell(*a_name, a);
    std::cerr << "\n";
}
template<class T, class... Ts>
void __tell(std::istream_iterator<std::string> a_name, const T& a, const Ts&...args) {
    __tell(*a_name, a);
    std::cerr << ", ";
    __tell(++a_name, args...);
}

#define say teller(__LINE__, __FILE__)
#define sayo teller(__LINE__, __FILE__, std::cout)
class teller {
    static std::map<std::pair<size_t, std::string>, int> count;
    std::ostream& out;
    size_t line;
    const char* file;
    bool start = true;
    
    public:
        teller(size_t line, const char* file, std::ostream& out = std::cerr) : out(out), line(line), file(file) {
            out << "[" << ++count[std::pair<size_t, std::string>(line, file)] << ", " << line << ", " << file << "] ";
        }
        
        template <class T>
        teller& operator()(const T& obj, std::string label = "_") noexcept {
            if (!start) {
                out << ", ";
            }
            start = false;
            out << "[" << get_type(obj) << "] " << label << " = " << obj;
            return *this;
        }
    ~teller() {
        out << "\n";
    }
};
std::map<std::pair<size_t, std::string>, int> teller::count;
