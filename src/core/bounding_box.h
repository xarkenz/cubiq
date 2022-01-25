#pragma once

struct BoundingBox {

    float minX, maxX, minY, maxY;

    float width() const { return maxX - minX; }
    float height() const { return maxY - minY; }
    float centerX() const { return 0.5f * (maxX + minX); }
    float centerY() const { return 0.5f * (maxY + minY); }

    BoundingBox moved(float dx, float dy) const {
        return {minX + dx, maxX + dx, minY + dy, maxY + dy};
    }

};
