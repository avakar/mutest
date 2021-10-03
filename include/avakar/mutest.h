#include <exception>

namespace avakar {
namespace mutest {

// Declares a new test case with the specified description text. The text
// will be shown in case the test case fails. It doesn't have to be unique.
//
// Test case is considered successful when it completes without throwing
// an exception. Use `chk` macro to conditionally throw `check_failed`.
//
//     mutest_case("it should honk") {
//         goose untitled;
//         untitled.press('Y');
//         chk untitled.honked();
//     }
#define mutest_case(name) _avakar_mutest_case(name)

// The macro `chk` examines the subsequent expression and, if false,
// throws a `check_failed` exception.
#define chk ::avakar::mutest::_assertion_checker(__FILE__, __LINE__),

// Runs all registered test cases. Failed tests are reported to stdout.
//
// You can call this directly from `main`.
//
//     int main(int argc, char * argv[]) {
//         return avakar::mutest::run(argc, argv);
//     }
//
// No command-line options are currently recognized.
int run(int argc, char const * const argv[]);

// Thrown by the `chk` macro when the checked expression is false.
// You usually don't have to deal with the exception directly, but it is
// public API so that you can catch it if necessary.
struct check_failed
	: std::exception
{
	explicit check_failed(char const * file, int line) noexcept
		: _file(file), _line(line)
	{
	}

	char const * what() const noexcept
	{
		return "mutest check failed";
	}

	char const * file() const noexcept
	{
		return _file;
	}

	int line() const noexcept
	{
		return _line;
	}

private:
	char const * _file;
	int _line;
};



struct _test_registrar;
extern _test_registrar * _test_registry;

struct _test_registrar
{
	_test_registrar(char const * name, void (*fn)()) noexcept
		: name(name), fn(fn)
	{
		next = _test_registry;
		_test_registry = this;
	}

	char const * name;
	void (*fn)();
	_test_registrar * next;
};

#define _avakar_mutest_pp_cat2(a, b) a ## b
#define _avakar_mutest_pp_cat(a, b) _avakar_mutest_pp_cat2(a, b)

#define _avakar_mutest_testcase(id, name) \
	static void id(); \
	static ::avakar::mutest::_test_registrar _avakar_mutest_pp_cat(id, _registrar){name, &id}; \
	static void id()

#define _avakar_mutest_case(name) _avakar_mutest_testcase(_avakar_mutest_pp_cat(_mutest_case_, __COUNTER__), name)

struct _assertion_checker
{
	explicit _assertion_checker(char const * file, int line) noexcept
		: _file(file), _line(line)
	{
	}

	template <typename E>
	void operator,(E && e)
	{
		if (!e)
			throw mutest::check_failed(_file, _line);
	}

private:
	char const * _file;
	int _line;
};

}
}

#pragma once
