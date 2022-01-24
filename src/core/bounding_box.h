#pragma once

struct BoundingBox {
    float minX, maxX, minY, maxY;

    float width() {return maxX - minX;}
    float height() {return maxY - minY;}
    float centerX() {return 0.5f*(maxX+minX);}
    float centerY() {return 0.5f*(maxY+minY);}
};
