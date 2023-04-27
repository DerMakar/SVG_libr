#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <deque>

namespace svg {

    struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};



class Circle final : public Object {
public: 
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
/*
AddPoint(Point p): добавляет вершину ломаной — элемент свойства points, записываемый в виде x,y и 
отделяемый пробелами от соседних элементов (см. примеры). Значение свойства по умолчанию — пустая строка.
*/
class Polyline final : public Object{
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

private:
    void RenderObject(const RenderContext& context) const override;
    std::deque<Point> polyline_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */

class Text final : public Object {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

private:
    void RenderObject(const RenderContext& context) const override;
    
    Point pos_; // координата опорной точки
    Point offset_; // смещение относительно опорной точки
    uint32_t size_ = 1; // свойства font-size — размер шрифта
    std::string font_weight_; // значение свойства font-weight — толщина шрифта
    std::string font_family_; // значение свойства font-family — название семейства шрифта
    std::string data_; // содержимое тега <text> - выводимый текст

};


// ObjectContainer задаёт интерфейс для доступа к контейнеру SVG-объектов.
// Через него Drawable-объекты могут визуализировать себя, добавляя в контейнер SVG - примитивы.
class ObjectContainer {
public:
    template <typename T>
    void Add(T obj) {
        AddPtr(std::make_unique<T>(std::move(obj)));
    }
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

    virtual ~ObjectContainer() = default;
};
/*
Интерфейс Drawable унифицирует работу с объектами, которые можно нарисовать, подключив SVG-библиотеку.
Для этого в нём есть метод Draw, принимающий ссылку на интерфейс ObjectContainer.
*/
class Drawable {
public:
    virtual void Draw(ObjectContainer& g) const = 0;

    virtual ~Drawable() = default;
};

/*
Класс Document должен реализовывать интерфейс svg::ObjectContainer. Это позволит создать композицию
из произвольных Drawable-объектов и создать на их основе SVG-документ
*/ 
class Document : public ObjectContainer {
public:
    Document() = default;
    
    void AddPtr(std::unique_ptr<Object>&& obj) override;
        
    void Render(std::ostream& out) const;

private:
    std::deque<std::unique_ptr<Object>> objects_;
};

}  // namespace svg