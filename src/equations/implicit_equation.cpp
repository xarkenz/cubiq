#include "implicit_equation.h"

#include <cmath>


namespace Cubiq {

    /*ImplicitEquation::ImplicitEquation(DisplaySettings settings, std::function<float(float,float)> func) : Equation(settings) {
        function = std::move(func);
    }*/

    ImplicitEquation::ImplicitEquation(DisplaySettings settings, Parser::Expression expr) : Equation(settings) {
        expression = std::move(expr);
    }

    /*float ImplicitEquation::apply(float x, float y) const {
        return function(x,y);
    }*/

    float ImplicitEquation::apply(float x, float y) const {
        return eval(expression.getChildren().data(),x,y) - eval(expression.getChildren().data()+1,x,y);
    }

    float ImplicitEquation::eval(const Parser::Expression* expr, float x, float y) {
        if (expr->isEmpty()) {
            if (expr->getChildren().size() == 0) {return 0;}
            else {return eval(expr->getChildren().data(),x,y);} // TODO: idk if I need this
        }
        else if (expr->isNumber()) {return expr->getNumber();}
        else if (expr->isSymbol()) {
            std::string name = expr->getSymbol().name;
            if (name == "x") { return x; }
            else if (name == "y") { return y; }
            else {return 0;}
        }
        else if (expr->isOperation()) { // TODO: clean up
            switch(expr->getOperation()) {
                case Parser::Operation::ADD:
                    return eval(expr->getChildren().data(),x,y) + eval(expr->getChildren().data()+1,x,y);
                case Parser::Operation::SUB:
                    return eval(expr->getChildren().data(),x,y) - eval(expr->getChildren().data()+1,x,y);
                case Parser::Operation::MUL:
                    return eval(expr->getChildren().data(),x,y) * eval(expr->getChildren().data()+1,x,y);
                case Parser::Operation::DIV:
                    return eval(expr->getChildren().data(),x,y) / eval(expr->getChildren().data()+1,x,y);
                case Parser::Operation::POS:
                    return eval(expr->getChildren().data(),x,y);
                case Parser::Operation::NEG:
                    return -eval(expr->getChildren().data(),x,y);
                case Parser::Operation::EXP:
                    return powf(eval(expr->getChildren().data(),x,y), eval(expr->getChildren().data()+1,x,y));
                case Parser::Operation::SQRT:
                    return powf(eval(expr->getChildren().data()+1,x,y), 1.0f/eval(expr->getChildren().data(),x,y));
                case Parser::Operation::FACT:
                    return std::tgamma(1+eval(expr->getChildren().data(),x,y));
                case Parser::Operation::SIN:
                    return std::sinf(eval(expr->getChildren().data(),x,y));
                case Parser::Operation::COS:
                    return std::cosf(eval(expr->getChildren().data(),x,y));
                case Parser::Operation::TAN:
                    return std::tanf(eval(expr->getChildren().data(),x,y));
                case Parser::Operation::SEC:
                    return 1.0f/std::cosf(eval(expr->getChildren().data(),x,y));
                case Parser::Operation::CSC:
                    return 1.0f/std::sinf(eval(expr->getChildren().data(),x,y));
                case Parser::Operation::COT:
                    return 1.0f/std::tanf(eval(expr->getChildren().data(),x,y));
                default:
                    return 0; // TODO: account for more things
            }
        }
        return 0;
    }

    unsigned long ImplicitEquation::getNumVertices(BoundingBox boundingBox, double precision) const {
        int gridWidth = (int) (std::ceil(boundingBox.maxX / precision) - std::floor(boundingBox.minX / precision));
        int gridHeight = (int) (std::ceil(boundingBox.maxY / precision) - std::floor(boundingBox.minY / precision));
        if (gridWidth <= 0 or gridHeight <= 0) { return 0; }
        return 4 * gridWidth * gridHeight; // At most 4 vertices per cell
    }

    void ImplicitEquation::writeVertices(GLfloat* vertices, BoundingBox boundingBox, double precision) const {

        unsigned long numVerts = getNumVertices(boundingBox, precision);
        if (numVerts == 0) { return; }

        int gridWidth = (int) (std::ceil(boundingBox.maxX / precision) - std::floor(boundingBox.minX / precision));
        int gridHeight = (int) (std::ceil(boundingBox.maxY / precision) - std::floor(boundingBox.minY / precision));


        // Calculate values
        float** values = new float* [gridHeight + 1];

        float x, y;

        #pragma omp parallel for num_threads(Equation::NUM_THREADS) shared(gridHeight, gridWidth, values, precision, boundingBox) private(x, y) default(none)
        for (int yInd = 0; yInd < gridHeight + 1; yInd++) {
            values[yInd] = new float[gridWidth + 1];

            y = (float) ((std::floor(boundingBox.minY / precision) + (double) yInd) * precision);
            for (int xInd = 0; xInd < gridWidth + 1; xInd++) {
                x = (float) ((std::floor(boundingBox.minX / precision) + (double) xInd) * precision);
                values[yInd][xInd] = apply(x, y);
            }
        }

        // Create vertices
        int vertIndex;
        float tl, tr, bl, br;
        float l, r, b, t, c;
        float v1x, v1y, v2x, v2y, v3x, v3y, v4x, v4y;
        bool v1, v2, v3, v4;

        #pragma omp parallel for num_threads(Equation::NUM_THREADS) collapse(2) shared(vertices, gridHeight, gridWidth, values, precision, boundingBox) private(vertIndex, tl, tr, bl, br, l, r, b, t, c, v1x, v1y, v2x, v2y, v3x, v3y, v4x, v4y, v1, v2, v3, v4) default(none)
        for (int x = 0; x < gridWidth; x++) {
            for (int y = 0; y < gridHeight; y++) {

                vertIndex = (y * gridWidth + x) * 4;

                writeVertex(vertices, vertIndex, 0, 0);
                writeVertex(vertices, vertIndex + 1, 0, 0);
                writeVertex(vertices, vertIndex + 2, 0, 0);
                writeVertex(vertices, vertIndex + 3, 0, 0);

                tl = values[y + 1][x];
                tr = values[y + 1][x + 1];
                bl = values[y][x];
                br = values[y][x + 1];

                if ((tl > 0 and tr > 0 and bl > 0 and br > 0) or (tl < 0 and tr < 0 and bl < 0 and br < 0) or
                    (tl == 0 and tr == 0 and bl == 0 and br == 0)) {
                    continue;
                }

                v1 = v2 = v3 = v4 = false;

                l = (float) ((std::floor(boundingBox.minX / precision) + x) * precision);
                r = l + (float) precision;
                b = (float) ((std::floor(boundingBox.minY / precision) + y) * precision);
                t = b + (float) precision;


                if (bl == 0 and br == 0) {
                    writeVertex(vertices, vertIndex++, l, b);
                    writeVertex(vertices, vertIndex++, r, b);
                }
                if (tl == 0 and bl == 0) {
                    writeVertex(vertices, vertIndex++, l, t);
                    writeVertex(vertices, vertIndex++, l, b);
                }
                if (bl == 0 and tr == 0) {
                    writeVertex(vertices, vertIndex++, l, b);
                    writeVertex(vertices, vertIndex++, r, t);
                }
                if (tl == 0 and br == 0) {
                    writeVertex(vertices, vertIndex++, l, t);
                    writeVertex(vertices, vertIndex++, r, b);
                }


                if ((tl > 0 and tr < 0) or (tl < 0 and tr > 0)) {
                    v1 = true;
                    v1y = t;
                    v1x = (float) ((double) (tl / (tl - tr)) * precision) + l;
                }
                if ((bl > 0 and br < 0) or (bl < 0 and br > 0)) {
                    v2 = true;
                    v2y = b;
                    v2x = (float) ((double) (bl / (bl - br)) * precision) + l;
                }

                if ((bl > 0 and tl < 0) or (bl < 0 and tl > 0)) {
                    v3 = true;
                    v3x = l;
                    v3y = (float) ((double) (bl / (bl - tl)) * precision) + b;
                }

                if ((br > 0 and tr < 0) or (br < 0 and tr > 0)) {
                    v4 = true;
                    v4x = r;
                    v4y = (float) ((double) (br / (br - tr)) * precision) + b;
                }

                if (v1 and v2 and not v3 and not v4) {
                    writeVertex(vertices, vertIndex++, v1x, v1y);
                    writeVertex(vertices, vertIndex++, v2x, v2y);
                } else if (v1 and v3 and not v2 and not v4) {
                    writeVertex(vertices, vertIndex++, v1x, v1y);
                    writeVertex(vertices, vertIndex++, v3x, v3y);
                } else if (v1 and v4 and not v2 and not v3) {
                    writeVertex(vertices, vertIndex++, v1x, v1y);
                    writeVertex(vertices, vertIndex++, v4x, v4y);
                } else if (v2 and v3 and not v1 and not v4) {
                    writeVertex(vertices, vertIndex++, v2x, v2y);
                    writeVertex(vertices, vertIndex++, v3x, v3y);
                } else if (v2 and v4 and not v1 and not v3) {
                    writeVertex(vertices, vertIndex++, v2x, v2y);
                    writeVertex(vertices, vertIndex++, v4x, v4y);
                } else if (v3 and v4 and not v1 and not v2) {
                    writeVertex(vertices, vertIndex++, v3x, v3y);
                    writeVertex(vertices, vertIndex++, v4x, v4y);
                } else if (v1 and v2 and v3 and v4) {
                    c = apply((l + r) / 2, (b + t) / 2);
                    if (c * tl >= 0) {
                        writeVertex(vertices, vertIndex++, v1x, v1y);
                        writeVertex(vertices, vertIndex++, v4x, v4y);

                        writeVertex(vertices, vertIndex++, v2x, v2y);
                        writeVertex(vertices, vertIndex++, v3x, v3y);
                    } else {
                        writeVertex(vertices, vertIndex++, v1x, v1y);
                        writeVertex(vertices, vertIndex++, v3x, v3y);

                        writeVertex(vertices, vertIndex++, v2x, v2y);
                        writeVertex(vertices, vertIndex++, v4x, v4y);
                    }
                }

            }
        }

        for (int y = 0; y < gridHeight + 1; y++) {
            delete[] values[y];
        }
        delete[] values;

    }

}