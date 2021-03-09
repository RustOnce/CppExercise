#include <iostream>
#include "value.h"
#include "writer.h"
#include <utility>
#include <stdexcept>
#include <cstring>
#include <cassert>
#include <cstddef>

# include "json_allocator.h"

#define JSON_ASSERT_UNREACHABLE assert( false )
#define JSON_ASSERT( condition ) assert( condition );  
#define JSON_ASSERT_MESSAGE( condition, message ) if (!( condition )) throw std::runtime_error( message );

namespace Json {

const Value Value::null;
const Value::Int Value::minInt = Value::Int( ~(Value::UInt(-1)/2) );
const Value::Int Value::maxInt = Value::Int( Value::UInt(-1)/2 );
const Value::UInt Value::maxUInt = Value::UInt(-1);

ValueAllocator::~ValueAllocator()
{
}

class DefaultValueAllocator : public ValueAllocator
{
public:
    virtual ~DefaultValueAllocator()
    {
    }

    virtual char *makeMemberName( const char *memberName )
    {
        return duplicateStringValue( memberName );
    }

    virtual void releaseMemberName( char *memberName )
    {
        releaseStringValue( memberName );
    }

    virtual char *duplicateStringValue( const char *value, 
                                                    unsigned int length = unknown )
    {

        if (length == unknown)
        {
            length = (unsigned int)strlen(value);
        }
        char *newString = static_cast<char *>( malloc( length + 1 ) );
        memcpy( newString, value, length );
        newString[length] = 0;
        return newString;
    }

    virtual void releaseStringValue( char *value )
    {
         if (value)
         {
              free(value);
         }
    }
};

static ValueAllocator *&valueAllocator()
{
    static DefaultValueAllocator defaultAllocator;
    static ValueAllocator *valueAllocator = &defaultAllocator;
    return valueAllocator;
}

static struct DummyValueAllocatorInitializer {
    DummyValueAllocatorInitializer() 
    {
        valueAllocator(); 
    }
} dummyValueAllocatorInitializer;


Value::CommentInfo::CommentInfo()
    : comment_( 0 )
{
}

Value::CommentInfo::~CommentInfo()
{
    if (comment_)
    {
        valueAllocator()->releaseStringValue(comment_);
    }
        
}


void 
Value::CommentInfo::setComment( const char *text )
{
    if (comment_)
    {
        valueAllocator()->releaseStringValue(comment_);
    }

    JSON_ASSERT( text );
    JSON_ASSERT_MESSAGE( text[0]=='\0' || text[0]=='/', "Comments must start with /");
    
    comment_ = valueAllocator()->duplicateStringValue( text );
}

Value::CZString::CZString( int index )
    : cstr_( 0 )
    , index_( index )
{
}

Value::CZString::CZString( const char *cstr, DuplicationPolicy allocate )
    : cstr_( allocate == duplicate ? valueAllocator()->makeMemberName(cstr) : cstr )
    , index_( allocate )
{
}

Value::CZString::CZString( const CZString &other )
: cstr_( other.index_ != noDuplication &&  other.cstr_ != 0
                     ?  valueAllocator()->makeMemberName( other.cstr_ )
                     : other.cstr_ )
    , index_( other.cstr_ ? (other.index_ == noDuplication ? noDuplication : duplicate) : other.index_ )
{
}

Value::CZString::~CZString()
{
    if (cstr_  &&  index_ == duplicate)
    {
        valueAllocator()->releaseMemberName(const_cast<char *>(cstr_));
    }
}

void 
Value::CZString::swap( CZString &other )
{
    std::swap( cstr_, other.cstr_ );
    std::swap( index_, other.index_ );
}

Value::CZString &
Value::CZString::operator =( const CZString &other )
{
    CZString temp( other );
    swap( temp );
    return *this;
}

bool 
Value::CZString::operator<( const CZString &other ) const 
{
    if (cstr_)
    {
        return strcmp(cstr_, other.cstr_) < 0;
    }

    return index_ < other.index_;
}

bool 
Value::CZString::operator==( const CZString &other ) const 
{
    if (cstr_)
    {
        return strcmp(cstr_, other.cstr_) == 0;
    }

    return index_ == other.index_;
}


int 
Value::CZString::index() const
{
    return index_;
}


const char *
Value::CZString::c_str() const
{
    return cstr_;
}

bool 
Value::CZString::isStaticString() const
{
    return index_ == noDuplication;
}


Value::Value( ValueType type )
    : type_( type )
    , allocated_( 0 )
    , comments_( 0 )
{
    switch ( type )
    {
    case nullValue:
        break;
    case intValue:
    case uintValue:
        value_.int_ = 0;
        break;
    case realValue:
        value_.real_ = 0.0;
        break;
    case stringValue:
        value_.string_ = 0;
        break;
    case arrayValue:
    case objectValue:
        value_.map_ = new ObjectValues();
        break;
    case booleanValue:
        value_.bool_ = false;
        break;
    default:
        JSON_ASSERT_UNREACHABLE;
    }
}


Value::Value( Int value )
    : type_( intValue )
    , comments_( 0 )
{
    value_.int_ = value;
}


Value::Value( UInt value )
    : type_( uintValue )
    , comments_( 0 )
{
    value_.uint_ = value;
}

Value::Value( double value )
    : type_( realValue )
    , comments_( 0 )
{
    value_.real_ = value;
}

Value::Value( const char *value )
    : type_( stringValue )
    , allocated_( true )
    , comments_( 0 )
{
    value_.string_ = valueAllocator()->duplicateStringValue( value );
}

Value::Value( const std::string &value )
    : type_( stringValue )
    , allocated_( true )
    , comments_( 0 )

{
    value_.string_ = valueAllocator()->duplicateStringValue( value.c_str(),  (unsigned int)value.length() );

}

Value::Value( const StaticString &value )
    : type_( stringValue )
    , allocated_( false )
    , comments_( 0 )
{
    value_.string_ = const_cast<char *>( value.c_str() );
}

Value::Value( bool value )
    : type_( booleanValue )
    , comments_( 0 )
{
    value_.bool_ = value;
}


Value::Value( const Value &other )
    : type_( other.type_ )
    , comments_( 0 )
{
    switch ( type_ )
    {
    case nullValue:
    case intValue:
    case uintValue:
    case realValue:
    case booleanValue:
        value_ = other.value_;
        break;
    case stringValue:
        if ( other.value_.string_ )
        {
            value_.string_ = valueAllocator()->duplicateStringValue( other.value_.string_ );
            allocated_ = true;
        }
        else
        {
             value_.string_ = 0;
        }
        break;
    case arrayValue:
    case objectValue:
        value_.map_ = new ObjectValues( *other.value_.map_ );
        break;
    default:
        JSON_ASSERT_UNREACHABLE;
    }
    if ( other.comments_ )
    {
        comments_ = new CommentInfo[numberOfCommentPlacement];
        for ( int comment =0; comment < numberOfCommentPlacement; ++comment )
        {
            const CommentInfo &otherComment = other.comments_[comment];
            if (otherComment.comment_)
            {
                 comments_[comment].setComment(otherComment.comment_);
            }
        }
    }
}


Value::~Value()
{
    switch ( type_ )
    {
    case nullValue:
    case intValue:
    case uintValue:
    case realValue:
    case booleanValue:
        break;
    case stringValue:
         if (allocated_)
         {
              valueAllocator()->releaseStringValue(value_.string_);
         }
        break;

    case arrayValue:
    case objectValue:
        delete value_.map_;
        break;
    default:
        JSON_ASSERT_UNREACHABLE;
    }

    if (comments_)
    {
         delete[] comments_;
    }
}

Value &
Value::operator=( const Value &other )
{
    Value temp( other );
    swap( temp );
    return *this;
}

void 
Value::swap( Value &other )
{
    ValueType temp = type_;
    type_ = other.type_;
    other.type_ = temp;
    std::swap( value_, other.value_ );
    int temp2 = allocated_;
    allocated_ = other.allocated_;
    other.allocated_ = temp2;
}

ValueType 
Value::type() const
{
    return type_;
}


int 
Value::compare( const Value &other )
{
    return 0;
}

bool 
Value::operator <( const Value &other ) const
{
    int typeDelta = type_ - other.type_;
    if (typeDelta)
    {
         return typeDelta < 0 ? true : false;
    }
    switch ( type_ )
    {
    case nullValue:
        return false;
    case intValue:
        return value_.int_ < other.value_.int_;
    case uintValue:
        return value_.uint_ < other.value_.uint_;
    case realValue:
        return value_.real_ < other.value_.real_;
    case booleanValue:
        return value_.bool_ < other.value_.bool_;
    case stringValue:
        return ( value_.string_ == 0  &&  other.value_.string_ )
                 || ( other.value_.string_  
                        &&  value_.string_  
                        && strcmp( value_.string_, other.value_.string_ ) < 0 );
    case arrayValue:
    case objectValue:
        {
            int delta = int( value_.map_->size() - other.value_.map_->size() );
            if (delta)
            {
                 return delta < 0;
            }
            return (*value_.map_) < (*other.value_.map_);
        }
    default:
        JSON_ASSERT_UNREACHABLE;
    }
    return 0;  // unreachable
}

bool 
Value::operator <=( const Value &other ) const
{
    return !(other > *this);
}

bool 
Value::operator >=( const Value &other ) const
{
    return !(*this < other);
}

bool 
Value::operator >( const Value &other ) const
{
    return other < *this;
}

bool 
Value::operator ==( const Value &other ) const
{

    int temp = other.type_;
    if (type_ != temp)
    {
         return false;
    }
    switch ( type_ )
    {
    case nullValue:
        return true;
    case intValue:
        return value_.int_ == other.value_.int_;
    case uintValue:
        return value_.uint_ == other.value_.uint_;
    case realValue:
        return value_.real_ == other.value_.real_;
    case booleanValue:
        return value_.bool_ == other.value_.bool_;
    case stringValue:
        return ( value_.string_ == other.value_.string_ )
                 || ( other.value_.string_  
                        &&  value_.string_  
                        && strcmp( value_.string_, other.value_.string_ ) == 0 );
    case arrayValue:
    case objectValue:
        return value_.map_->size() == other.value_.map_->size()
                 && (*value_.map_) == (*other.value_.map_);
    default:
        JSON_ASSERT_UNREACHABLE;
    }
    return 0;  // unreachable
}

bool 
Value::operator !=( const Value &other ) const
{
    return !( *this == other );
}

const char *
Value::asCString() const
{
    JSON_ASSERT( type_ == stringValue );
    return value_.string_;
}


std::string 
Value::asString() const
{
    switch ( type_ )
    {
    case nullValue:
        return "";
    case stringValue:
        return value_.string_ ? value_.string_ : "";
    case booleanValue:
        return value_.bool_ ? "true" : "false";
    case intValue:
    case uintValue:
    case realValue:
    case arrayValue:
    case objectValue:
        JSON_ASSERT_MESSAGE( false, "Type is not convertible to string" );
    default:
        JSON_ASSERT_UNREACHABLE;
    }
    return ""; // unreachable
}

Value::Int 
Value::asInt() const
{
    switch ( type_ )
    {
    case nullValue:
        return 0;
    case intValue:
        return value_.int_;
    case uintValue:
        JSON_ASSERT_MESSAGE( value_.uint_ < (unsigned)maxInt, "integer out of signed integer range" );
        return value_.uint_;
    case realValue:
        JSON_ASSERT_MESSAGE( value_.real_ >= minInt  &&  value_.real_ <= maxInt, "Real out of signed integer range" );
        return Int( value_.real_ );
    case booleanValue:
        return value_.bool_ ? 1 : 0;
    case stringValue:
    case arrayValue:
    case objectValue:
        JSON_ASSERT_MESSAGE( false, "Type is not convertible to int" );
    default:
        JSON_ASSERT_UNREACHABLE;
    }
    return 0; // unreachable;
}

Value::UInt 
Value::asUInt() const
{
    switch ( type_ )
    {
    case nullValue:
        return 0;
    case intValue:
        JSON_ASSERT_MESSAGE( value_.int_ >= 0, "Negative integer can not be converted to unsigned integer" );
        return value_.int_;
    case uintValue:
        return value_.uint_;
    case realValue:
        JSON_ASSERT_MESSAGE( value_.real_ >= 0  &&  value_.real_ <= maxUInt,  "Real out of unsigned integer range" );
        return UInt( value_.real_ );
    case booleanValue:
        return value_.bool_ ? 1 : 0;
    case stringValue:
    case arrayValue:
    case objectValue:
        JSON_ASSERT_MESSAGE( false, "Type is not convertible to uint" );
    default:
        JSON_ASSERT_UNREACHABLE;
    }
    return 0; // unreachable;
}

double 
Value::asDouble() const
{
    switch ( type_ )
    {
    case nullValue:
        return 0.0;
    case intValue:
        return value_.int_;
    case uintValue:
        return value_.uint_;
    case realValue:
        return value_.real_;
    case booleanValue:
        return value_.bool_ ? 1.0 : 0.0;
    case stringValue:
    case arrayValue:
    case objectValue:
        JSON_ASSERT_MESSAGE( false, "Type is not convertible to double" );
    default:
        JSON_ASSERT_UNREACHABLE;
    }
    return 0; // unreachable;
}

bool 
Value::asBool() const
{
    switch ( type_ )
    {
    case nullValue:
        return false;
    case intValue:
    case uintValue:
        return value_.int_ != 0;
    case realValue:
        return value_.real_ != 0.0;
    case booleanValue:
        return value_.bool_;
    case stringValue:
        return value_.string_  &&  value_.string_[0] != 0;
    case arrayValue:
    case objectValue:
        return value_.map_->size() != 0;
    default:
        JSON_ASSERT_UNREACHABLE;
    }
    return false; // unreachable;
}


bool 
Value::isConvertibleTo( ValueType other ) const
{
    switch ( type_ )
    {
    case nullValue:
        return true;
    case intValue:
        return ( other == nullValue  &&  value_.int_ == 0 )
                 || other == intValue
                 || ( other == uintValue  && value_.int_ >= 0 )
                 || other == realValue
                 || other == stringValue
                 || other == booleanValue;
    case uintValue:
        return ( other == nullValue  &&  value_.uint_ == 0 )
                 || ( other == intValue  && value_.uint_ <= (unsigned)maxInt )
                 || other == uintValue
                 || other == realValue
                 || other == stringValue
                 || other == booleanValue;
    case realValue:
        return ( other == nullValue  &&  value_.real_ == 0.0 )
                 || ( other == intValue  &&  value_.real_ >= minInt  &&  value_.real_ <= maxInt )
                 || ( other == uintValue  &&  value_.real_ >= 0  &&  value_.real_ <= maxUInt )
                 || other == realValue
                 || other == stringValue
                 || other == booleanValue;
    case booleanValue:
        return ( other == nullValue  &&  value_.bool_ == false )
                 || other == intValue
                 || other == uintValue
                 || other == realValue
                 || other == stringValue
                 || other == booleanValue;
    case stringValue:
        return other == stringValue
                 || ( other == nullValue  &&  (!value_.string_  ||  value_.string_[0] == 0) );
    case arrayValue:
        return other == arrayValue
                 ||  ( other == nullValue  &&  value_.map_->size() == 0 );
    case objectValue:
        return other == objectValue
                 ||  ( other == nullValue  &&  value_.map_->size() == 0 );
    default:
        JSON_ASSERT_UNREACHABLE;
    }
    return false; // unreachable;
}


Value::UInt 
Value::size() const
{
    switch ( type_ )
    {
    case nullValue:
    case intValue:
    case uintValue:
    case realValue:
    case booleanValue:
    case stringValue:
        return 0;
    case arrayValue:
        if ( !value_.map_->empty() )
        {
            ObjectValues::const_iterator itLast = value_.map_->end();
            --itLast;
            return (*itLast).first.index()+1;
        }
        return 0;
    case objectValue:
        return Int( value_.map_->size() );
    default:
        JSON_ASSERT_UNREACHABLE;
    }
    return 0; // unreachable;
}


bool 
Value::empty() const
{
     if (isNull() || isArray() || isObject())
     {
          return size() == 0u;
     }
     else
     {
          return false;
     }
}


bool
Value::operator!() const
{
    return isNull();
}


void 
Value::clear()
{
    JSON_ASSERT( type_ == nullValue  ||  type_ == arrayValue  || type_ == objectValue );

    switch ( type_ )
    {
    case arrayValue:
    case objectValue:
        value_.map_->clear();
        break;
    default:
        break;
    }
}

void 
Value::resize( UInt newSize )
{
    JSON_ASSERT( type_ == nullValue  ||  type_ == arrayValue );
    if (type_ == nullValue)
    {
         *this = Value(arrayValue);
    }

    UInt oldSize = size();
    if (newSize == 0)
    {
         clear();
    }
    else if (newSize > oldSize)
    {
         (*this)[newSize - 1];
    }
    else
    {
         for (UInt index = newSize; index < oldSize; ++index)
         {
              value_.map_->erase(index);
         }
        assert( size() == newSize );
    }
}


Value &
Value::operator[]( UInt index )
{
    JSON_ASSERT( type_ == nullValue  ||  type_ == arrayValue );
    if (type_ == nullValue)
    {
         *this = Value(arrayValue);
    }

    CZString key( index );
    ObjectValues::iterator it = value_.map_->lower_bound( key );
    if (it != value_.map_->end() && (*it).first == key)
    {
         return (*it).second;
    }

    ObjectValues::value_type defaultValue( key, null );
    it = value_.map_->insert( it, defaultValue );
    return (*it).second;
}


const Value &
Value::operator[]( UInt index ) const
{
    JSON_ASSERT( type_ == nullValue  ||  type_ == arrayValue );
    if (type_ == nullValue)
    {
         return null;
    }

    CZString key( index );
    ObjectValues::const_iterator it = value_.map_->find( key );
    if (it == value_.map_->end())
    {
         return null;
    }

    return (*it).second;
}


Value &
Value::operator[]( const char *key )
{
    return resolveReference( key, false );
}


Value &
Value::resolveReference( const char *key, 
                                 bool isStatic )
{
    JSON_ASSERT( type_ == nullValue  ||  type_ == objectValue );
    if (type_ == nullValue)
    {
         *this = Value(objectValue);
    }

    CZString actualKey( key, isStatic ? CZString::noDuplication  : CZString::duplicateOnCopy );
    ObjectValues::iterator it = value_.map_->lower_bound( actualKey );
    if (it != value_.map_->end() && (*it).first == actualKey)
    {
         return (*it).second;
    }

    ObjectValues::value_type defaultValue( actualKey, null );
    it = value_.map_->insert( it, defaultValue );
    Value &value = (*it).second;
    return value;
}


Value 
Value::get( UInt index, 
                const Value &defaultValue ) const
{
    const Value *value = &((*this)[index]);
    return value == &null ? defaultValue : *value;
}


bool 
Value::isValidIndex( UInt index ) const
{
    return index < size();
}



const Value &
Value::operator[]( const char *key ) const
{
    JSON_ASSERT( type_ == nullValue  ||  type_ == objectValue );
    if (type_ == nullValue)
    {
         return null;
    }

    CZString actualKey( key, CZString::noDuplication );
    ObjectValues::const_iterator it = value_.map_->find( actualKey );
    if (it == value_.map_->end())
    {
         return null;
    }
    return (*it).second;
}


Value &
Value::operator[]( const std::string &key )
{
    return (*this)[ key.c_str() ];
}


const Value &
Value::operator[]( const std::string &key ) const
{
    return (*this)[ key.c_str() ];
}

Value &
Value::operator[]( const StaticString &key )
{
    return resolveReference( key, true );
}

Value &
Value::append( const Value &value )
{
    return (*this)[size()] = value;
}


Value 
Value::get( const char *key, 
                const Value &defaultValue ) const
{
    const Value *value = &((*this)[key]);
    return value == &null ? defaultValue : *value;
}


Value 
Value::get( const std::string &key,
                const Value &defaultValue ) const
{
    return get( key.c_str(), defaultValue );
}

Value
Value::removeMember( const char* key )
{
    JSON_ASSERT( type_ == nullValue  ||  type_ == objectValue );
    if (type_ == nullValue)
    {
         return null;
    }

    CZString actualKey( key, CZString::noDuplication );
    ObjectValues::iterator it = value_.map_->find( actualKey );
    if (it == value_.map_->end())
    {
         return null;
    }
    Value old(it->second);
    value_.map_->erase(it);
    return old;
}

Value
Value::removeMember( const std::string &key )
{
    return removeMember( key.c_str() );
}

bool 
Value::isMember( const char *key ) const
{
    const Value *value = &((*this)[key]);
    return value != &null;
}


bool 
Value::isMember( const std::string &key ) const
{
    return isMember( key.c_str() );
}

Value::Members 
Value::getMemberNames() const
{
    JSON_ASSERT( type_ == nullValue  ||  type_ == objectValue );
    if (type_ == nullValue)
    {
         return Value::Members();
    }

    Members members;
    members.reserve( value_.map_->size() );

    ObjectValues::const_iterator it = value_.map_->begin();
    ObjectValues::const_iterator itEnd = value_.map_->end();
    for (; it != itEnd; ++it)
    {
        members.push_back(std::string((*it).first.c_str()));
    }

    return members;
}

bool
Value::isNull() const
{
    return type_ == nullValue;
}


bool 
Value::isBool() const
{
    return type_ == booleanValue;
}


bool 
Value::isInt() const
{
    return type_ == intValue;
}


bool 
Value::isUInt() const
{
    return type_ == uintValue;
}


bool 
Value::isIntegral() const
{
    return type_ == intValue  
             ||  type_ == uintValue  
             ||  type_ == booleanValue;
}


bool 
Value::isDouble() const
{
    return type_ == realValue;
}


bool 
Value::isNumeric() const
{
    return isIntegral() || isDouble();
}


bool 
Value::isString() const
{
    return type_ == stringValue;
}


bool 
Value::isArray() const
{
    return type_ == nullValue  ||  type_ == arrayValue;
}


bool 
Value::isObject() const
{
    return type_ == nullValue  ||  type_ == objectValue;
}


void 
Value::setComment( const char *comment,
                         CommentPlacement placement )
{
    if (!comments_)
    {
        comments_ = new CommentInfo[numberOfCommentPlacement];
    }

    comments_[placement].setComment( comment );
}


void 
Value::setComment( const std::string &comment,
                         CommentPlacement placement )
{
    setComment( comment.c_str(), placement );
}


bool 
Value::hasComment( CommentPlacement placement ) const
{
    return comments_ != 0  &&  comments_[placement].comment_ != 0;
}

std::string 
Value::getComment( CommentPlacement placement ) const
{
    if (hasComment(placement))
    {
        return comments_[placement].comment_;
    }

    return "";
}


std::string 
Value::toStyledString() const
{
    StyledWriter writer;
    return writer.write( *this );
}


Value::const_iterator 
Value::begin() const
{
    switch ( type_ )
    {
    case arrayValue:
    case objectValue:
        if (value_.map_)
        {
            return const_iterator(value_.map_->begin());
        }
        break;
    default:
        break;
    }
    return const_iterator();
}

Value::const_iterator 
Value::end() const
{
    switch ( type_ )
    {
    case arrayValue:
    case objectValue:
         if (value_.map_)
         {
              return const_iterator(value_.map_->end());
         }
        break;
    default:
        break;
    }
    return const_iterator();
}


Value::iterator 
Value::begin()
{
    switch ( type_ )
    {
    case arrayValue:
    case objectValue:
         if (value_.map_)
         {
              return iterator(value_.map_->begin());
         }
        break;
    default:
        break;
    }
    return iterator();
}

Value::iterator 
Value::end()
{
    switch ( type_ )
    {
    case arrayValue:
    case objectValue:
         if (value_.map_)
         {
              return iterator(value_.map_->end());
         }
        break;
    default:
        break;
    }
    return iterator();
}


PathArgument::PathArgument()
    : kind_( kindNone )
{
}


PathArgument::PathArgument( Value::UInt index )
    : index_( index )
    , kind_( kindIndex )
{
}


PathArgument::PathArgument( const char *key )
    : key_( key )
    , kind_( kindKey )
{
}


PathArgument::PathArgument( const std::string &key )
    : key_( key.c_str() )
    , kind_( kindKey )
{
}

Path::Path( const std::string &path,
                const PathArgument &a1,
                const PathArgument &a2,
                const PathArgument &a3,
                const PathArgument &a4,
                const PathArgument &a5 )
{
    InArgs in;
    in.push_back( &a1 );
    in.push_back( &a2 );
    in.push_back( &a3 );
    in.push_back( &a4 );
    in.push_back( &a5 );
    makePath( path, in );
}


void 
Path::makePath( const std::string &path,
                     const InArgs &in )
{
    const char *current = path.c_str();
    const char *end = current + path.length();
    InArgs::const_iterator itInArg = in.begin();
    while ( current != end )
    {
        if ( *current == '[' )
        {
            ++current;
            if (*current == '%')
            {
                 addPathInArg(path, in, itInArg, PathArgument::kindIndex);
            }
            else
            {
                Value::UInt index = 0;
                for (; current != end && *current >= '0' && *current <= '9'; ++current)
                {
                     index = index * 10 + Value::UInt(*current - '0');
                }
                args_.push_back( index );
            }
            if (current == end || *current++ != ']')
            {
                 invalidPath(path, int(current - path.c_str()));
            }
        }
        else if ( *current == '%' )
        {
            addPathInArg( path, in, itInArg, PathArgument::kindKey );
            ++current;
        }
        else if ( *current == '.' )
        {
            ++current;
        }
        else
        {
            const char *beginName = current;
            while (current != end  &&  !strchr("[.", *current))
            {
                 ++current;
            }
            args_.push_back( std::string( beginName, current ) );
        }
    }
}


void 
Path::addPathInArg( const std::string &path, 
                          const InArgs &in, 
                          InArgs::const_iterator &itInArg, 
                          PathArgument::Kind kind )
{
    if ( itInArg == in.end() )
    {

    }
    else if ( (*itInArg)->kind_ != kind )
    {

    }
    else
    {
        args_.push_back( **itInArg );
    }
}


void 
Path::invalidPath( const std::string &path, 
                         int location )
{

}


const Value &
Path::resolve( const Value &root ) const
{
    const Value *node = &root;
    for ( Args::const_iterator it = args_.begin(); it != args_.end(); ++it )
    {
        const PathArgument &arg = *it;
        if ( arg.kind_ == PathArgument::kindIndex )
        {
            if ( !node->isArray()  ||  node->isValidIndex( arg.index_ ) )
            {
                
            }
            node = &((*node)[arg.index_]);
        }
        else if ( arg.kind_ == PathArgument::kindKey )
        {
            if ( !node->isObject() )
            {
               
            }
            node = &((*node)[arg.key_]);
            if ( node == &Value::null )
            {
              
            }
        }
    }
    return *node;
}


Value 
Path::resolve( const Value &root, 
                    const Value &defaultValue ) const
{
    const Value *node = &root;
    for ( Args::const_iterator it = args_.begin(); it != args_.end(); ++it )
    {
        const PathArgument &arg = *it;
        if ( arg.kind_ == PathArgument::kindIndex )
        {
             if (!node->isArray() || node->isValidIndex(arg.index_))
             {
                  return defaultValue;
             }
            node = &((*node)[arg.index_]);
        }
        else if ( arg.kind_ == PathArgument::kindKey )
        {
            if (!node->isObject())
            {
                return defaultValue;
            }
            node = &((*node)[arg.key_]);
            if (node == &Value::null)
            {
                 return defaultValue;
            }
        }
    }
    return *node;
}


Value &
Path::make( Value &root ) const
{
    Value *node = &root;
    for ( Args::const_iterator it = args_.begin(); it != args_.end(); ++it )
    {
        const PathArgument &arg = *it;

        if ( arg.kind_ == PathArgument::kindIndex )
        {
            if ( !node->isArray() )
            {

            }
            node = &((*node)[arg.index_]);
        }
        else if ( arg.kind_ == PathArgument::kindKey )
        {
            if ( !node->isObject() )
            {

            }

            node = &((*node)[arg.key_]);
        }
    }
    return *node;
}

ValueIteratorBase::ValueIteratorBase()
{
}


ValueIteratorBase::ValueIteratorBase(const Value::ObjectValues::iterator &current)
    : current_(current)
{
}

Value &
ValueIteratorBase::deref() const
{
    return current_->second;
}


void
ValueIteratorBase::increment()
{
    ++current_;
}


void
ValueIteratorBase::decrement()
{
    --current_;
}


ValueIteratorBase::difference_type
ValueIteratorBase::computeDistance(const SelfType &other) const
{
    return difference_type(std::distance(current_, other.current_));
}


bool
ValueIteratorBase::isEqual(const SelfType &other) const
{
    return current_ == other.current_;
}


void
ValueIteratorBase::copy(const SelfType &other)
{
    current_ = other.current_;
}


Value
ValueIteratorBase::key() const
{

    const Value::CZString czstring = (*current_).first;
    if (czstring.c_str())
    {
        if (czstring.isStaticString())
        {
            return Value(StaticString(czstring.c_str()));
        }
        return Value(czstring.c_str());
    }
    return Value(czstring.index());

}


Value::UInt
ValueIteratorBase::index() const
{

    const Value::CZString czstring = (*current_).first;
    if (!czstring.c_str())
    {
        return czstring.index();
    }
    return Value::UInt(-1);
}


const char *
ValueIteratorBase::memberName() const
{
    const char *name = (*current_).first.c_str();
    return name ? name : "";
}

ValueConstIterator::ValueConstIterator()
{
}



ValueConstIterator::ValueConstIterator(const Value::ObjectValues::iterator &current)
    : ValueIteratorBase(current)
{
}

ValueConstIterator &
ValueConstIterator::operator =(const ValueIteratorBase &other)
{
    copy(other);
    return *this;
}

ValueIterator::ValueIterator()
{
}


ValueIterator::ValueIterator(const Value::ObjectValues::iterator &current)
    : ValueIteratorBase(current)
{
}

ValueIterator::ValueIterator(const ValueConstIterator &other)
    : ValueIteratorBase(other)
{
}

ValueIterator::ValueIterator(const ValueIterator &other)
    : ValueIteratorBase(other)
{
}

ValueIterator &
ValueIterator::operator =(const SelfType &other)
{
    copy(other);
    return *this;
}

} // namespace Json