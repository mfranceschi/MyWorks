//
// Created by mfran on 25/12/2019.
//

#ifndef MYWORKS_TEST0_ARGUMENTSPARSER_HPP
#define MYWORKS_TEST0_ARGUMENTSPARSER_HPP

#include <exception>
#include <map>
#include <string>

class InvalidMultiItemsArgument : std::exception
{
public:
    inline explicit InvalidMultiItemsArgument(int index) :
            std::exception(),
            indexOfProblematicItem(index) {}
    const char* what() const noexcept override;

protected:
    int indexOfProblematicItem;
};

class UnfinishedMultiItemsArgument : InvalidMultiItemsArgument
{
public:
    inline explicit UnfinishedMultiItemsArgument(int index) :
        InvalidMultiItemsArgument(index) {}
    const char* what() const noexcept override;
};

enum class ValueType {
    BOOL = 1,
    INTEGER = 2,
    LIST_STRING = 3
};

union ValueUnion {
    int entier;
    bool bouleen;
    //std::string str;
};

struct Argument {
    ValueUnion value;
    bool present;
    ValueType valueType;
};

struct ArgumentParameter {
    const char* name = nullptr;
    unsigned short minOccurs = 1; // Minimum required number of occurrences of the argument.
    unsigned short maxOccurs = 1; // Maximum required number of occurrences of the argument.
    unsigned short minFollowingParameters = 1;
    unsigned short maxFollowingParameters = 1;
};

class ArgumentsParser {
public:
    // INNER TYPES
    using _Results = std::map<const char*, Argument*>;

    // PUBLIC METHODS
    const ArgumentsParser& parse(int nargs, const char **the_args); // Parses the user input and returns current instance.
    inline const _Results& get() const; // Returns the results.
    void free(); // Call this to free the resources from the results.

    // OPERATORS OVERLOADING

    // CONSTRUCTORS AND DESTRUCTOR
    ArgumentsParser() = default;
    ArgumentsParser(const ArgumentsParser&) = delete;
    ArgumentsParser(const ArgumentsParser&&) = delete;
    ArgumentsParser& operator= (const ArgumentsParser&) = delete;
    ArgumentsParser&& operator= (const ArgumentsParser&&) = delete;
    virtual ~ArgumentsParser();

protected:
    // PROTECTED METHODS
    void parseInternal(int begin, int end, const char **the_args);

    // PROTECTED ATTRIBUTES
    _Results results;
};

// OTHER DEFINITIONS
const ArgumentsParser::_Results &ArgumentsParser::get() const {
    return results;
}

#endif //MYWORKS_TEST0_ARGUMENTSPARSER_HPP
