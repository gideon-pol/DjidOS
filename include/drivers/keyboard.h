#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <drivers/driver.h>
#include <common.h>

class KeyboardDriver// : public Driver
{
public:
    KeyboardDriver(){};
    void Initialize();
    void HandleInterrupt();
};

extern KeyboardDriver keyboardDriver;

class KeyCode{
public:
    enum Key : uint16_t {
        Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, Excl, At, Hashtag, Dollar, Percent, Ampesand, Asterix, LParenthesis, RParenthesis, Minus, Equals, LBracket, RBracket, LBraces, RBraces, Backslash, SemiColon, Colon, Apostrophe, Quote, Comma, LAngBracket, Dot, RAngBracket, Slash, Question, Backspace, Tab, CapsLock, Enter, LShift, RShift, CTRL, Alt, Space, AltGr, Esc, Error 
    };

    KeyCode() = default;
    constexpr KeyCode(Key code) : value(code), capitalized(false){}
    constexpr KeyCode(Key code, bool cap) : value(code), capitalized(cap){}

    constexpr char ToChar() const {
        return defaultTranslation[(uint16_t)value];
    }

    constexpr operator int() const { return (int)value; }

    static KeyCode FromScanCode(uint8_t scanCode){
        switch(scanCode){
            case 0x1: return KeyCode::Esc;
            case 0x2: return KeyCode::One;
            case 0x3: return KeyCode::Two;
            case 0x4: return KeyCode::Three;
            case 0x5: return KeyCode::Four;
            case 0x6: return KeyCode::Five;
            case 0x7: return KeyCode::Six;
            case 0x8: return KeyCode::Seven;
            case 0x9: return KeyCode::Eight;
            case 0xA: return KeyCode::Nine;
            case 0xB: return KeyCode::Zero;
            case 0xC: return KeyCode::Minus;
            case 0xD: return KeyCode::Equals;
            case 0xE: return KeyCode::Backspace;
            case 0xF: return KeyCode::Tab;
            case 0x10: return KeyCode(KeyCode::Q, false);
            case 0x11: return KeyCode(KeyCode::W, false);
            case 0x12: return KeyCode(KeyCode::E, false);
            case 0x13: return KeyCode(KeyCode::R, false);
            case 0x14: return KeyCode(KeyCode::T, false);
            case 0x15: return KeyCode(KeyCode::Y, false);
            case 0x16: return KeyCode(KeyCode::U, false);
            case 0x17: return KeyCode(KeyCode::I, false);
            case 0x18: return KeyCode(KeyCode::O, false);
            case 0x19: return KeyCode(KeyCode::P, false);
            case 0x1A: return KeyCode::LBracket;
            case 0x1B: return KeyCode::RBracket;
            case 0x1C: return KeyCode::Enter;
            case 0x1D: return KeyCode::CTRL;
            case 0x1E: return KeyCode(KeyCode::A, false);
            case 0x1F: return KeyCode(KeyCode::S, false);
            case 0x20: return KeyCode(KeyCode::D, false);
            case 0x21: return KeyCode(KeyCode::F, false);
            case 0x22: return KeyCode(KeyCode::G, false);
            case 0x23: return KeyCode(KeyCode::H, false);
            case 0x24: return KeyCode(KeyCode::J, false);
            case 0x25: return KeyCode(KeyCode::K, false);
            case 0x26: return KeyCode(KeyCode::L, false);
            case 0x27: return KeyCode::SemiColon;
            case 0x28: return KeyCode::Apostrophe;
            case 0x2A: return KeyCode::LShift;
            case 0x2B: return KeyCode::Backslash;
            case 0x2C: return KeyCode(KeyCode::Z, false);
            case 0x2D: return KeyCode(KeyCode::X, false);
            case 0x2E: return KeyCode(KeyCode::C, false);
            case 0x2F: return KeyCode(KeyCode::V, false);
            case 0x30: return KeyCode(KeyCode::B, false);
            case 0x31: return KeyCode(KeyCode::N, false);
            case 0x32: return KeyCode(KeyCode::M, false);
            case 0x33: return KeyCode::Comma;
            case 0x34: return KeyCode::Dot;
            case 0x35: return KeyCode::Slash;
            case 0x36: return KeyCode::RShift;
            case 0x38: return KeyCode::Alt;
            case 0x39: return KeyCode::Space;
            case 0x40: return KeyCode::CapsLock;
        }

        return KeyCode::Error;
    }
private:
    Key value;
    bool capitalized;
    const char* defaultTranslation = "0123456789abcdefghijklmnopqrstuvwxyz!@#$%&*()-=[]{}\\;:'\",<.>/?";
};

void KeyboardHandler(uint8_t scanCode);

#endif