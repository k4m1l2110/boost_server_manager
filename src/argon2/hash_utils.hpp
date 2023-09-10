
#ifndef HASH_UTILS
#define HASH_UTILS
#include <iostream>
#include <argon2.h>
#include <openssl/rand.h>


char* generate_random(size_t length) {
    char *random_bytes = (char *)malloc(length);
    if (random_bytes == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }

    if (RAND_bytes((unsigned char *)random_bytes, length) != 1) {
        perror("Failed to generate random bytes");
        exit(1);
    }

    return random_bytes;
}

std::string to_hex_string(const uint8_t* data, size_t size) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < size; ++i) {
        ss << std::setw(2) << static_cast<unsigned int>(data[i]);
    }
    return ss.str();
}

bool get_hash(const std::string& value, uint8_t* salt, uint8_t* dest) {
    // Set Argon2 parameters (you can adjust these based on your requirements)

    uint8_t *pwd = (uint8_t *)strdup(value.c_str());
    uint32_t pwdlen = strlen((char *)pwd);

    uint32_t t_cost = 2;            // 2-pass computation
    uint32_t m_cost = (1<<16);      // 64 mebibytes memory usage
    uint32_t parallelism = 1;       // number of threads and lanes

    // high-level API

    // low-level API
    argon2_context context = {
        dest,  /* output array, at least HASHLEN in size */
        HASHLEN, /* digest length */
        pwd, /* password array */
        pwdlen, /* password length */
        salt,  /* salt array */
        SALTLEN, /* salt length */
        NULL, 0, /* optional secret data */
        NULL, 0, /* optional associated data */
        t_cost, m_cost, parallelism, parallelism,
        ARGON2_VERSION_13, /* algorithm version */
        NULL, NULL, /* custom memory allocation / deallocation functions */
        /* by default only internal memory is cleared (pwd is not wiped) */
        ARGON2_DEFAULT_FLAGS
    };
    
    int rc = argon2i_ctx( &context );
    if(ARGON2_OK != rc) {
        printf("Error: %s\n", argon2_error_message(rc));
        return 0;
    }
    else
        return 1;
}

bool verify_password(const std::string& password, const uint8_t* stored_hash,uint8_t* salt) {
    uint8_t hash[HASHLEN];
    uint8_t *pwd = (uint8_t *)strdup(password.c_str());
    uint32_t pwdlen = strlen((char *)pwd);

    uint32_t t_cost = 2;            // 2-pass computation
    uint32_t m_cost = (1<<16);      // 64 mebibytes memory usage
    uint32_t parallelism = 1;       // number of threads and lanes

    // low-level API
    argon2_context context = {
        hash,  /* output array, at least HASHLEN in size */
        HASHLEN, /* digest length */
        pwd, /* password array */
        pwdlen, /* password length */
        salt,  /* salt array */
        SALTLEN, /* salt length */
        NULL, 0, /* optional secret data */
        NULL, 0, /* optional associated data */
        t_cost, m_cost, parallelism, parallelism,
        ARGON2_VERSION_13, /* algorithm version */
        NULL, NULL, /* custom memory allocation / deallocation functions */
        /* by default only internal memory is cleared (pwd is not wiped) */
        ARGON2_DEFAULT_FLAGS
    };

    free(pwd);
    int rc = argon2i_ctx(&context);
    if (ARGON2_OK != rc) {
        printf("Error: %s\n", argon2_error_message(rc));
        return false;
    }

    // Compare the computed hash with the stored hash
    if (memcmp(hash, stored_hash, HASHLEN) == 0) {
        return true; // Passwords match
    } else {
        return false; // Passwords do not match
    }
}

#endif