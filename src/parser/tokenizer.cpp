#include "tokenizer.h"

#include "defs.h"


namespace Cubiq::Parser {

    bool operator==(const Symbol& s1, const Symbol& s2) {
        return s1.name == s2.name;
    }

    bool operator!=(const Symbol& s1, const Symbol& s2) {
        return s1.name != s2.name;
    }


    bool operator==(const Empty&, const Empty&) {
        return true;
    }

    bool operator!=(const Empty&, const Empty&) {
        return false;
    }


    Token::Token(Token::Value val) : value(std::move(val)) {
    }

    bool Token::isNumber() const {
        return std::holds_alternative<Number>(value);
    }
	
	bool Token::isSymbol() const {
		return std::holds_alternative<Symbol>(value);
	}

    bool Token::isEmpty() const {
		return std::holds_alternative<Empty>(value);
	}

    const Token::Value& Token::getValue() const {
        return value;
    }
	
	const Symbol& Token::getSymbol() const {
		return std::get<Symbol>(value);
	}

    Number Token::getNumber() const {
		return std::get<Number>(value);
	}

    bool Token::hasValue(Token::Value val) const {
        return value == val;
    }


    std::string Token::toString() const {
        if (isEmpty()) {
            return "<EMPTY>";
        } else if (isSymbol()) {
            return getSymbol().name;
        } else if (isNumber()) {
            return std::to_string(getNumber());
        }
        return "<?>";
    }


    namespace {

        int firstChar = 0;

        Token grabSymbol(CharStream& stream, int c) {
            std::string word;

            if (c == '\\') {
                word += c;
                c = stream();
                while (std::isalpha(c)) {
                    word += c;
                    c = stream();
                }

                if ((word == "\\" && c == ' ')
                        || (word == "\\left" && (c == '(' || c == '[' || c == '{'))
                        || (word == "\\right" && (c == ')' || c == ']' || c == '}'))) {
                    word += c;
                    c = stream();
                }
            } else {
                word += c;
                c = stream();
        
                if ((word == "<" && c == '<') || (word == ">" && c == '>')) {
                    word += c;
                    c = stream();
                }
            }

            firstChar = c;
            return {Symbol{word}};
        }


        Token grabNumber(CharStream& stream, int c) {
            std::string word;
            if (firstChar == '.') word += ".";
            bool isInteger = c != '.' && firstChar != '.';

            do {
                word.push_back(char(c));
                c = stream();
                if (c == '.') {
                    if (isInteger) isInteger = false;
                    else throw Error{ErrorType::UNEXPECTED, "."};
                }
            } while (c == '.' || std::isdigit(c));

            firstChar = c;

            return {Number(std::strtod(word.c_str(), nullptr))};
        }


        Token tokenize(CharStream& stream) {
            if (firstChar == 0) firstChar = stream();

            while (true) {
                if (firstChar < 0) {
                    return {Empty()};
                } else if (std::isspace(firstChar)) {
                    firstChar = stream();
                    continue;
                } else if (std::isdigit(firstChar)) {
                    return grabNumber(stream, firstChar);
                } else if (firstChar == '.') {
                    int c = stream();
                    if (std::isdigit(c)) return grabNumber(stream, c);
                    firstChar = c;
                    return {Symbol{"."}};
                } else {
                    return grabSymbol(stream, firstChar);
                }
            }
        }

    }


    TokenIterator::TokenIterator(CharStream& stream) : charStream(stream), current(Empty()) {
        ++(*this);
    }

    TokenIterator& TokenIterator::operator++() {
        current = tokenize(charStream);
        return *this;
    }

    const Token& TokenIterator::operator*() const {
        return current;
    }

    const Token* TokenIterator::operator->() const {
        return &current;
    }

    TokenIterator::operator bool() const {
        return !current.isEmpty();
    }

}