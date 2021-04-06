#include <dlfcn.h>
#include <memory>
#include <iostream>
#include <stdexcept>

using namespace std;

struct dlopen_deleter {
	void operator()(void* handle) {
		dlclose(handle);
	}
};
using dl_ptr = std::unique_ptr<void, dlopen_deleter>;

void noop(void*) {}

class Plugin {
	using initialize_fp = void*(*)();
	using destroy_fp = void(*)(void*);

	using display_ptr = std::unique_ptr<void, destroy_fp>;
public:
	Plugin(const char* name):
		plugin_library{dlopen(name, RTLD_NOW)} {
		if(!plugin_library) {
			std::cerr << dlerror() << std::endl;
			throw std::runtime_error{"Failed to open library!"};
		}
		initialize_display = load_function<initialize_fp>("initialize_display");
		destroy_display = load_function<destroy_fp>("destroy_display");

		auto* d = initialize_display();
		display = {d, destroy_display};
	}

private:
	dl_ptr plugin_library;
	initialize_fp initialize_display = {};
	destroy_fp destroy_display = {};
	display_ptr display = {nullptr, noop};

	template<typename F>
	F load_function(const char* name) {
		F ptr = reinterpret_cast<F>(dlsym(plugin_library.get(), name));
		if(!ptr){
			std::cerr << dlerror() << std::endl;
			throw std::runtime_error{"Failed to lookup function!"};
		}
		return ptr;
	}
};

int main(int argc, char* argv[]) {
	Plugin sdl_plugin{"plugins/libsdl_display.so"};
	std::cout << "successfully loaded plugin" << std::endl;
}
