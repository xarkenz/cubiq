#pragma once


namespace Cubiq {

    struct BoundingBox {

        float minX, maxX, minY, maxY;

        [[nodiscard]] float width() const { return maxX - minX; }
        [[nodiscard]] float height() const { return maxY - minY; }
        [[nodiscard]] float centerX() const { return 0.5f * (maxX + minX); }
        [[nodiscard]] float centerY() const { return 0.5f * (maxY + minY); }

        [[nodiscard]] BoundingBox moved(float dx, float dy) const {
            return {minX + dx, maxX + dx, minY + dy, maxY + dy};
        }

    };

}