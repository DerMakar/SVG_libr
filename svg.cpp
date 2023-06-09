#include "svg.h"

namespace svg {

using namespace std::literals;

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------
Circle& Circle::SetCenter(Point center)  {
    center_ = std::move(center);
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = std::move(radius);
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    out << "/>"sv;
}

// ---------- Polyline ------------------
Polyline& Polyline::AddPoint(Point point) {
    polyline_.push_back(std::move(point));
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    bool IsFirst = true;
    for (const auto& point : polyline_) {
        if (IsFirst) {
            IsFirst = false;
        }
        else {
            out << " "sv;
        }
        out << point.x;
        out << ","sv;
        out << point.y;
    }
    out << "\" />"sv;

}

// ---------- Text ------------------
Text& Text::SetPosition(Point pos) {
    pos_ = std::move(pos);
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = std::move(offset);
    return *this;
}

Text& Text::SetFontSize(uint32_t size) {
    size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = font_weight;
    return *this;
}


Text& Text::SetData(std::string data) {
    data_ = data;
    return *this;
}


void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text x=\""sv << pos_.x << "\" y=\""sv << pos_.y << "\" dx=\""sv;
    out << offset_.x << "\" dy=\""sv << offset_.y << "\" font-size=\""sv << size_ << "\""sv;
    if (!font_family_.empty()) {
        out << " font-family=\""sv << font_family_ << "\" "sv;
    }
    if (!font_weight_.empty()) {
        out << "font-weight=\""sv << font_weight_ << "\""sv;
    }
    out << ">"sv;
    if (!data_.empty()) {
        out << data_;
    }
    out << "</text>"sv;
    }


// ---------- Document ------------------
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(move(obj));
}


void Document::Render(std::ostream& out) const {
    auto svg = RenderContext(out,2,2);
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
    for (size_t i = 0; i < objects_.size(); ++i) {
        objects_[i]->Render(svg);
    }
    out << "</svg>"sv;
}
}  // namespace svg