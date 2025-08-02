/**
 * @file TranspilerEngine.h
 * @brief Core transpiler engine for NOTAL to Pascal conversion
 * 
 * This file defines the main transpiler interface for converting NOTAL
 * language source code to Pascal code. The transpiler engine coordinates
 * the lexical analysis, parsing, and code generation phases to produce
 * equivalent Pascal code from NOTAL input.
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */

#ifndef TRANSPILER_H
#define TRANSPILER_H

#include <string>

/**
 * @brief Transpile NOTAL code to Pascal code
 * 
 * This function represents the main entry point for the transpiler engine.
 * It takes NOTAL source code as input and returns the equivalent Pascal
 * code. The transpilation process includes lexical analysis, syntax parsing,
 * semantic analysis, and code generation.
 * 
 * @param notal_code The NOTAL source code to transpile
 * @return The equivalent Pascal source code
 * 
 * @throws std::runtime_error If transpilation fails due to syntax or semantic errors
 * 
 * @author GATE Project Team
 * @version 1.0
 * @date 2025
 */
std::string transpile(const std::string& notal_code);

#endif // TRANSPILER_H
