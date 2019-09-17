/* Martin Franceschi - MyWorks package - Toolbox module.
 *
 * 
 */

 //---------- Interface of module <Toolbox> (file Toolbox.h) 
#if ! defined ( TOOLBOX_H )
#define TOOLBOX_H 

//--------------------------------------------------------------- Includes
#include <functional>
#include <sstream>

namespace Toolbox
{
	//------------------------------------------------------------------ Types

	// Simple "istringstream" extension for reading C-string
	// without copy and without end character.
	class InCharArrayStream : public std::istringstream
	{
	public:
		// The full string and the size.
		// Reading content[size-1] is ok, reading content[size] is undefined behavior.
		InCharArrayStream(const char* content, size_t size);

	protected:
		struct InCharArrayBuffer : public std::stringbuf
		{
			InCharArrayBuffer(const char* content, size_t size);
		};
		InCharArrayBuffer icab;
	};

	//-------------------------------------------------------------- Constants
	
	constexpr int INFERIOR = -1, EQUAL = 0, SUPERIOR = +1; // For comparisons.

	//////////////////////////////////////////////////////////////////  PUBLIC
	//------------------------------------------------------- Public functions

	// Returns the same C-string but converted in wchar_t*.
	// It is created using new[] --> please use delete[] after use!
	// Returns nullptr in case of failure.
	wchar_t* ToWchar_t(const char* source);

	// Returns the execution time for the given function.
	// It executes iter times and returns (total_time/iter).
	double Timethis(size_t iter, const std::function<void(void)>&);

	// Returns the sign of the given number: -1, 0 or +1, as an int.
	template <typename type = long long>
	constexpr int Sign(type nbr);

	// Returns the value if it is valid or throws given object.
	template <typename value_type, typename exception_class>
	constexpr value_type Validate(value_type new_value, const exception_class& value_to_throw, const std::function<bool(value_type)> validator);

	// Overloading of "Validate" for checking "new_value" is in the right range [min,max].
	template <typename value_type, typename exception_class>
	constexpr value_type Validate(value_type new_value, const exception_class& value_to_throw, value_type min, value_type max);

}

//------------------------------------------------------ Other definitions
template <typename type>
constexpr int Toolbox::Sign(type nbr)
{
	return !nbr ?
		0 : nbr < static_cast<type>(0) ?
		-1 :
		+1;
}

template <typename value_type, typename exception_class>
constexpr value_type Toolbox::Validate(value_type new_value, const exception_class& value_to_throw, const std::function<bool(value_type)> validator)
{
	if (validator(new_value))
		return new_value;
	else
		throw value_to_throw;
}

template <typename value_type, typename exception_class>
constexpr value_type Toolbox::Validate(value_type new_value, const exception_class& value_to_throw, value_type min, value_type max)
{
	std::function<bool(value_type)> func = [&min, &max](value_type newvalue) -> bool {
		return !(newvalue < min || newvalue > max);
	};

	return Toolbox::Validate(
		new_value, 
		value_to_throw, 
		func
	);
}

#endif // TOOLBOX_H 
