#pragma once

#include "context.h"


namespace Cubiq::Parser {

    Expression generateParseTree(GraphContext& context, TokenIterator& it, DataType type, bool allowEmpty, bool allowComma = false);

}