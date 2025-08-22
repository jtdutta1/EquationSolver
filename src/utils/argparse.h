#include <string>
#include <exception>
using std::string;
using std::exception;

struct Args {
    string eq;
    string fname;
};

class ArgParseException : public exception {
    private:
        string message;
    
    public:
        ArgParseException(const char* msg) : message(msg) {}

        const char* what() const noexcept{
            return message.c_str();
        }
};

Args parse_args(int, char*[]);
void display_usage();