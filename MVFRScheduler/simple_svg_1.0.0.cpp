/*******************************************************************************
*  The "New BSD License" : http://www.opensource.org/licenses/bsd-license.php  *
********************************************************************************

Copyright (c) 2010, Mark Turney
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/

#ifndef SIMPLE_SVG_HPP
#define SIMPLE_SVG_HPP

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <iostream>
#include "simple_svg_1.0.0.h"

namespace svg {
    // Utility XML/String Functions.
    std::string elemStart(std::string const &element_name) {
        return "\t<" + element_name + " ";
    }

    std::string elemEnd(std::string const &element_name) {
        return "</" + element_name + ">\n";
    }

    std::string emptyElemEnd() {
        return "/>\n";
    }

    //Dimensions
    Dimensions::Dimensions(double width, double height) : width(width), height(height) { }

    Dimensions::Dimensions(double combined) : width(combined), height(combined) { }


    Point::Point(double x, double y) : x(x), y(y) { }

    optional<Point> getMinPoint(std::vector<Point> const &points) {
        if (points.empty())
            return optional<Point>();

        Point min = points[0];
        for (unsigned i = 0; i < points.size(); ++i) {
            if (points[i].x < min.x)
                min.x = points[i].x;
            if (points[i].y < min.y)
                min.y = points[i].y;
        }
        return optional<Point>(min);
    }

    optional<Point> getMaxPoint(std::vector<Point> const &points) {
        if (points.empty())
            return optional<Point>();

        Point max = points[0];
        for (unsigned i = 0; i < points.size(); ++i) {
            if (points[i].x > max.x)
                max.x = points[i].x;
            if (points[i].y > max.y)
                max.y = points[i].y;
        }
        return optional<Point>(max);
    }

    // Defines the dimensions, scale, origin, and origin offset of the document.
    Layout::Layout(Dimensions const &dimensions, Origin origin,
                   double scale, Point const &origin_offset)
            : dimensions(dimensions), scale(scale), origin(origin), origin_offset(origin_offset) { }

    // Convert coordinates in user space to SVG native space.
    double translateX(double x, Layout const &layout) {
        if (layout.origin == Layout::BottomRight || layout.origin == Layout::TopRight)
            return layout.dimensions.width - ((x + layout.origin_offset.x) * layout.scale);
        else
            return (layout.origin_offset.x + x) * layout.scale;
    }

    double translateY(double y, Layout const &layout) {
        if (layout.origin == Layout::BottomLeft || layout.origin == Layout::BottomRight)
            return layout.dimensions.height - ((y + layout.origin_offset.y) * layout.scale);
        else
            return (layout.origin_offset.y + y) * layout.scale;
    }

    double translateScale(double dimension, Layout const &layout) {
        return dimension * layout.scale;
    }

    Serializeable::Serializeable() { }

    Serializeable::~Serializeable() { };

    Color::Color(int r, int g, int b) : transparent(false), red(r), green(g), blue(b) { }

    Color::Color(Defaults color) : transparent(false), red(0), green(0), blue(0) {
        switch (color) {
            case Aqua:
                assign(0, 255, 255);
                break;
            case Black:
                assign(0, 0, 0);
                break;
            case Blue:
                assign(0, 0, 255);
                break;
            case Brown:
                assign(165, 42, 42);
                break;
            case Cyan:
                assign(0, 255, 255);
                break;
            case Fuchsia:
                assign(255, 0, 255);
                break;
            case Green:
                assign(0, 128, 0);
                break;
            case Lime:
                assign(0, 255, 0);
                break;
            case Magenta:
                assign(255, 0, 255);
                break;
            case Orange:
                assign(255, 165, 0);
                break;
            case Purple:
                assign(128, 0, 128);
                break;
            case Red:
                assign(255, 0, 0);
                break;
            case Silver:
                assign(192, 192, 192);
                break;
            case White:
                assign(255, 255, 255);
                break;
            case Yellow:
                assign(255, 255, 0);
                break;
            default:
                transparent = true;
                break;
        }
    }

    Color::~Color() { }

    std::string Color::toString(Layout const &) const {
        std::stringstream ss;
        if (transparent)
            ss << "transparent";
        else
            ss << "rgb(" << red << "," << green << "," << blue << ")";
        return ss.str();
    }

    void Color::assign(int r, int g, int b) {
        red = r;
        green = g;
        blue = b;
    }

    Fill::Fill(Color::Defaults color) : color(color) { }

    Fill::Fill(Color color) : color(color) { }

    std::string Fill::toString(Layout const &layout) const {
        std::stringstream ss;
        ss << attribute("fill", color.toString(layout));
        return ss.str();
    }

    Stroke::Stroke(double width, Color color,
                   std::vector<unsigned int> dasharray)
            : width(width), color(color), dasharray(dasharray) { }

    std::string Stroke::toString(Layout const &layout) const {
        // If stroke width is invalid.
        if (width < 0)
            return std::string();

        std::stringstream ss;
        ss << attribute("stroke-width", translateScale(width, layout)) << attribute("stroke", color.toString(layout));
        if (!dasharray.empty()) {
            std::stringstream ss2;
            for (std::vector<unsigned int>::const_iterator it = dasharray.begin(); it != dasharray.end(); it++) {
                if (it == dasharray.begin()) {
                    ss2 << *it;
                }
                else {
                    ss2 << " " << *it;
                }
            }
            ss << attribute("stroke-dasharray", ss2.str());
        }
        return ss.str();
    }

    Font::Font(double size, std::string const &family) : size(size), family(family) { }

    std::string Font::toString(Layout const &layout) const {
        std::stringstream ss;
        ss << attribute("font-size", translateScale(size, layout)) << attribute("font-family", family);
        return ss.str();
    }


    ShapeRendering::ShapeRendering(ShapeRenderingOptions const &shapeRendering)
            : shapeRendering(shapeRendering) { };

    std::string ShapeRendering::toString(Layout const &layout) const {
        std::stringstream ss;
        switch (shapeRendering) {
            case AUTO:
                ss << "";
                break;
            case OPTIMIZESPEED:
                ss << attribute("shape-rendering", "optimizeSpeed");
                break;
            case CRISPEDGES:
                ss << attribute("shape-rendering", "crispEdges");
                break;
            case GEOMETRICPRECISION:
                ss << attribute("shape-rendering", "geometricPrecision");
                break;
            case INHERIT:
                ss << attribute("shape-rendering", "inherit");
                break;
            default:
                ss << "";
                break;
        }
        return ss.str();
    }


    Shape::Shape(Fill const &fill, Stroke const &stroke,
                 ShapeRenderingOptions const &shapeRendering)
            : fill(fill), stroke(stroke), shapeRendering(ShapeRendering(shapeRendering)) { }

    Shape::~Shape() { }

    Circle::Circle(Point const &center, double diameter, Fill const &fill, Stroke const &stroke)
            : Shape(fill, stroke), center(center), radius(diameter / 2) { }

    std::string Circle::toString(Layout const &layout) const {
        std::stringstream ss;
        ss << elemStart("circle") << attribute("cx", translateX(center.x, layout))
        << attribute("cy", translateY(center.y, layout))
        << attribute("r", translateScale(radius, layout)) << fill.toString(layout)
        << stroke.toString(layout) << emptyElemEnd();
        return ss.str();
    }

    void Circle::offset(Point const &offset) {
        center.x += offset.x;
        center.y += offset.y;
    }

    Elipse::Elipse(Point const &center, double width, double height,
                   Fill const &fill, Stroke const &stroke)
            : Shape(fill, stroke), center(center), radius_width(width / 2),
              radius_height(height / 2) { }

    std::string Elipse::toString(Layout const &layout) const {
        std::stringstream ss;
        ss << elemStart("ellipse") << attribute("cx", translateX(center.x, layout))
        << attribute("cy", translateY(center.y, layout))
        << attribute("rx", translateScale(radius_width, layout))
        << attribute("ry", translateScale(radius_height, layout))
        << fill.toString(layout) << stroke.toString(layout) << emptyElemEnd();
        return ss.str();
    }

    void Elipse::offset(Point const &offset) {
        center.x += offset.x;
        center.y += offset.y;
    }

    Rectangle::Rectangle(Point const &edge, double width, double height,
                         Fill const &fill, Stroke const &stroke)
            : Shape(fill, stroke), edge(edge), width(width),
              height(height) { }

    std::string Rectangle::toString(Layout const &layout) const {
        std::stringstream ss;
        ss << elemStart("rect") << attribute("x", translateX(edge.x, layout))
        << attribute("y", translateY(edge.y, layout))
        << attribute("width", translateScale(width, layout))
        << attribute("height", translateScale(height, layout))
        << fill.toString(layout) << stroke.toString(layout) << emptyElemEnd();
        return ss.str();
    }

    void Rectangle::offset(Point const &offset) {
        edge.x += offset.x;
        edge.y += offset.y;
    }


    Line::Line(Point const &start_point, Point const &end_point,
               Stroke const &stroke, ShapeRenderingOptions const &shapeRendering)
            : Shape(Fill(), stroke, shapeRendering), start_point(start_point),
              end_point(end_point) { }

    std::string Line::toString(Layout const &layout) const {
        std::stringstream ss;
        ss << elemStart("line") << attribute("x1", translateX(start_point.x, layout))
        << attribute("y1", translateY(start_point.y, layout))
        << attribute("x2", translateX(end_point.x, layout))
        << attribute("y2", translateY(end_point.y, layout))
        << stroke.toString(layout) << shapeRendering.toString(layout) << emptyElemEnd();
        return ss.str();
    }

    void Line::offset(Point const &offset) {
        start_point.x += offset.x;
        start_point.y += offset.y;

        end_point.x += offset.x;
        end_point.y += offset.y;
    }

    Polygon::Polygon(Fill const &fill, Stroke const &stroke,
                     ShapeRenderingOptions const &shapeRendering)
            : Shape(fill, stroke, shapeRendering) { }

    Polygon::Polygon(Stroke const &stroke) : Shape(Color::Transparent, stroke) { }

    Polygon &Polygon::operator<<(Point const &point) {
        points.push_back(point);
        return *this;
    }

    std::string Polygon::toString(Layout const &layout) const {
        std::stringstream ss;
        ss << elemStart("polygon");

        ss << "points=\"";
        for (unsigned i = 0; i < points.size(); ++i)
            ss << translateX(points[i].x, layout) << "," << translateY(points[i].y, layout) << " ";
        ss << "\" ";

        ss << fill.toString(layout) << stroke.toString(layout) << shapeRendering.toString(layout) << emptyElemEnd();
        return ss.str();
    }

    void Polygon::offset(Point const &offset) {
        for (unsigned i = 0; i < points.size(); ++i) {
            points[i].x += offset.x;
            points[i].y += offset.y;
        }
    }

    Polyline::Polyline(Fill const &fill, Stroke const &stroke)
            : Shape(fill, stroke) { }

    Polyline::Polyline(Stroke const &stroke) : Shape(Color::Transparent, stroke) { }

    Polyline::Polyline(std::vector<Point> const &points,
                       Fill const &fill, Stroke const &stroke)
            : Shape(fill, stroke), points(points) { }

    Polyline &Polyline::operator<<(Point const &point) {
        points.push_back(point);
        return *this;
    }

    std::string Polyline::toString(Layout const &layout) const {
        std::stringstream ss;
        ss << elemStart("polyline");

        ss << "points=\"";
        for (unsigned i = 0; i < points.size(); ++i)
            ss << translateX(points[i].x, layout) << "," << translateY(points[i].y, layout) << " ";
        ss << "\" ";

        ss << fill.toString(layout) << stroke.toString(layout) << emptyElemEnd();
        return ss.str();
    }

    void Polyline::offset(Point const &offset) {
        for (unsigned i = 0; i < points.size(); ++i) {
            points[i].x += offset.x;
            points[i].y += offset.y;
        }
    }


    Text::Text(Point const &origin, std::string const &content, Fill const &fill,
               Font const &font, Stroke const &stroke)
            : Shape(fill, stroke), origin(origin), content(content), font(font) { }

    std::string Text::toString(Layout const &layout) const {
        std::stringstream ss;
        ss << elemStart("text") << attribute("x", translateX(origin.x, layout))
        << attribute("y", translateY(origin.y, layout))
        << fill.toString(layout) << stroke.toString(layout) << font.toString(layout)
        << ">" << content << elemEnd("text");
        return ss.str();
    }

    void Text::offset(Point const &offset) {
        origin.x += offset.x;
        origin.y += offset.y;
    }

    // Sample charting class.
    LineChart::LineChart(Dimensions margin, double scale,
                         Stroke const &axis_stroke)
            : axis_stroke(axis_stroke), margin(margin), scale(scale) { }

    LineChart &LineChart::operator<<(Polyline const &polyline) {
        if (polyline.points.empty())
            return *this;

        polylines.push_back(polyline);
        return *this;
    }

    std::string LineChart::toString(Layout const &layout) const {
        if (polylines.empty())
            return "";

        std::string ret;
        for (unsigned i = 0; i < polylines.size(); ++i)
            ret += polylineToString(polylines[i], layout);

        return ret + axisString(layout);
    }

    void LineChart::offset(Point const &offset) {
        for (unsigned i = 0; i < polylines.size(); ++i)
            polylines[i].offset(offset);
    }

    optional<Dimensions> LineChart::getDimensions() const {
        if (polylines.empty())
            return optional<Dimensions>();

        optional<Point> min = getMinPoint(polylines[0].points);
        optional<Point> max = getMaxPoint(polylines[0].points);
        for (unsigned i = 0; i < polylines.size(); ++i) {
            if (getMinPoint(polylines[i].points)->x < min->x)
                min->x = getMinPoint(polylines[i].points)->x;
            if (getMinPoint(polylines[i].points)->y < min->y)
                min->y = getMinPoint(polylines[i].points)->y;
            if (getMaxPoint(polylines[i].points)->x > max->x)
                max->x = getMaxPoint(polylines[i].points)->x;
            if (getMaxPoint(polylines[i].points)->y > max->y)
                max->y = getMaxPoint(polylines[i].points)->y;
        }

        return optional<Dimensions>(Dimensions(max->x - min->x, max->y - min->y));
    }

    std::string LineChart::axisString(Layout const &layout) const {
        optional<Dimensions> dimensions = getDimensions();
        if (!dimensions)
            return "";

        // Make the axis 10% wider and higher than the data points.
        double width = dimensions->width * 1.1;
        double height = dimensions->height * 1.1;

        // Draw the axis.
        Polyline axis(Color::Transparent, axis_stroke);
        axis << Point(margin.width, margin.height + height) << Point(margin.width, margin.height)
        << Point(margin.width + width, margin.height);

        return axis.toString(layout);
    }

    std::string LineChart::polylineToString(Polyline const &polyline, Layout const &layout) const {
        Polyline shifted_polyline = polyline;
        shifted_polyline.offset(Point(margin.width, margin.height));

        std::vector<Circle> vertices;
        for (unsigned i = 0; i < shifted_polyline.points.size(); ++i)
            vertices.push_back(Circle(shifted_polyline.points[i], getDimensions()->height / 30.0, Color::Black));

        return shifted_polyline.toString(layout) + vectorToString(vertices, layout);
    }

    EmbeddedFont::EmbeddedFont(const std::string fontFileName) : fontFileName(fontFileName) { }

    std::string EmbeddedFont::toString(Layout const &layout) const {
        std::ifstream fontFile(fontFileName);
        std::string line;
        std::stringstream ss;
        while (getline(fontFile, line)) {
            ss << line << std::endl;
        }
        fontFile.close();
        return ss.str();
    }

    Document::Document(std::string const &file_name, Layout layout)
            : file_name(file_name), layout(layout) { }

    Document &Document::operator<<(Serializeable const &shape) {
        body_nodes_str += shape.toString(layout);
        return *this;
    }

    std::string Document::toString() const {
        std::stringstream ss;
        ss << "<?xml " << attribute("version", "1.0") << attribute("standalone", "no")
        << "?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
        << "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n<svg "
        << attribute("width", layout.dimensions.width, "px")
        << attribute("height", layout.dimensions.height, "px")
        << attribute("xmlns", "http://www.w3.org/2000/svg")
        << attribute("version", "1.1") << ">\n" << body_nodes_str << elemEnd("svg");
        return ss.str();
    }

    bool Document::save() const {
        std::ofstream ofs(file_name.c_str());
        if (!ofs.good())
            return false;

        ofs << toString();
        ofs.close();
        return true;
    }
}

#endif
