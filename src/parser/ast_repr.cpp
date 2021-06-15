/*
 * This file is a part of the Kithare programming language source code.
 * The source code for Kithare programming language is distributed under the MIT license.
 * Copyright (C) 2021 Kithare Organization
 */

#include <kithare/ast.hpp>


#define REPR_ALL_IN(var)  \
    for (auto _var : var) \
        if (_var)         \
    str += U"\n\t" + ind + kh::repr(*_var, indent + 1)

#define BODY_HEADER()                   \
    std::u32string ind;                 \
    ind.reserve(indent);                \
    for (size_t i = 0; i < indent; i++) \
        ind += '\t';                    \
                                        \
    std::u32string str;


std::u32string kh::repr(const kh::Ast& module_ast, const size_t indent) {
    std::u32string ind;
    ind.reserve(indent);
    for (size_t i = 0; i < indent; i++)
        ind += '\t';

    std::u32string str = U"ast:";

    REPR_ALL_IN(module_ast.imports);
    REPR_ALL_IN(module_ast.functions);
    REPR_ALL_IN(module_ast.user_types);
    REPR_ALL_IN(module_ast.enums);
    REPR_ALL_IN(module_ast.variables);
    return str;
}

std::u32string kh::repr(const kh::AstImport& import_ast, const size_t indent) {
    std::u32string ind;
    ind.reserve(indent);
    for (size_t i = 0; i < indent; i++)
        ind += '\t';

    std::u32string str = import_ast.is_include ? U"include:" : U"import:";
    str += U"\n\t" + ind + U"type: " + (import_ast.is_relative ? U"relative" : U"absolute");

    str += U"\n\t" + ind + U"path: ";
    for (const std::u32string& dir : import_ast.path)
        str += dir + (&dir == &import_ast.path.back() ? U"" : U".");

    if (!import_ast.is_include)
        str += U"\n\t" + ind + U"identifier: " + import_ast.identifier;

    return str;
}

std::u32string kh::repr(const kh::AstUserType& type_ast, const size_t indent) {
    std::u32string ind;
    ind.reserve(indent);
    for (size_t i = 0; i < indent; i++)
        ind += '\t';

    std::u32string str = (type_ast.is_class ? U"class:\n\t" : U"struct:\n\t") + ind + U"name: ";
    for (const std::u32string& identifier : type_ast.identifiers)
        str += identifier + (&identifier == &type_ast.identifiers.back() ? U"" : U".");

    if (!type_ast.bases.empty()) {
        str += U"\n\t" + ind + U"base(s):";
        for (const std::shared_ptr<kh::AstIdentifierExpression>& base : type_ast.bases)
            if (base)
                str += U"\n\t\t" + ind + kh::repr(*base, indent + 3);
    }

    if (!type_ast.generic_args.empty()) {
        str += U"\n\t" + ind + U"generic argument(s): ";
        for (const std::u32string& generic_ : type_ast.generic_args)
            str += generic_ + (&generic_ == &type_ast.generic_args.back() ? U"" : U", ");
    }

    str += U"\n\t" + ind + U"member(s):";
    for (auto member : type_ast.members)
        if (member)
            str += U"\n\t\t" + ind + kh::repr(*member, indent + 2);

    if (!type_ast.methods.empty()) {
        str += U"\n\t" + ind + U"method(s):";
        for (auto method : type_ast.methods)
            if (method)
                str += U"\n\t\t" + ind + kh::repr(*method, indent + 2);
    }

    return str;
}

std::u32string kh::repr(const kh::AstEnumType& enum_ast, const size_t indent) {
    std::u32string ind;
    ind.reserve(indent);
    for (size_t i = 0; i < indent; i++)
        ind += '\t';

    std::u32string str = U"enum:\n\t" + ind + U"name: ";
    for (const std::u32string& identifier : enum_ast.identifiers)
        str += identifier + (&identifier == &enum_ast.identifiers.back() ? U"" : U".");

    str += U"\n\t" + ind + U"member(s):";
    for (size_t member = 0; member < enum_ast.members.size(); member++)
        str += U"\n\t\t" + ind + enum_ast.members[member] + U": " + kh::repr(enum_ast.values[member]);

    return str;
}

std::u32string kh::repr(const kh::AstBody& body_ast, const size_t indent) {
    return body_ast.repr(indent);
}

std::u32string kh::AstBody::repr(const size_t indent) const {
    return U"[unknown body]";
}

std::u32string kh::AstExpression::repr(const size_t indent) const {
    return U"[unknown expression]";
}

std::u32string kh::AstIdentifierExpression::repr(const size_t indent) const {
    BODY_HEADER();

    for (const std::u32string& identifier : this->identifiers)
        str += identifier + (&identifier == &this->identifiers.back() ? U"" : U".");

    bool is_function = this->identifiers.size() == 1 && this->identifiers[0] == U"func";

    if (!this->generics.empty()) {
        str += U"!(";
        for (size_t i = 0; i < this->generics.size(); i++) {
            if (!this->generics[i])
                continue;

            for (size_t refs = 0; refs < this->generics_refs[i]; refs++)
                str += U"ref ";
            str += kh::repr(*(this->generics[i]), indent);

            if (this->generics_array[i].size() && this->generics_array[i][0] != 0) {
                for (size_t d = 0; d < this->generics_array[i].size(); d++)
                    str += U"[" + kh::repr(this->generics_array[i][d]) + U"]";
            }

            if (is_function && i == 0)
                str += U"(";
            else if (i != this->generics.size() - 1)
                str += U", ";
        }
        str += is_function ? U"))" : U")";
    }

    return str;
}

std::u32string kh::AstUnaryExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"unary expression:";

    str += U"\n\t" + ind + U"operator: " + kh::repr(this->operation);

    if (this->rvalue)
        str += U"\n\t" + ind + U"rvalue:\n\t\t" + ind + kh::repr(*this->rvalue, indent + 2);

    return str;
}

std::u32string kh::AstBinaryExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"binary expression:";

    str += U"\n\t" + ind + U"operator: " + kh::repr(this->operation);

    if (this->lvalue)
        str += U"\n\t" + ind + U"lvalue:\n\t\t" + ind + kh::repr(*this->lvalue, indent + 2);

    if (this->rvalue)
        str += U"\n\t" + ind + U"rvalue:\n\t\t" + ind + kh::repr(*this->rvalue, indent + 2);

    return str;
}

std::u32string kh::AstTernaryExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"ternary expression:";

    if (this->condition)
        str += U"\n\t" + ind + U"condition:\n\t\t" + ind + kh::repr(*this->condition, indent + 2);

    if (this->value)
        str += U"\n\t" + ind + U"value:\n\t\t" + ind + kh::repr(*this->value, indent + 2);

    if (this->otherwise)
        str += U"\n\t" + ind + U"otherwise:\n\t\t" + ind + kh::repr(*this->otherwise, indent + 2);

    return str;
}

std::u32string kh::AstComparisonExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"comparison expression:\n\t" + ind + U"operation(s): ";

    for (const kh::Operator& operation : this->operations)
        str += kh::repr(operation) + (&operation == &this->operations.back() ? U"" : U",");

    str += U"\n\t" + ind + U"value(s):";
    for (std::shared_ptr<kh::AstExpression> value : this->values)
        if (value)
            str += U"\n\t\t" + ind + kh::repr(*value);

    return str;
}

std::u32string kh::AstSubscriptExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"subscript:";

    if (this->expression)
        str += U"\n\t" + ind + U"expression:\n\t\t" + ind + kh::repr(*this->expression, indent + 2);

    if (!this->arguments.empty()) {
        str += U"\n\t" + ind + U"argument(s):";
        for (auto argument : this->arguments)
            if (argument)
                str += U"\n\t\t" + ind + kh::repr(*argument, indent + 2);
    }

    return str;
}

std::u32string kh::AstCallExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"call:";

    if (this->expression)
        str += U"\n\t" + ind + U"expression:\n\t\t" + ind + kh::repr(*this->expression, indent + 2);

    if (!this->arguments.empty()) {
        str += U"\n\t" + ind + U"argument(s):";
        for (auto argument : this->arguments)
            if (argument)
                str += U"\n\t\t" + ind + kh::repr(*argument, indent + 2);
    }

    return str;
}

std::u32string kh::AstDeclarationExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"declare:";

    if (this->var_type) {
        str += U"\n\t" + ind + U"type: ";

        if (this->is_static)
            str += U"static ";
        if (!this->is_public)
            str += U"private ";

        for (size_t refs = 0; refs < this->refs; refs++)
            str += U"ref ";
        str += kh::repr(*this->var_type, indent + 1);

        if (this->var_array.size() && this->var_array[0] != 0) {
            for (const uint64_t dimension : this->var_array)
                str += U'[' + kh::repr(dimension) + U']';
        }
    }

    str += U"\n\t" + ind + U"name: " + this->var_name;

    if (this->expression)
        str += U"\n\t" + ind + U"initializer expression:\n\t\t" + ind +
               kh::repr(*this->expression, indent + 2);

    return str;
}

std::u32string kh::AstFunctionExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = this->is_conditional ? U"conditional function:" : U"function:";

    str += U"\n\t" + ind + U"static: " + (this->is_static ? U"true" : U"false");
    str += U"\n\t" + ind + U"access: " + (this->is_public ? U"public" : U"private");

    if (this->identifiers.empty())
        str += U"\n\t" + ind + U"name: (lambda)";
    else {
        str += U"\n\t" + ind + U"name: ";
        for (const std::u32string& identifier : this->identifiers)
            str += identifier + (&identifier == &this->identifiers.back() ? U"" : U".");

        if (!this->generic_args.empty()) {
            str += U"\n\t" + ind + U"generic argument(s): ";
            for (const std::u32string& generic_ : this->generic_args)
                str += generic_ + (&generic_ == &this->generic_args.back() ? U"" : U", ");
        }
    }

    if (this->return_type) {
        str += U"\n\t" + ind + U"return type: ";

        for (size_t refs = 0; refs < this->return_refs; refs++)
            str += U"ref ";
        str += kh::repr(*this->return_type, indent + 1);

        if (this->return_array.size() && this->return_array[0] != 0) {
            for (const uint64_t dimension : this->return_array)
                str += U'[' + kh::repr(dimension) + U']';
        }
    }

    str += U"\n\t" + ind + U"argument(s):";
    if (this->arguments.empty())
        str += U" [none]";
    for (auto arg : this->arguments)
        if (arg)
            str += U"\n\t\t" + ind + kh::repr(*arg, indent + 2);

    str += U"\n\t" + ind + U"body:";
    for (auto part : this->body)
        if (part)
            str += U"\n\t\t" + ind + kh::repr(*part, indent + 2);

    return str;
}

std::u32string kh::AstScopeExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"scoping (";

    if (!this->identifiers.empty()) {
        for (const std::u32string& identifier : this->identifiers)
            str += (&this->identifiers.back() == &identifier ? U"" : U".") + identifier;
    }

    str += U"):"; /* sad face */

    if (this->expression)
        str += U"\n\t" + ind + U"expression:\n\t\t" + ind + kh::repr(*this->expression, indent + 2);

    return str;
}

std::u32string kh::AstConstValue::repr(const size_t indent) const {
    BODY_HEADER();

    switch (this->value_type) {
        case kh::AstConstValue::ValueType::CHARACTER:
            str = U"character: " + this->character;
            break;

        case kh::AstConstValue::ValueType::UINTEGER:
            str = U"unsigned integer: " + kh::repr(this->uinteger);
            break;

        case kh::AstConstValue::ValueType::INTEGER:
            str = U"integer: " + kh::repr(this->integer);
            break;

        case kh::AstConstValue::ValueType::FLOATING:
            str = U"floating: " + kh::repr(this->floating);
            break;

        case kh::AstConstValue::ValueType::IMAGINARY:
            str = U"imaginary: " + kh::repr(this->imaginary) + U"i";
            break;

        case kh::AstConstValue::ValueType::BUFFER:
            str = U"buffer: " + kh::quote(this->buffer);
            break;

        case kh::AstConstValue::ValueType::STRING:
            str = U"string: " + kh::quote(this->string);
            break;

        default:
            str = U"[unknown constant]";
    }

    return str;
}

std::u32string kh::AstTupleExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"tuple:";

    if (this->elements.empty())
        str += U" [no elements]";
    else {
        for (auto element : this->elements)
            if (element)
                str += U"\n\t" + ind + kh::repr(*element, indent + 1);
    }

    return str;
}

std::u32string kh::AstListExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"list:";

    if (this->elements.empty())
        str += U" [no elements]";
    else {
        for (auto element : this->elements)
            if (element)
                str += U"\n\t" + ind + kh::repr(*element, indent + 1);
    }

    return str;
}

std::u32string kh::AstDictExpression::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"dict:";

    if (this->keys.empty())
        str += U" [no pairs]";
    else {
        for (size_t i = 0; i < this->keys.size(); i++) {
            str += U"\n\t" + ind + U"pair:";
            if (this->keys[i])
                str += U"\n\t\t" + ind + kh::repr(*this->keys[i], indent + 2);
            if (this->items[i])
                str += U"\n\t\t" + ind + kh::repr(*this->items[i], indent + 2);
        }
    }

    return str;
}

std::u32string kh::AstIf::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"if:";

    for (size_t clause = 0; clause < this->conditions.size(); clause++) {
        str += U"\n\t" + ind + U"if clause:";

        if (this->conditions[clause])
            str += U"\n\t\t" + ind + U"condition:\n\t\t\t" + ind +
                   kh::repr(*this->conditions[clause], indent + 3);

        if (!this->bodies[clause].empty()) {
            str += U"\n\t\t" + ind + U"body:";
            for (auto part : this->bodies[clause])
                if (part)
                    str += U"\n\t\t\t" + ind + kh::repr(*part, indent + 3);
        }
    }

    if (!this->else_body.empty()) {
        str += U"\n\t" + ind + U"else body:";
        for (auto part : this->else_body)
            if (part)
                str += U"\n\t\t" + ind + kh::repr(*part, indent + 2);
    }

    return str;
}

std::u32string kh::AstWhile::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"while:";

    if (this->condition)
        str += U"\n\t" + ind + U"condition:\n\t\t" + ind + kh::repr(*this->condition, indent + 2);

    if (!this->body.empty()) {
        str += U"\n\t" + ind + U"body:";
        for (auto part : this->body)
            if (part)
                str += U"\n\t\t" + ind + kh::repr(*part, indent + 2);
    }

    return str;
}

std::u32string kh::AstDoWhile::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"do while:";

    if (this->condition)
        str += U"\n\t" + ind + U"condition:\n\t\t" + ind + kh::repr(*this->condition, indent + 2);

    if (!this->body.empty()) {
        str += U"\n\t" + ind + U"body:";
        for (auto part : this->body)
            if (part)
                str += U"\n\t\t" + ind + kh::repr(*part, indent + 2);
    }

    return str;
}

std::u32string kh::AstFor::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"for:";

    if (this->initialize)
        str += U"\n\t" + ind + U"initializer:\n\t\t" + ind + kh::repr(*this->initialize, indent + 2);

    if (this->condition)
        str += U"\n\t" + ind + U"condition:\n\t\t" + ind + kh::repr(*this->condition, indent + 2);

    if (this->step)
        str += U"\n\t" + ind + U"step:\n\t\t" + ind + kh::repr(*this->step, indent + 2);

    if (!this->body.empty()) {
        str += U"\n\t" + ind + U"body:";
        for (auto part : this->body)
            if (part)
                str += U"\n\t\t" + ind + kh::repr(*part, indent + 2);
    }

    return str;
}

std::u32string kh::AstForEach::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"foreach:";

    if (this->target)
        str += U"\n\t" + ind + U"target:\n\t\t" + ind + kh::repr(*this->target, indent + 2);

    if (this->iterator)
        str += U"\n\t" + ind + U"iterator:\n\t\t" + ind + kh::repr(*this->iterator, indent + 2);

    if (!this->body.empty()) {
        str += U"\n\t" + ind + U"body:";
        for (auto part : this->body)
            if (part)
                str += U"\n\t\t" + ind + kh::repr(*part, indent + 2);
    }

    return str;
}

std::u32string kh::AstStatement::repr(const size_t indent) const {
    BODY_HEADER();
    str = U"statement: ";

    switch (this->statement_type) {
        case kh::AstStatement::Type::CONTINUE:
            str += U"continue";
            break;
        case kh::AstStatement::Type::BREAK:
            str += U"break";
            break;
        case kh::AstStatement::Type::RETURN:
            str += U"return";
            break;
        default:
            str += U"unknown";
            break;
    }

    if (this->statement_type == kh::AstStatement::Type::RETURN) {
        if (this->expression)
            str += U"\n\t" + ind + kh::repr(*this->expression, indent + 1);
    }
    else
        str += U" " + kh::repr((uint64_t)this->loop_count);

    return str;
}