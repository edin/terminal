﻿#include <iostream>
#include <string>
#include <string_view>
#include <streambuf>
#include <iomanip>

/**
    @Author: Edin Omeragic

    @Version: v0.1 - Support for text and background basic colors and true colors
    @Version: v0.1 - Support for errasing text, cursor movement, scrolling
    @Version: v0.1 - Nesting output using enterLevel, exitLevel and resetLevel

    @ToDo: Reading response from terminal (DeviceStatus enum)

    @BasedOn: http://www.termsys.demon.co.uk/vtansi.htm
*/
namespace terminal
{
    class Scroll;
    class Cursor;
    enum class TextColor;
    enum class Attribute;
    enum class BackgroundColor;
    enum class EraseText;
    enum class TabControl;
    enum class Terminal;

    std::ostream& operator<<(std::ostream& out, const Scroll& scroll);
    std::ostream& operator<<(std::ostream& out, const Cursor& cursor);
    std::ostream& operator<<(std::ostream& out, TextColor color);
    std::ostream& operator<<(std::ostream& out, Attribute sequence);
    std::ostream& operator<<(std::ostream& out, BackgroundColor color);
    std::ostream& operator<<(std::ostream& out, EraseText value);
    std::ostream& operator<<(std::ostream& out, TabControl value);
    std::ostream& operator<<(std::ostream& out, Terminal terminal);

    enum class Terminal {
        ResetDevice,		   //<ESC>c
        EnableLineWrap,		   //<ESC>[7h
        DisableLineWrap		   //<ESC>[7l
    };

    /*
        Device Status
        The following codes are used for reporting terminal / display settings, and vary depending on the implementation :
        Query Device Code	    <ESC>[c                 Requests a Report Device Code response from the device.
        Report Device Code	    <ESC>[{code}0c          Generated by the device in response to Query Device Code request.
        Query Device Status	    <ESC>[5n                Requests a Report Device Status response from the device.
        Report Device OK	    <ESC>[0n                Generated by the device in response to a Query Device Status request; indicates that device is functioning correctly.
        Report Device Failure	<ESC>[3n                Generated by the device in response to a Query Device Status request; indicates that device is functioning improperly.
        Query Cursor Position	<ESC>[6n                Requests a Report Cursor Position response from the device.
        Report Cursor Position	<ESC>[{ROW}; {COLUMN}R  Generated by the device in response to a Query Cursor Position request; reports current cursor position.
    */
    enum class DeviceStatus {
        QueryDeviceCode,        //<ESC>[c
        ReportDeviceCode,       //<ESC>[{code}0c
        QueryDeviceStatus,      //<ESC>[{5n
        ReportDeviceOK,         //<ESC>[0n
        ReportDeviceFailure,    //<ESC>[3n
        QueryCursorPosition,    //<ESC>[6n
        ReportCursorPosition    //<ESC>[{ROW};{COLUMN}R
    };

    // Note: Not fully implemented (not sure how)
    // Note: Maybe it would be better to have DeviceQuery and DeviceStatus classes
    class Device {
    private:
        DeviceStatus status = DeviceStatus::QueryDeviceCode;
        int row = 0;
        int column = 0;
    public:
        Device(DeviceStatus status) : status(status) {}
        Device(DeviceStatus status, int row, int column) : status(status), row(row), column(column) {}
        static Device queryDeviceCode() { return Device(DeviceStatus::QueryDeviceCode); }
        static Device queryDeviceStatus() { return Device(DeviceStatus::QueryDeviceStatus); }
        static Device queryCursorPosition() { return Device(DeviceStatus::QueryCursorPosition); }

        static Device reportDeviceCode(int code) { return Device(DeviceStatus::ReportDeviceCode, code, 0); }
        static Device reportDeviceOK() { return Device(DeviceStatus::ReportDeviceOK); }
        static Device reportDeviceFailure() { return Device(DeviceStatus::ReportDeviceFailure); }
        static Device reportCursorPosition(int row, int col) { return Device(DeviceStatus::ReportCursorPosition, row, col); }

        DeviceStatus getStatus() { return status; }
        int getCount() { return row; }
        int getRow() { return row; }
        int getColumn() { return column; }
    };

    enum class TabControl {
        SetTab,              //<ESC>H
        ClearTab,            //<ESC>[g
        ClearAllTab          //<ESC>[3g
    };

    enum class EraseText {
        EraseEndOfLine,      //<ESC>[K
        EraseStartOfLine,    //<ESC>[1K
        EraseLine,			 //<ESC>[2K
        EraseDown,			 //<ESC>[J
        EraseUp,			 //<ESC>[1J
        EraseScreen,		 //<ESC>[2J
    };

    enum class TextColor
    {
        None = 0,
        Black = 30,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        Start = Black,
        End = White
    };

    enum class BackgroundColor
    {
        None = 0,
        Black = 40,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        Start = Black,
        End = White
    };

    enum class Attribute
    {
        Reset = 0,
        Bold = 1,
        Dim = 2,
        Underscore = 4,
        Blink = 5,
        Reverse = 7,
        Hidden = 8
    };

    enum class Aligment
    {
        Left,
        Center,
        Right
    };

    /*
        Cursor Control
        Cursor Home 		    <ESC>[{ROW};{COLUMN}H   Sets the cursor position where subsequent text will begin.
                                                        If no row/column parameters are provided (ie. <ESC>[H), the cursor will move to
                                                        the home position, at the upper left of the screen.

        Cursor Up		        <ESC>[{COUNT}A          Moves the cursor up by COUNT rows; the default count is 1.
        Cursor Down		        <ESC>[{COUNT}B          Moves the cursor down by COUNT rows; the default count is 1.
        Cursor Forward		    <ESC>[{COUNT}C          Moves the cursor forward by COUNT columns; the default count is 1.
        Cursor Backward		    <ESC>[{COUNT}D          Moves the cursor backward by COUNT columns; the default count is 1.
        Force Cursor Pos.	    <ESC>[{ROW};{COLUMN}f   Identical to Cursor Home.
        Save Cursor			    <ESC>[s					Save current cursor position.
        Unsave Cursor		    <ESC>[u					Restores cursor position after a Save Cursor.
        Save Cursor & Attrs	    <ESC>7					Save current cursor position.
        Restore Cursor & Attrs	<ESC>8				    Restores cursor position after a Save Cursor.
    */

    enum class CursorMovement {
        None = 0,
        Home = 'H',
        Up = 'A',
        Down = 'B',
        Forward = 'C',
        Backward = 'D',
        ForcePosition = 'f',
        SaveCursor = 's',
        RestoreCursor = 'u',
        SaveCursorAndAttrs = 7,
        RestoreCursorAndAttrs = 8
    };

    /*
        Scrolling
        Scroll Screen	<ESC>[r                 Enable scrolling for entire display.
        Scroll Screen	<ESC>[{start};{end}r    Enable scrolling from row {start} to row {end}.
        Scroll Down		<ESC>D                  Scroll display down one line.
        Scroll Up		<ESC>M                  Scroll display up one line.
    */

    enum class Scrolling {
        ScrollScreen,
        ScrollFromPosition,
        ScrollDown,
        ScrollUp
    };

    struct Color
    {
        int r;
        int g;
        int b;
        Color() : r(0), g(0), b(0) {}
        Color(int r, int g, int b) : r(r), g(g), b(b) {}
    };

    class Style {
    private:
        static const bool HasTrueTextColor = 1 << 1;
        static const bool HasTrueBgColor = 1 << 2;
        bool m_flags = 0;
        bool m_bold = false;
        int  m_width = -1;
        TextColor m_textColor = TextColor::None;
        BackgroundColor m_bgColor = BackgroundColor::None;
        Color m_trueTextColor;
        Color m_trueBgColor;
        Aligment m_alignment = Aligment::Left;
        std::string_view m_view = "";
    public:
        Style() {}
        Style(const std::string_view& view) : m_view(view) {}
        Style& black() { m_textColor = TextColor::Black; return *this; }
        Style& red() { m_textColor = TextColor::Red; return *this; }
        Style& green() { m_textColor = TextColor::Green; return *this; }
        Style& yellow() { m_textColor = TextColor::Yellow; return *this; }
        Style& blue() { m_textColor = TextColor::Blue; return *this; }
        Style& magenta() { m_textColor = TextColor::Magenta; return *this; }
        Style& cyan() { m_textColor = TextColor::Cyan; return *this; }
        Style& white() { m_textColor = TextColor::White; return *this; }

        Style& bgBlack() { m_bgColor = BackgroundColor::Black; return *this; }
        Style& bgRed() { m_bgColor = BackgroundColor::Red; return *this; }
        Style& bgGreen() { m_bgColor = BackgroundColor::Green; return *this; }
        Style& bgYellow() { m_bgColor = BackgroundColor::Yellow; return *this; }
        Style& bgBlue() { m_bgColor = BackgroundColor::Blue; return *this; }
        Style& bgMagenta() { m_bgColor = BackgroundColor::Magenta; return *this; }
        Style& bgCyan() { m_bgColor = BackgroundColor::Cyan; return *this; }
        Style& bgWhite() { m_bgColor = BackgroundColor::White; return *this; }

        Style& color(const TextColor& color) {
            m_textColor = color;
            return *this;
        }

        Style& color(const BackgroundColor& color) {
            m_bgColor = color;
            return *this;
        }

        Style& color(const Color& color) {
            m_trueTextColor = color;
            m_flags |= HasTrueTextColor;
            return *this;
        }

        Style& background(const Color& color) {
            m_trueBgColor = color;
            m_flags |= HasTrueBgColor;
            return *this;
        }

        Style& bold() { m_bold = true; return *this; }

        Style& width(int value) { m_width = value;  return *this; }
        Style& left() { m_alignment = Aligment::Left;  return *this; }
        Style& right() { m_alignment = Aligment::Right;  return *this; }
        Style& center() { m_alignment = Aligment::Center;  return *this; }

        Style& text(const std::string_view& view) {
            m_view = view;
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& out, const Style& style) {
            if (style.m_textColor != TextColor::None) {
                out << style.m_textColor;
            }
            if (style.m_bgColor != BackgroundColor::None) {
                out << style.m_bgColor;
            }
            if (style.m_flags && Style::HasTrueTextColor) {
                out << "\x1b[38;2;"
                    << style.m_trueTextColor.r << ";"
                    << style.m_trueTextColor.g << ";"
                    << style.m_trueTextColor.b << ";"
                    << "0m";
            }
            if (style.m_flags && Style::HasTrueBgColor) {
                out << "\x1b[48;2;"
                    << style.m_trueBgColor.r << ";"
                    << style.m_trueBgColor.g << ";"
                    << style.m_trueBgColor.b << ";"
                    << "0m";
            }
            if (style.m_bold) {
                out << Attribute::Bold;
            }

            if (style.m_bold) {
                out << Attribute::Bold;
            }

            auto flags = out.flags();

            if (style.m_width > -1) {
                switch (style.m_alignment) {
                case Aligment::Left: {
                    out << std::left;
                    out << std::setw(style.m_width);
                    break;
                }
                case Aligment::Right:
                    out << std::right;
                    out << std::setw(style.m_width);
                    break;
                case Aligment::Center:
                    int padding = (style.m_width - style.m_view.length()) / 2;
                    if (padding > 0) {
                        out << std::string(padding, ' ');
                    }
                    break;
                }
            }

            out << style.m_view;
            out << Attribute::Reset;

            out.flags(flags);
            return out;
        }
    };

    class Cursor {
    private:
        CursorMovement operation = CursorMovement::None;
        int count = 0, column = 0;
    public:
        Cursor(CursorMovement movement, int count = 0, int column = 0) :
            operation(movement), count(count), column(column)
        { }

        CursorMovement getOperation() const { return operation; }
        int getCount() const { return count; }
        int getColumn() const { return column; }

        static Cursor home(int row, int col) { return Cursor(CursorMovement::Home, row, col); }
        static Cursor up(int count) { return Cursor(CursorMovement::Up, count); }
        static Cursor down(int count) { return Cursor(CursorMovement::Down, count); }
        static Cursor forward(int count) { return Cursor(CursorMovement::Forward, count); }
        static Cursor backward(int count) { return Cursor(CursorMovement::Backward, count); }
        static Cursor forcePosition(int row, int col) { return Cursor(CursorMovement::ForcePosition, row, col); }
        static Cursor saveCursor() { return Cursor(CursorMovement::SaveCursor); }
        static Cursor restoreCursor() { return Cursor(CursorMovement::RestoreCursor); }
        static Cursor saveCursorAndAttrs() { return Cursor(CursorMovement::SaveCursorAndAttrs); }
        static Cursor restoreCursorAndAttrs() { return Cursor(CursorMovement::RestoreCursorAndAttrs); }
    };

    class Scroll {
    private:
        Scrolling scroll = Scrolling::ScrollScreen;
        int start = 0;
        int end = 0;
    public:
        Scroll() {}
        Scroll(Scrolling scroll) : scroll(scroll) {}
        Scroll(Scrolling scroll, int start, int end) : scroll(scroll), start(start), end(end) {}

        Scrolling getScrollType() const { return scroll; }
        int getStart() const { return start; }
        int getEnd() const { return end; }

        static Scroll scrollScreen() {
            return Scroll();
        }
        static Scroll scrollScreen(int start, int end) {
            return Scroll(Scrolling::ScrollFromPosition, start, end);
        }
        static Scroll scrollDown() {
            return Scroll(Scrolling::ScrollDown);
        }
        static Scroll scrollUp() {
            return Scroll(Scrolling::ScrollUp);
        }
    };

    class Console : std::streambuf, public std::ostream
    {
    private:
        int level;
    public:
        Console() : std::ostream(this), level(0) {}

        void enterLevel() { level++; }
        void exitLevel() { if (level > 0) level--; }
        void resetLevel() { level = 0; }

        virtual std::streambuf::int_type overflow(std::streambuf::int_type c) {
            if (c == '\n') {
                if (putchar('\n') != EOF) {
                    for (int i = 0; i < level * 4; i++) {
                        if (putchar(' ') == EOF) {
                            return EOF;
                        }
                    }
                }
                return c;
            }
            if (c != EOF) {
                if (putchar(c) == EOF) {
                    return EOF;
                }
            }
            return c;
        }

        void notice(const std::string_view& view) { block(Style(view).bgBlue().white().width(80)); }
        void error(const std::string_view& view) { block(Style(view).bgRed().white().width(80)); }
        void warning(const std::string_view& view) { block(Style(view).bgYellow().blue().width(80)); }
        void success(const std::string_view& view) { block(Style(view).bgGreen().white().width(80)); }

    private:
        void block(const Style& textStyle) {
            *this << "\n";
            *this << textStyle;
            *this << "\n";
        }
    };

    /* Terminal stream output */

    std::ostream& operator<<(std::ostream& out, Terminal terminal) {
        switch (terminal) {
        case Terminal::ResetDevice: {
            out << "\x1b[c";
        } break;
        case Terminal::EnableLineWrap: {
            out << "\x1b[7h";
        } break;
        case Terminal::DisableLineWrap: {
            out << "\x1b[7l";
        } break;
        }
        return out;
    }

    /* TabControl stream output */

    std::ostream& operator<<(std::ostream& out, TabControl value) {
        switch (value) {
        case TabControl::SetTab: {
            out << "\x1bH";
        } break;
        case TabControl::ClearTab: {
            out << "\x1b[g";
        } break;
        case TabControl::ClearAllTab: {
            out << "\x1b[3g";
        } break;
        }
        return out;
    }

    /* EraseText stream output */

    std::ostream& operator<<(std::ostream& out, EraseText value) {
        switch (value) {
        case EraseText::EraseEndOfLine: {
            out << "\x1b[K";
        } break;
        case EraseText::EraseStartOfLine: {
            out << "\x1b[1K";
        } break;
        case EraseText::EraseLine: {
            out << "\x1b[2K";
        } break;
        case EraseText::EraseDown: {
            out << "\x1b[J";
        } break;
        case EraseText::EraseUp: {
            out << "\x1b[1J";
        } break;
        case EraseText::EraseScreen: {
            out << "\x1b[2J";
        } break;
        }
        return out;
    }

    /* Color stream output */

    std::ostream& operator<<(std::ostream& out, TextColor color) {
        out << "\x1b[" << (int)color << 'm';
        return out;
    }

    std::ostream& operator<<(std::ostream& out, BackgroundColor color) {
        out << "\x1b[" << (int)color << 'm';
        return out;
    }

    std::ostream& operator<<(std::ostream& out, Attribute sequence) {
        out << "\x1b[" << (int)sequence << 'm';
        return out;
    }

    /* Cursor Movement Output */

    std::ostream& operator<<(std::ostream& out, const Cursor& cursor) {
        auto operation = cursor.getOperation();
        auto rowOrCount = cursor.getCount();
        auto col = cursor.getColumn();

        out << "\x1b[";

        switch (operation)
        {
        case CursorMovement::Home: {
            out << "[" << rowOrCount << ";" << col;
        } break;
        case CursorMovement::Up: {
            out << "[" << rowOrCount;
        } break;
        case CursorMovement::Down: {
            out << "[" << rowOrCount;
        } break;
        case CursorMovement::Forward: {
            out << "[" << rowOrCount;
        } break;
        case CursorMovement::Backward: {
            out << "[" << rowOrCount;
        } break;
        case CursorMovement::ForcePosition: {
            out << "[" << rowOrCount << ";" << col;
        } break;
        case CursorMovement::SaveCursor: {
            out << "[";
        } break;
        case CursorMovement::RestoreCursor: {
            out << "[";
        } break;
            //case CursorMovement::SaveCursorAndAttrs: {
            //} break;
            //case CursorMovement::RestoreCursorAndAttrs: {
            //} break;
        }
        out << operation;

        return out;
    }

    /* Scroll Output */

    std::ostream& operator<<(std::ostream& out, const Scroll& scroll) {
        switch (scroll.getScrollType()) {
        case Scrolling::ScrollScreen: {
            out << "\x1b[r";
        } break;
        case Scrolling::ScrollFromPosition: {
            out << "\x1b[" << scroll.getStart() << ";" << scroll.getEnd() << "r";
        } break;
        case Scrolling::ScrollUp: {
            out << "\x1bM";
        } break;
        case Scrolling::ScrollDown: {
            out << "\x1b" << "D";
        } break;
        }
        return out;
    }
}