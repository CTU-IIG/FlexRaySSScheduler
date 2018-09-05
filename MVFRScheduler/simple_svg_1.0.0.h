//
// Created by jenik on 10/6/15.
//

#ifndef MVFRSCHEDULER_SIMPLE_SVG_1_0_0_H
#define MVFRSCHEDULER_SIMPLE_SVG_1_0_0_H

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <iostream>

namespace svg {
    // Utility XML/String Functions.
    template<typename T>
    std::string attribute(std::string const &attribute_name,
                          T const &value, std::string const &unit = "") {
        std::stringstream ss;
        ss << attribute_name << "=\"" << value << unit << "\" ";
        return ss.str();
    }

    std::string elemStart(std::string const &element_name);

    std::string elemEnd(std::string const &element_name);

    std::string emptyElemEnd();

    enum ShapeRenderingOptions {
        AUTO, OPTIMIZESPEED, CRISPEDGES, GEOMETRICPRECISION, INHERIT
    };

    //  Quick optional return type.  This allows functions to return an invalid
    //  value if no good return is possible.  The user checks for validity
    //  before using the returned value.
    template<typename T>
    class optional {
    public:
        optional<T>(T const &type)
                : valid(true), type(type) { }

        optional<T>() : valid(false), type(T()) { }

        T *operator->() {
            // If we try to access an invalid value, an exception is thrown.
            if (!valid)
                throw std::exception();

            return &type;
        }

        // Test for validity.
        bool operator!() const { return !valid; }

    private:
        bool valid;
        T type;
    };

    struct Dimensions {
        Dimensions(double width, double height);

        Dimensions(double combined = 0);

        double width;
        double height;
    };

    struct Point {
        Point(double x = 0, double y = 0);

        double x;
        double y;
    };

    optional<Point> getMinPoint(std::vector<Point> const &points);

    optional<Point> getMaxPoint(std::vector<Point> const &points);

    // Defines the dimensions, scale, origin, and origin offset of the document.
    struct Layout {
        enum Origin {
            TopLeft, BottomLeft, TopRight, BottomRight
        };

        Layout(Dimensions const &dimensions = Dimensions(400, 300), Origin origin = BottomLeft,
               double scale = 1, Point const &origin_offset = Point(0, 0));

        Dimensions dimensions;
        double scale;
        Origin origin;
        Point origin_offset;
    };

    // Convert coordinates in user space to SVG native space.
    double translateX(double x, Layout const &layout);

    double translateY(double y, Layout const &layout);

    double translateScale(double dimension, Layout const &layout);

    class Serializeable {
    public:
        Serializeable();

        virtual ~Serializeable();

        virtual std::string toString(Layout const &layout) const = 0;
    };

    class Color : public Serializeable {
    public:
        enum Defaults {
            Transparent = -1, Aqua, Black, Blue, Brown, Cyan, Fuchsia,
            Green, Lime, Magenta, Orange, Purple, Red, Silver, White, Yellow
        };

        Color(int r, int g, int b);

        Color(Defaults color);

        virtual ~Color();

        std::string toString(Layout const &) const;

    private:
        bool transparent;
        int red;
        int green;
        int blue;

        void assign(int r, int g, int b);
    };

    class Fill : public Serializeable {
    public:
        Fill(Color::Defaults color);

        Fill(Color color = Color::Transparent);

        std::string toString(Layout const &layout) const;

    private:
        Color color;
    };

    class Stroke : public Serializeable {
    public:
        Stroke(double width = -1, Color color = Color::Transparent,
               std::vector<unsigned int> dasharray = std::vector<unsigned int>());

        std::string toString(Layout const &layout) const;

    private:
        double width;
        Color color;
        std::vector<unsigned int> dasharray;
    };

    class Font : public Serializeable {
    public:
        Font(double size = 12, std::string const &family = "Verdana");

        std::string toString(Layout const &layout) const;

    private:
        double size;
        std::string family;
    };

    class ShapeRendering : public Serializeable {
    public:
        ShapeRendering(ShapeRenderingOptions const &shapeRendering = AUTO);

        virtual std::string toString(Layout const &layout) const;

    private:
        ShapeRenderingOptions shapeRendering;
    };

    class Shape : public Serializeable {
    public:
        Shape(Fill const &fill = Fill(), Stroke const &stroke = Stroke(),
              ShapeRenderingOptions const &shapeRendering = AUTO);

        virtual ~Shape();

        virtual std::string toString(Layout const &layout) const = 0;

        virtual void offset(Point const &offset) = 0;

    protected:
        Fill fill;
        Stroke stroke;
        ShapeRendering shapeRendering;
    };

    template<typename T>
    std::string vectorToString(std::vector<T> collection, Layout const &layout) {
        std::string combination_str;
        for (unsigned i = 0; i < collection.size(); ++i)
            combination_str += collection[i].toString(layout);

        return combination_str;
    }

    class Circle : public Shape {
    public:
        Circle(Point const &center, double diameter, Fill const &fill,
               Stroke const &stroke = Stroke());

        std::string toString(Layout const &layout) const;

        void offset(Point const &offset);

    private:
        Point center;
        double radius;
    };

    class Elipse : public Shape {
    public:
        Elipse(Point const &center, double width, double height,
               Fill const &fill = Fill(), Stroke const &stroke = Stroke());

        std::string toString(Layout const &layout) const;

        void offset(Point const &offset);

    private:
        Point center;
        double radius_width;
        double radius_height;
    };

    class Rectangle : public Shape {
    public:
        Rectangle(Point const &edge, double width, double height,
                  Fill const &fill = Fill(), Stroke const &stroke = Stroke());

        std::string toString(Layout const &layout) const;

        void offset(Point const &offset);

    private:
        Point edge;
        double width;
        double height;
    };

    class Line : public Shape {
    public:
        Line(Point const &start_point, Point const &end_point,
             Stroke const &stroke = Stroke(), ShapeRenderingOptions const &shapeRendering = AUTO);

        std::string toString(Layout const &layout) const;

        void offset(Point const &offset);

    private:
        Point start_point;
        Point end_point;
    };

    class Polygon : public Shape {
    public:
        Polygon(Fill const &fill = Fill(), Stroke const &stroke = Stroke(),
                ShapeRenderingOptions const &shapeRendering = AUTO);

        Polygon(Stroke const &stroke = Stroke());

        Polygon &operator<<(Point const &point);

        std::string toString(Layout const &layout) const;

        void offset(Point const &offset);

    private:
        std::vector<Point> points;
    };

    class Polyline : public Shape {
    public:
        Polyline(Fill const &fill = Fill(), Stroke const &stroke = Stroke());

        Polyline(Stroke const &stroke = Stroke());

        Polyline(std::vector<Point> const &points,
                 Fill const &fill = Fill(), Stroke const &stroke = Stroke());

        Polyline &operator<<(Point const &point);

        std::string toString(Layout const &layout) const;

        void offset(Point const &offset);

        std::vector<Point> points;
    };

    class Text : public Shape {
    public:
        Text(Point const &origin, std::string const &content, Fill const &fill = Fill(),
             Font const &font = Font(), Stroke const &stroke = Stroke());

        std::string toString(Layout const &layout) const;

        void offset(Point const &offset);

    private:
        Point origin;
        std::string content;
        Font font;
    };

    // Sample charting class.
    class LineChart : public Shape {
    public:
        LineChart(Dimensions margin = Dimensions(), double scale = 1,
                  Stroke const &axis_stroke = Stroke(.5, Color::Purple));

        LineChart &operator<<(Polyline const &polyline);

        std::string toString(Layout const &layout) const;

        void offset(Point const &offset);

    private:
        Stroke axis_stroke;
        Dimensions margin;
        double scale;
        std::vector<Polyline> polylines;

        optional<Dimensions> getDimensions() const;

        std::string axisString(Layout const &layout) const;

        std::string polylineToString(Polyline const &polyline, Layout const &layout) const;
    };

    class EmbeddedFont : public Serializeable {
    public:
        EmbeddedFont(const std::string fontFileName);

        std::string toString(Layout const &layout) const;

    protected:
        std::string fontFileName;
    };

    class Document {
    public:
        Document(std::string const &file_name, Layout layout = Layout());

        Document &operator<<(Serializeable const &shape);

        std::string toString() const;

        bool save() const;

    private:
        std::string file_name;
        Layout layout;

        std::string body_nodes_str;
    };
}

#endif //MVFRSCHEDULER_SIMPLE_SVG_1_0_0_H
