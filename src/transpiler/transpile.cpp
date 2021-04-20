/*
 * This file is a part of the Kithare programming language source code.
 * The source code for Kithare programming language is distributed under the MIT license.
 * Copyright (C) 2021 Kithare Organization
 *
 * src/transpiler/transpile.cpp
 * Defines include/transpiler/transpile.hpp.
 */

#include "transpiler/transpile.hpp"


#define TRANSPILE_ALL_IN(var) \
    for (auto _var : var)     \
        if (_var)             \
    str += kh::transpile(*_var)

std::u32string kh::transpile(const kh::Ast& module_ast) {
    std::u32string str;
    TRANSPILE_ALL_IN(module_ast.imports);
    TRANSPILE_ALL_IN(module_ast.variables);
    TRANSPILE_ALL_IN(module_ast.enums);
    TRANSPILE_ALL_IN(module_ast.structs);
    TRANSPILE_ALL_IN(module_ast.functions);
    TRANSPILE_ALL_IN(module_ast.classes);

    str += U"int main(const int argc, char* argv[]) { return kh_main(); }\n";
    return str;
}

std::u32string kh::transpile(const kh::AstImport& import_ast) {
    std::u32string str = U"#include \"";

    for (const std::u32string& pth : import_ast.path)
        str += pth + U"/";

    if (str.back() == U'/')
        str.pop_back();

    str += U".kh.hpp\"\n";

    if (import_ast.is_include) {
        // TODO
    }
    else
        str += U"auto kh_" + import_ast.identifier + U" = kh_" + import_ast.path[0] + U"_mod();\n";

    return str;
}

std::u32string kh::transpile(const kh::AstClass& class_ast) {
    std::u32string str = U"class kh_" + class_ast.name;
    if (class_ast.base) {
        str += U" : kh_";
        str += kh::transpile(*class_ast.base);
    }
    str += U" {";

    if (!class_ast.generic_args.empty()) {} // TODO: HANDLE THIS

    for (auto member : class_ast.members) {
        str += U"\n";
        str += kh::transpile(*member);
        str += U";";
    }

    for (auto method : class_ast.methods) {
        str += U"\n";
        str += kh::transpile(*method);
    }

    str += U"};\n";

    return str;
}

std::u32string kh::transpile(const kh::AstStruct& struct_ast) {
    std::u32string str = U"struct kh_" + struct_ast.name;
    if (struct_ast.base)
        str += U" : kh_" + kh::transpile(*struct_ast.base);

    str += U" {";

    for (auto member : struct_ast.members) {
        str += U"\n";
        str += kh::transpile(*member);
    }

    str += U"};\n";
    return str;
}

std::u32string kh::transpile(const kh::AstEnum& enum_ast) {
    std::u32string str = U"enum kh_" + enum_ast.name;

    if (enum_ast.base) {} // TODO

    str += U" {";
    for (auto member : enum_ast.members) {
        str += U"\n";
        str += member;
        str += U",";
    }

    str += U"}\n";

    return str;
}

std::u32string kh::transpile(const kh::AstBody& ast) {
    std::u32string str;

    switch (ast.type) {
        case kh::AstBody::Type::EXPRESSION: {
            str += kh::transpile(*(kh::AstExpression*)&ast);
            str += U";\n";
            break;
        }

        case kh::AstBody::Type::IF: {
            const kh::AstIf& ast_if = *(kh::AstIf*)&ast;
            str += U"if (";

            for (size_t clause = 0; clause < ast_if.conditions.size(); clause++) {
                if (ast_if.conditions[clause])
                    str += kh::transpile(*ast_if.conditions[clause]);

                str += U") {\n";
                for (auto part : ast_if.bodies[clause]) {
                    if (part)
                        str += kh::transpile(*part);
                }
                str += U"} \nelse if (";
            }

            str += U"1) {\n";
            for (auto part : ast_if.else_body) {
                if (part)
                    str += kh::transpile(*part);
            }
            str += U"}\n";
            break;
        }

        case kh::AstBody::Type::WHILE: {
            const kh::AstWhile& ast_while = *(kh::AstWhile*)&ast;
            str += U"while (";

            if (ast_while.condition)
                str += kh::transpile(*ast_while.condition);

            str += U") {\n";
            for (auto part : ast_while.body) {
                if (part)
                    str += kh::transpile(*part);
            }
            str += U"}\n";
            break;
        }

        case kh::AstBody::Type::DO_WHILE: {
            const kh::AstDoWhile& ast_do_while = *(kh::AstDoWhile*)&ast;
            str += U"do {\n";

            for (auto part : ast_do_while.body) {
                if (part)
                    str += kh::transpile(*part);
            }
            str += U"} while (";

            if (ast_do_while.condition)
                str += kh::transpile(*ast_do_while.condition);

            str += U");\n";
            break;
        }
        case kh::AstBody::Type::FOR: {
            const kh::AstFor& ast_for = *(kh::AstFor*)&ast;
            str += U"{\n";

            for (auto target : ast_for.targets) {
                if (target)
                    str += kh::transpile(*target);
            }
            str += U"while (";

            if (ast_for.iterator)
                str += kh::transpile(*ast_for.iterator);

            str += U".iterate(";

            for (auto target : ast_for.targets) {
                if (target) {
                    str += (*(kh::AstDeclarationExpression*)target.get()).var_name;
                    str += U", ";
                }
            }
            if (str.back() == U' ')
                str.pop_back();
            if (str.back() == U',')
                str.pop_back();

            str += U")) {\n";
            for (auto part : ast_for.body)
                if (part)
                    str += kh::transpile(*part);

            str += U"}}\n";
            break;
        }

        case kh::AstBody::Type::STATEMENT: {
            const kh::AstStatement& ast_statement = *(kh::AstStatement*)&ast;

            switch (ast_statement.statement_type) {
                case kh::AstStatement::Type::CONTINUE:
                    str += U"continue;\n";
                    break;
                case kh::AstStatement::Type::BREAK:
                    str += U"break;\n";
                    break;
                case kh::AstStatement::Type::RETURN:
                    str += U"return ";
                    break;
                default:
                    // Maybe raise error here?
                    break;
            }

            if (ast_statement.expression) {
                str += kh::transpile(*ast_statement.expression);
                str += U";\n";
            }

            break;
        }

            /*
            case kh::AstBody::Type::INSTRUCTION: {
                const kh::AstInstruction& ast_inst = *(kh::AstInstruction*)&ast;
                str += U"INSTRUCTION:";

                str += U"\n\t" + ind + U"OP NAME: " + ast_inst.op_name;

                for (auto& op_arg : ast_inst.op_arguments)
                    if (op_arg)
                        str += U"\n\t" + ind + U"OP ARGUMENT:\n\t\t" + ind + kh::repr(*op_arg, indent +
            2);

                break;
            }
            */

            // default: Maybe raise error here?
    }

    return str;
}

std::u32string kh::transpile(const kh::AstExpression& expr) {
    std::u32string str;

    switch (expr.expression_type) {
        case kh::AstExpression::ExType::IDENTIFIER: {
            const kh::AstIdentifierExpression& expr_id = *(kh::AstIdentifierExpression*)&expr;

            for (const std::u32string& identifier : expr_id.identifiers) {
                str += identifier;
                str += U".";
            }
            if (str.back() == U'.')
                str.pop_back();

            /* TODO
            if (!expr_id.generics.empty()) {
                str += U"\n\t" + ind + U"GENERICS:";
                for (auto generic_ : expr_id.generics)
                    if (generic_)
                        str += U"\n\t\t" + ind + kh::repr(*generic_, indent + 2);
            }
            */

            break;
        }

        case kh::AstExpression::ExType::UNARY: {
            const kh::AstUnaryExpression& expr_unary = *(kh::AstUnaryExpression*)&expr;
            str += kh::repr(expr_unary.operation);
            if (expr_unary.rvalue)
                str += kh::transpile(*expr_unary.rvalue);

            break;
        }

        case kh::AstExpression::ExType::BINARY: {
            const kh::AstBinaryExpression& expr_binary = *(kh::AstBinaryExpression*)&expr;
            if (expr_binary.lvalue)
                str += kh::transpile(*expr_binary.lvalue);

            str += kh::repr(expr_binary.operation);

            if (expr_binary.rvalue)
                str += kh::transpile(*expr_binary.rvalue);

            break;
        }

        case kh::AstExpression::ExType::TERNARY: {
            const kh::AstTernaryExpression& expr_ternary = *(kh::AstTernaryExpression*)&expr;
            if (expr_ternary.condition)
                str += kh::transpile(*expr_ternary.condition);

            str += U" ? ";
            if (expr_ternary.value)
                str += kh::transpile(*expr_ternary.value);

            str += U" : ";
            if (expr_ternary.otherwise)
                str += kh::transpile(*expr_ternary.otherwise);

            break;
        }

        case kh::AstExpression::ExType::SUBSCRIPT: {
            const kh::AstSubscriptExpression& expr_subscript = *(kh::AstSubscriptExpression*)&expr;
            if (expr_subscript.expression)
                str += kh::transpile(*expr_subscript.expression);

            str += U"[";
            for (auto argument : expr_subscript.arguments) {
                if (argument)
                    str += kh::transpile(*argument);
                str += U", ";
            }
            if (str.back() == U' ')
                str.pop_back();
            if (str.back() == U',')
                str.pop_back();
            str += U"]";
            break;
        }

        case kh::AstExpression::ExType::CALL: {
            const kh::AstCallExpression& expr_call = *(kh::AstCallExpression*)&expr;
            str += U"kh_";
            if (expr_call.expression)
                str += kh::transpile(*expr_call.expression);

            str += U"(";
            for (auto argument : expr_call.arguments) {
                if (argument)
                    str += kh::transpile(*argument);
                str += U", ";
            }
            if (str.back() == U' ')
                str.pop_back();
            if (str.back() == U',')
                str.pop_back();
            str += U")";
            break;
        }

        case kh::AstExpression::ExType::DECLARE: {
            const kh::AstDeclarationExpression& expr_declare = *(kh::AstDeclarationExpression*)&expr;
            // TODO Handle public/private and ref shit
            if (expr_declare.is_static)
                str += U"static ";

            if (expr_declare.var_type)
                str += kh::transpile(*expr_declare.var_type);

            str += U" ";
            str += expr_declare.var_name;

            if (expr_declare.expression) {
                str += U" = ";
                str += kh::transpile(*expr_declare.expression);
            }
            break;
        }

        case kh::AstExpression::ExType::FUNCTION: {
            const kh::AstFunctionExpression& expr_func = *(kh::AstFunctionExpression*)&expr;
            std::u32string rtype = kh::transpile(*expr_func.return_type);
            // TODO Handle public/private
            if (expr_func.is_static)
                str += U"static ";

            str += (rtype == U"" || rtype == U" ") ? U"void " : rtype + U" ";
            if (expr_func.identifiers.empty()) {
                // TODO
            }
            else {
                // TODO handle generics and multiple identifiers
                str += U"kh_";
                str += expr_func.identifiers[0];
            }

            str += U"(";
            for (auto arg : expr_func.arguments) {
                if (arg) {
                    str += kh::transpile(*arg);
                    str += U", ";
                }
            }
            if (str.back() == U' ')
                str.pop_back();
            if (str.back() == U',')
                str.pop_back();

            str += U") {\n";
            for (auto part : expr_func.body)
                if (part)
                    str += kh::transpile(*part);

            str += U"}\n";
            break;
        }

        case kh::AstExpression::ExType::SCOPE: {
            const kh::AstScopeExpression& expr_scope = *(kh::AstScopeExpression*)&expr;

            if (expr_scope.expression) {
                str += U"(";
                str += kh::transpile(*expr_scope.expression);
                str += U")";
            }

            for (const std::u32string& identifier : expr_scope.identifiers)
                str += U"." + identifier;

            break;
        }

        case kh::AstExpression::ExType::CONSTANT: {
            const kh::AstConstValue& expr_const = *(kh::AstConstValue*)&expr;
            switch (expr_const.value_type) {
                case kh::AstConstValue::ValueType::CHARACTER:
                    str += U"U" + kh::quote(expr_const.character);
                    break;

                case kh::AstConstValue::ValueType::UINTEGER:
                    str += kh::repr(expr_const.uinteger);
                    break;

                case kh::AstConstValue::ValueType::INTEGER:
                    str += kh::repr(expr_const.integer);
                    break;

                case kh::AstConstValue::ValueType::FLOATING:
                    str += kh::repr(expr_const.floating);
                    break;

                case kh::AstConstValue::ValueType::COMPLEX:
                    str += kh::repr(expr_const.complex);
                    break;

                case kh::AstConstValue::ValueType::BUFFER:
                    str += kh::quote(expr_const.buffer);
                    break;

                case kh::AstConstValue::ValueType::STRING:
                    str += U"U" + kh::quote(expr_const.string);
                    break;

                    // default: Maybe raise error here?
            }

            break;
        }

            /*
            case kh::AstExpression::ExType::TUPLE: {
                const kh::AstTupleExpression& expr_const = *(kh::AstTupleExpression*)&expr;
                str += U"TUPLE:";

                for (auto element : expr_const.elements)
                    if (element)
                        str += U"\n\t" + ind + kh::repr(*element, indent + 1);

                break;
            }
            */

            // default: Maybe raise error here?
    }

    return str;
}
