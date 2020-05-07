#pragma once

#include <iostream>

namespace Amber
{

typedef enum class KeyCode : uint16_t
{
    // From glfw3.h
    Space = 32,
    Apostrophe = 39, /* ' */
    Comma = 44, /* , */
    Minus = 45, /* - */
    Period = 46, /* . */
    Slash = 47, /* / */

    D0 = 48, /* 0 */
    D1 = 49, /* 1 */
    D2 = 50, /* 2 */
    D3 = 51, /* 3 */
    D4 = 52, /* 4 */
    D5 = 53, /* 5 */
    D6 = 54, /* 6 */
    D7 = 55, /* 7 */
    D8 = 56, /* 8 */
    D9 = 57, /* 9 */

    Semicolon = 59, /* ; */
    Equal = 61, /* = */

    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,

    LeftBracket = 91,  /* [ */
    Backslash = 92,  /* \ */
    RightBracket = 93,  /* ] */
    GraveAccent = 96,  /* ` */

    World1 = 161, /* non-US #1 */
    World2 = 162, /* non-US #2 */

    /* Function keys */
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,

    /* Keypad */
    KP0 = 320,
    KP1 = 321,
    KP2 = 322,
    KP3 = 323,
    KP4 = 324,
    KP5 = 325,
    KP6 = 326,
    KP7 = 327,
    KP8 = 328,
    KP9 = 329,
    KPDecimal = 330,
    KPDivide = 331,
    KPMultiply = 332,
    KPSubtract = 333,
    KPAdd = 334,
    KPEnter = 335,
    KPEqual = 336,

    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348
} Key;

inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
{
    os << static_cast<int32_t>(keyCode);
    return os;
}

}

// From glfw3.h
#define AB_KEY_SPACE           ::Amber::Key::Space
#define AB_KEY_APOSTROPHE      ::Amber::Key::Apostrophe    /* ' */
#define AB_KEY_COMMA           ::Amber::Key::Comma         /* , */
#define AB_KEY_MINUS           ::Amber::Key::Minus         /* - */
#define AB_KEY_PERIOD          ::Amber::Key::Period        /* . */
#define AB_KEY_SLASH           ::Amber::Key::Slash         /* / */
#define AB_KEY_0               ::Amber::Key::D0
#define AB_KEY_1               ::Amber::Key::D1
#define AB_KEY_2               ::Amber::Key::D2
#define AB_KEY_3               ::Amber::Key::D3
#define AB_KEY_4               ::Amber::Key::D4
#define AB_KEY_5               ::Amber::Key::D5
#define AB_KEY_6               ::Amber::Key::D6
#define AB_KEY_7               ::Amber::Key::D7
#define AB_KEY_8               ::Amber::Key::D8
#define AB_KEY_9               ::Amber::Key::D9
#define AB_KEY_SEMICOLON       ::Amber::Key::Semicolon     /* ; */
#define AB_KEY_EQUAL           ::Amber::Key::Equal         /* = */
#define AB_KEY_A               ::Amber::Key::A
#define AB_KEY_B               ::Amber::Key::B
#define AB_KEY_C               ::Amber::Key::C
#define AB_KEY_D               ::Amber::Key::D
#define AB_KEY_E               ::Amber::Key::E
#define AB_KEY_F               ::Amber::Key::F
#define AB_KEY_G               ::Amber::Key::G
#define AB_KEY_H               ::Amber::Key::H
#define AB_KEY_I               ::Amber::Key::I
#define AB_KEY_J               ::Amber::Key::J
#define AB_KEY_K               ::Amber::Key::K
#define AB_KEY_L               ::Amber::Key::L
#define AB_KEY_M               ::Amber::Key::M
#define AB_KEY_N               ::Amber::Key::N
#define AB_KEY_O               ::Amber::Key::O
#define AB_KEY_P               ::Amber::Key::P
#define AB_KEY_Q               ::Amber::Key::Q
#define AB_KEY_R               ::Amber::Key::R
#define AB_KEY_S               ::Amber::Key::S
#define AB_KEY_T               ::Amber::Key::T
#define AB_KEY_U               ::Amber::Key::U
#define AB_KEY_V               ::Amber::Key::V
#define AB_KEY_W               ::Amber::Key::W
#define AB_KEY_X               ::Amber::Key::X
#define AB_KEY_Y               ::Amber::Key::Y
#define AB_KEY_Z               ::Amber::Key::Z
#define AB_KEY_LEFT_BRACKET    ::Amber::Key::LeftBracket   /* [ */
#define AB_KEY_BACKSLASH       ::Amber::Key::Backslash     /* \ */
#define AB_KEY_RIGHT_BRACKET   ::Amber::Key::RightBracket  /* ] */
#define AB_KEY_GRAVE_ACCENT    ::Amber::Key::GraveAccent   /* ` */
#define AB_KEY_WORLD_1         ::Amber::Key::World1        /* non-US #1 */
#define AB_KEY_WORLD_2         ::Amber::Key::World2        /* non-US #2 */

/* Function keys */
#define AB_KEY_ESCAPE          ::Amber::Key::Escape
#define AB_KEY_ENTER           ::Amber::Key::Enter
#define AB_KEY_TAB             ::Amber::Key::Tab
#define AB_KEY_BACKSPACE       ::Amber::Key::Backspace
#define AB_KEY_INSERT          ::Amber::Key::Insert
#define AB_KEY_DELETE          ::Amber::Key::Delete
#define AB_KEY_RIGHT           ::Amber::Key::Right
#define AB_KEY_LEFT            ::Amber::Key::Left
#define AB_KEY_DOWN            ::Amber::Key::Down
#define AB_KEY_UP              ::Amber::Key::Up
#define AB_KEY_PAGE_UP         ::Amber::Key::PageUp
#define AB_KEY_PAGE_DOWN       ::Amber::Key::PageDown
#define AB_KEY_HOME            ::Amber::Key::Home
#define AB_KEY_END             ::Amber::Key::End
#define AB_KEY_CAPS_LOCK       ::Amber::Key::CapsLock
#define AB_KEY_SCROLL_LOCK     ::Amber::Key::ScrollLock
#define AB_KEY_NUM_LOCK        ::Amber::Key::NumLock
#define AB_KEY_PRINT_SCREEN    ::Amber::Key::PrintScreen
#define AB_KEY_PAUSE           ::Amber::Key::Pause
#define AB_KEY_F1              ::Amber::Key::F1
#define AB_KEY_F2              ::Amber::Key::F2
#define AB_KEY_F3              ::Amber::Key::F3
#define AB_KEY_F4              ::Amber::Key::F4
#define AB_KEY_F5              ::Amber::Key::F5
#define AB_KEY_F6              ::Amber::Key::F6
#define AB_KEY_F7              ::Amber::Key::F7
#define AB_KEY_F8              ::Amber::Key::F8
#define AB_KEY_F9              ::Amber::Key::F9
#define AB_KEY_F10             ::Amber::Key::F10
#define AB_KEY_F11             ::Amber::Key::F11
#define AB_KEY_F12             ::Amber::Key::F12
#define AB_KEY_F13             ::Amber::Key::F13
#define AB_KEY_F14             ::Amber::Key::F14
#define AB_KEY_F15             ::Amber::Key::F15
#define AB_KEY_F16             ::Amber::Key::F16
#define AB_KEY_F17             ::Amber::Key::F17
#define AB_KEY_F18             ::Amber::Key::F18
#define AB_KEY_F19             ::Amber::Key::F19
#define AB_KEY_F20             ::Amber::Key::F20
#define AB_KEY_F21             ::Amber::Key::F21
#define AB_KEY_F22             ::Amber::Key::F22
#define AB_KEY_F23             ::Amber::Key::F23
#define AB_KEY_F24             ::Amber::Key::F24
#define AB_KEY_F25             ::Amber::Key::F25

/* Keypad */
#define AB_KEY_KP_0            ::Amber::Key::KP0
#define AB_KEY_KP_1            ::Amber::Key::KP1
#define AB_KEY_KP_2            ::Amber::Key::KP2
#define AB_KEY_KP_3            ::Amber::Key::KP3
#define AB_KEY_KP_4            ::Amber::Key::KP4
#define AB_KEY_KP_5            ::Amber::Key::KP5
#define AB_KEY_KP_6            ::Amber::Key::KP6
#define AB_KEY_KP_7            ::Amber::Key::KP7
#define AB_KEY_KP_8            ::Amber::Key::KP8
#define AB_KEY_KP_9            ::Amber::Key::KP9
#define AB_KEY_KP_DECIMAL      ::Amber::Key::KPDecimal
#define AB_KEY_KP_DIVIDE       ::Amber::Key::KPDivide
#define AB_KEY_KP_MULTIPLY     ::Amber::Key::KPMultiply
#define AB_KEY_KP_SUBTRACT     ::Amber::Key::KPSubtract
#define AB_KEY_KP_ADD          ::Amber::Key::KPAdd
#define AB_KEY_KP_ENTER        ::Amber::Key::KPEnter
#define AB_KEY_KP_EQUAL        ::Amber::Key::KPEqual

#define AB_KEY_LEFT_SHIFT      ::Amber::Key::LeftShift
#define AB_KEY_LEFT_CONTROL    ::Amber::Key::LeftControl
#define AB_KEY_LEFT_ALT        ::Amber::Key::LeftAlt
#define AB_KEY_LEFT_SUPER      ::Amber::Key::LeftSuper
#define AB_KEY_RIGHT_SHIFT     ::Amber::Key::RightShift
#define AB_KEY_RIGHT_CONTROL   ::Amber::Key::RightControl
#define AB_KEY_RIGHT_ALT       ::Amber::Key::RightAlt
#define AB_KEY_RIGHT_SUPER     ::Amber::Key::RightSuper
#define AB_KEY_MENU            ::Amber::Key::Menu