/* this is -*- C++ -*- */
#ifndef SPEC_DMPLIB_H_
#define SPEC_DMPLIB_H_

#include <string>
#include <iostream>

//
// exceptions raised
//
class AsciiDumpException {};

class AsciiNoStartChar : public AsciiDumpException {};
class AsciiNoEndChar : public AsciiDumpException {};

class AsciiWrongTag : public AsciiDumpException {
public:
    AsciiWrongTag(const std::string& expected, const std::string& got)
        : m_expected(expected), m_got(got) {};
public:
    std::string expected() const { return m_expected; }
    std::string got() const { return m_got; };
private:
    std::string m_expected;
    std::string m_got;
};

class AsciiWrongStartTag : public AsciiWrongTag {
public:
    AsciiWrongStartTag(const std::string& expected, const std::string& got) 
        : AsciiWrongTag(expected, got)
        {}
};

class AsciiWrongEndTag : public AsciiWrongTag {
public:
    AsciiWrongEndTag(const std::string& expected, const std::string& got)
        : AsciiWrongTag(expected, got)
        {}
};

//
// the base class of all tagged classes
//
class Tagged {
public:
    Tagged() 
        : m_initialized(false)
        {};
public:
    bool initialized() const;
    void set_initialized();
    void unset_initalized();
protected:
    void check_start_tag(std::istream& is, const char *tag);
    void check_end_tag(std::istream& is, const char *tag);
private:
    bool               m_initialized;
    static std::string s_saved_tag;
};

inline
bool Tagged::initialized() const
{
    return m_initialized;
}

inline
void Tagged::set_initialized() 
{ 
    m_initialized = true; 
}

inline
void Tagged::unset_initalized()
{ 
    m_initialized = false; 
}

inline
void Tagged::check_start_tag(std::istream& is, const char *tag)
{ 
    // read input, check for '{' character
    char c; 
    if(!(is >> c) || (c != '{')) {
        throw AsciiNoStartChar();
    }

    // compare tags
    std::string in_tag;
    is >> in_tag;
    if(in_tag != tag) 
      throw AsciiWrongStartTag(tag, in_tag);

    // check for empty block
    is >> c;
    if(c == '}') {
        is >> in_tag;
        if(in_tag != tag) 
            throw AsciiWrongEndTag(tag, in_tag);
    } else {
        is.putback(c); 
        set_initialized();
    }
}

inline
void Tagged::check_end_tag(std::istream& is, const char *tag)
{
    char c; 
    is >> c;
    if(c != '}') 
        throw AsciiNoEndChar();

    std::string in_tag;
    is >> in_tag;
    if(in_tag != tag) 
        throw AsciiWrongEndTag(tag, in_tag);
}


#endif /* SPEC_DMPLIB_H_ */
