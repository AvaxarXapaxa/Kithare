/*
 * This file is a part of the Kithare programming language source code.
 * The source code for Kithare programming language is distributed under the MIT license.
 * Copyright (C) 2021 Kithare Organization
 *
 * include/transpiler/transpile.hpp
 */

#include "parser/ast.hpp"

namespace kh {
    std::u32string transpile(const kh::Ast& module_ast);
    std::u32string transpile(const kh::AstImport& import_ast);
    std::u32string transpile(const kh::AstClass& class_ast);
    std::u32string transpile(const kh::AstStruct& struct_ast);
    std::u32string transpile(const kh::AstEnum& enum_ast);
    std::u32string transpile(const kh::AstBody& ast);
    std::u32string transpile(const kh::AstExpression& expr);
}