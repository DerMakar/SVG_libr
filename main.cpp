// program.exe > greeting.svg 
#define _USE_MATH_DEFINES
#include "svg.h"

#include <cmath>

using namespace std::literals;
using namespace svg;

namespace shapes{
class Triangle : public svg::Drawable {
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {
    }

    // Реализует метод Draw интерфейса svg::Drawable
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }

private:
    svg::Point p1_, p2_, p3_;
};

/*
N-лучевая звезда, задаваемая координатами центра, внешним и внутренним радиусами и количеством лучей.
Для рисования звезды используйте готовую функцию CreateStar, чтобы тренажёр мог сравнить результат с ожидаемым:
*/
svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
    using namespace svg;
    Polyline polyline;
    for (int i = 0; i <= num_rays; ++i) {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({ center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle) });
        if (i == num_rays) {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({ center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle) });
    }
    return polyline;
}

class Star : public svg::Drawable {
public:
    Star(svg::Point center, double outer_rad, double inner_rad, int num_rays)
        : center_(center)
        , outer_rad_(outer_rad)
        , inner_rad_(inner_rad)
        , num_rays_(num_rays) {
    }
    
    void Draw(svg::ObjectContainer& container) const override {
        svg::Polyline star = CreateStar(center_, outer_rad_, inner_rad_, num_rays_);
        container.Add(star);
    }

private:
    svg::Point center_;
    double outer_rad_, inner_rad_;
    int num_rays_;
};

/*
Класс Snowman — снеговик, состоящий из трех кругов. Задаётся координатами центра головы и её радиусом. Размеры и положение оставшихся частей
центр второго в 2r, а второй 1,5r, центр третьего в 3r и 2r радиусом
*/
class Snowman : public svg::Drawable {
public:
    Snowman(svg::Point head, double head_rad)
        : head_(head)
        , head_rad_(head_rad) {
    }
    void Draw(svg::ObjectContainer& container) const override {
        Point legs = { head_.x, head_.y + 5 * head_rad_ };
        Point body = { head_.x, head_.y + 2 * head_rad_ };
        container.Add(svg::Circle().SetCenter(legs).SetRadius(2 * head_rad_));
        container.Add(svg::Circle().SetCenter(body).SetRadius(1.5 * head_rad_));
        container.Add(svg::Circle().SetCenter(head_).SetRadius(head_rad_));
    }

private:
    svg::Point head_;
    double head_rad_ = 1.0;
}; 

} // namespace shapes
template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}

int main() {
    using namespace svg;
    using namespace shapes;
    using namespace std;

    vector<unique_ptr<svg::Drawable>> picture;

    picture.emplace_back(make_unique<Triangle>(Point{ 100, 20 }, Point{ 120, 50 }, Point{ 80, 40 }));
    // 5-лучевая звезда с центром {50, 20}, длиной лучей 10 и внутренним радиусом 4
    picture.emplace_back(make_unique<Star>(Point{ 50.0, 20.0 }, 10.0, 4.0, 5));
    // Снеговик с "головой" радиусом 10, имеющей центр в точке {30, 20}
    picture.emplace_back(make_unique<Snowman>(Point{ 30, 20 }, 10.0));

    svg::Document doc;
    // Так как документ реализует интерфейс ObjectContainer,
    // его можно передать в DrawPicture в качестве цели для рисования
    DrawPicture(picture, doc);

    // Выводим полученный документ в stdout
    doc.Render(cout);
}