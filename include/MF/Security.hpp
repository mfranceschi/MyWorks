//
// Created by mfran on 30/04/2020.
//

#ifndef MFRANCESCHI_CPPLIBRARIES_SECURITY_HPP
#define MFRANCESCHI_CPPLIBRARIES_SECURITY_HPP

#include <string>
#include <cstdint>

class Hash {
public:
    const char* toCString() const;
    std::string toString() const;
    std::size_t getDigestLength() const;
    virtual ~Hash();

    Hash(const Hash& copy) = delete;
    Hash& operator=(const Hash& other) = delete;

protected:
    explicit Hash(std::size_t digestLength);

    void internalCallComputeHash();
    virtual void internalComputeHash() = 0;

    const char* internalSource = nullptr;
    char* internalDigest = nullptr;
    const std::size_t internalDigestLength;
};

class MD5Hash : public Hash {
public:
    /**
     * Generates the MD5 hash of the given string.
     * @param source Source byte array to analyse.
     */
    explicit MD5Hash(const std::string& source);

protected:
    void internalComputeHash() override;
};

#endif //MFRANCESCHI_CPPLIBRARIES_SECURITY_HPP
