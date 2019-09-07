#ifndef __Identifier_h___
#define __Identifier_h___

#include <vector>
#include <string>

/**
 **-----------------------------------------------
 **
 **  Identifier is a simple type-safe 32 bit unsigned integer. An
 **  Identifier relies on other classes to encode and
 **  decode its information.
 **  
 **  The default constructor created an Identifier an invalid state
 **  which can be check with the "is_valid" method to allow some error
 **  checking.
 **  
 **-----------------------------------------------
 */
class Identifier {
public:

  ///----------------------------------------------------------------
  /// Define public typedefs
  ///----------------------------------------------------------------
  typedef Identifier   id_type;
  typedef unsigned int value_type;
  typedef unsigned int size_type;

  ///----------------------------------------------------------------
  /// Constructors
  ///----------------------------------------------------------------

  /// Default constructor
  Identifier ();

  /// Constructor from value_type
  explicit Identifier (value_type value);

  /// Copy constructor
  Identifier (const Identifier& other);

  ///----------------------------------------------------------------
  /// Modifications
  ///----------------------------------------------------------------

  /// Assignment operator
  Identifier& operator = (value_type value);

  /// Bitwise operations 
  Identifier& operator |= (value_type value);
  Identifier& operator &= (value_type value);

  /// build from a string form - hexadecimal
  void set (const std::string& id);

  /// Reset to invalid state
  void clear ();

  ///----------------------------------------------------------------
  /// Accessors
  ///----------------------------------------------------------------
  /// Get the value 
  operator value_type         (void) const;
  value_type  get_value() const;

  ///----------------------------------------------------------------
  /// Comparison operators
  ///----------------------------------------------------------------
  bool operator ==    (const Identifier& other) const;
  bool operator !=    (const Identifier& other) const;
  bool operator <     (const Identifier& other) const;
  bool operator >     (const Identifier& other) const;

  ///----------------------------------------------------------------
  /// Error management
  ///----------------------------------------------------------------

  /// Check if id is in a valid state
  bool is_valid () const;
  
  ///----------------------------------------------------------------
  /// Utilities
  ///----------------------------------------------------------------

  /// Provide a string form of the identifier - hexadecimal
  std::string  getString() const;

  /// Print out in hex form
  void show () const;

private:

  typedef enum {
    max_value = 0xFFFFFFFF
  } max_value_type;

  //----------------------------------------------------------------
  // The compact identifier data.
  //----------------------------------------------------------------
  value_type m_id;
};

//-----------------------------------------------
//<<<<<< INLINE MEMBER FUNCTIONS                   >>>>>>


// Constructors
//-----------------------------------------------
inline Identifier::Identifier ()
    : m_id(max_value)
{}

//-----------------------------------------------
inline Identifier::Identifier (const Identifier& other)
    : m_id(other.get_value())
{
}

//-----------------------------------------------
inline Identifier::Identifier (value_type value)
    : m_id(value)
{
}


// Modifications
//-----------------------------------------------

inline Identifier&
Identifier::operator = (value_type value)
{
    m_id = value;
    return (*this);
}

inline Identifier&                                   
Identifier::operator |= (unsigned int value)
{
    m_id |= value;
    return (*this);
}

inline Identifier& 
Identifier::operator &= (unsigned int value)
{
    m_id &= value;
    return (*this);
}

inline void 
Identifier::clear () 
{
    m_id = max_value;
}

// Accessors
inline Identifier::operator Identifier::value_type (void) const
{
    return (m_id);
}
                                             
inline Identifier::value_type Identifier::get_value() const
{
    return (m_id);
}

// Comparison operators
//----------------------------------------------------------------
inline bool 
Identifier::operator == (const Identifier& other) const
{
    return (m_id == other.get_value());
}

//----------------------------------------------------------------
inline bool 
Identifier::operator != (const Identifier& other) const
{
    return (m_id != other.get_value());
}

//-----------------------------------------------
inline bool 
Identifier::operator < (const Identifier& other) const
{
    return (m_id < other.get_value());
}

//-----------------------------------------------
inline bool 
Identifier::operator > (const Identifier& other) const
{
    return (m_id > other.get_value());
}

inline bool 
Identifier::is_valid () const
{
    return (!(max_value == m_id));
}

//others -----------------
std::ostream& operator<<(std::ostream & os, const Identifier& Id);

#endif

