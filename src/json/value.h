#ifndef CPPTL_JSON_H_INCLUDED
# define CPPTL_JSON_H_INCLUDED

# include "forwards.h"
# include <string>
# include <vector>
# include <map>

namespace Json {

    enum ValueType
    {
        nullValue = 0,  ///< 'null' value
        intValue,       ///< signed integer value
        uintValue,      ///< unsigned integer value
        realValue,      ///< double value
        stringValue,    ///< UTF-8 string value
        booleanValue,   ///< bool value
        arrayValue,     ///< array value (ordered list)
        objectValue     ///< object value (collection of name/value pairs).
    };

    enum CommentPlacement
    {
        commentBefore = 0,          ///< a comment placed on the line before a value
        commentAfterOnSameLine,     ///< a comment just after a value on the same line
        commentAfter,               ///< a comment on the line after a value (only make sense for root value)
        numberOfCommentPlacement
    };

    class JSON_API StaticString
    {
    public:
        explicit StaticString( const char *czstring )
            : str_( czstring )
        {
        }

        operator const char *() const
        {
            return str_;
        }

        const char *c_str() const
        {
            return str_;
        }

    private:
        const char *str_;
    };

    class JSON_API Value 
    {
        friend class ValueIteratorBase;

    public:

        typedef std::vector<std::string> Members;

        typedef int Int;
        typedef unsigned int UInt;
        typedef ValueIterator iterator;
        typedef ValueConstIterator const_iterator;
        typedef UInt ArrayIndex;

        static const Value null;
        static const Int minInt;
        static const Int maxInt;
        static const UInt maxUInt;

    private:

        class CZString 
        {
        public:
            enum DuplicationPolicy 
            {
                noDuplication = 0,
                duplicate,
                duplicateOnCopy
            };

            CZString( int index );
            CZString( const char *cstr, DuplicationPolicy allocate );
            CZString( const CZString &other );
            ~CZString();

            CZString &operator =( const CZString &other );
            bool operator<( const CZString &other ) const;
            bool operator==( const CZString &other ) const;
            int index() const;

            const char *c_str() const;
            bool isStaticString() const;
        private:
            void swap( CZString &other );
            const char *cstr_;
            int index_;
        };

    public:

        typedef std::map<CZString, Value> ObjectValues;

    public:
      
        Value( ValueType type = nullValue );

        Value( Int value );
        Value( UInt value );
        Value( double value );
        Value( const char *value );
      
        Value( const StaticString &value );
        Value( const std::string &value );
        Value( bool value );
        Value( const Value &other );
        ~Value();

        Value &operator=( const Value &other );

        void swap( Value &other );

        ValueType type() const;

        bool operator <( const Value &other ) const;
        bool operator <=( const Value &other ) const;
        bool operator >=( const Value &other ) const;
        bool operator >( const Value &other ) const;

        bool operator ==( const Value &other ) const;
        bool operator !=( const Value &other ) const;

        int compare( const Value &other );

        const char *asCString() const;
        std::string asString() const;

        Int asInt() const;
        UInt asUInt() const;
        double asDouble() const;
        bool asBool() const;

        bool isNull() const;
        bool isBool() const;
        bool isInt() const;
        bool isUInt() const;
        bool isIntegral() const;
        bool isDouble() const;
        bool isNumeric() const;
        bool isString() const;
        bool isArray() const;
        bool isObject() const;

        bool isConvertibleTo( ValueType other ) const;

        UInt size() const;

        bool empty() const;

        bool operator!() const;

        void clear();

        void resize( UInt size );

        Value &operator[]( UInt index );

        const Value &operator[]( UInt index ) const;

        Value get( UInt index, const Value &defaultValue ) const;

        bool isValidIndex( UInt index ) const;

        Value &append( const Value &value );

        Value &operator[]( const char *key );
        
        const Value &operator[]( const char *key ) const;
        
        Value &operator[]( const std::string &key );
        
        const Value &operator[]( const std::string &key ) const;
        
        Value &operator[]( const StaticString &key );

        Value get( const char *key, const Value &defaultValue ) const;
        Value get( const std::string &key, const Value &defaultValue ) const;

        Value removeMember( const char* key );
        Value removeMember( const std::string &key );

        bool isMember( const char *key ) const;
        bool isMember( const std::string &key ) const;

        Members getMemberNames() const;

        void setComment( const char *comment, CommentPlacement placement );

        void setComment( const std::string &comment, CommentPlacement placement );

        bool hasComment( CommentPlacement placement ) const;

        std::string getComment( CommentPlacement placement ) const;

        std::string toStyledString() const;

        const_iterator begin() const;
        const_iterator end() const;

        iterator begin();
        iterator end();

    private:
        Value &resolveReference( const char *key, bool isStatic );

    private:
        struct CommentInfo
        {
            CommentInfo();
            ~CommentInfo();

            void setComment( const char *text );
            char *comment_;
        };

        union ValueHolder
        {
            Int int_;
            UInt uint_;
            double real_;
            bool bool_;
            char *string_;
            ObjectValues *map_;

        } value_;

        ValueType type_ : 8;
        int allocated_ : 1;  

        CommentInfo *comments_;
    };

    class PathArgument
    {
    public:
        friend class Path;

        PathArgument();
        PathArgument( Value::UInt index );
        PathArgument( const char *key );
        PathArgument( const std::string &key );

    private:

        enum Kind
        {
            kindNone = 0,
            kindIndex,
            kindKey
        };

        std::string key_;
        Value::UInt index_;
        Kind kind_;
    };

    class Path
    {
    public:

        Path( const std::string &path,
                const PathArgument &a1 = PathArgument(),
                const PathArgument &a2 = PathArgument(),
                const PathArgument &a3 = PathArgument(),
                const PathArgument &a4 = PathArgument(),
                const PathArgument &a5 = PathArgument() );

        const Value &resolve( const Value &root ) const;
        Value resolve( const Value &root, const Value &defaultValue ) const;
        Value &make( Value &root ) const;

    private:
        typedef std::vector<const PathArgument *> InArgs;
        typedef std::vector<PathArgument> Args;

        void makePath( const std::string &path, const InArgs &in );

        void addPathInArg( const std::string &path, 
                                 const InArgs &in, 
                                 InArgs::const_iterator &itInArg, 
                                 PathArgument::Kind kind );

        void invalidPath( const std::string &path, int location );

        Args args_;
    };

    class ValueAllocator
    {
    public:
        enum { unknown = (unsigned)-1 };

        virtual ~ValueAllocator();

        virtual char *makeMemberName( const char *memberName ) = 0;
        virtual void releaseMemberName( char *memberName ) = 0;
        virtual char *duplicateStringValue( const char *value, unsigned int length = unknown ) = 0;
        virtual void releaseStringValue( char *value ) = 0;
    };

    class ValueIteratorBase
    {
    public:
        typedef unsigned int size_t;
        typedef int difference_type;
        typedef ValueIteratorBase SelfType;

        ValueIteratorBase();

        explicit ValueIteratorBase( const Value::ObjectValues::iterator &current );

        bool operator ==( const SelfType &other ) const
        {
            return isEqual( other );
        }

        bool operator !=( const SelfType &other ) const
        {
            return !isEqual( other );
        }

        difference_type operator -( const SelfType &other ) const
        {
            return computeDistance( other );
        }

        Value key() const;

        Value::UInt index() const;

        const char *memberName() const;

    protected:
        Value &deref() const;

        void increment();

        void decrement();

        difference_type computeDistance( const SelfType &other ) const;

        bool isEqual( const SelfType &other ) const;

        void copy( const SelfType &other );

    private:

        Value::ObjectValues::iterator current_;

    };

    class ValueConstIterator : public ValueIteratorBase
    {
        friend class Value;
    public:
        typedef unsigned int size_t;
        typedef int difference_type;
        typedef const Value &reference;
        typedef const Value *pointer;
        typedef ValueConstIterator SelfType;

        ValueConstIterator();

    private:

        explicit ValueConstIterator( const Value::ObjectValues::iterator &current );

    public:
        SelfType &operator =( const ValueIteratorBase &other );

        SelfType operator++( int )
        {
            SelfType temp( *this );
            ++*this;
            return temp;
        }

        SelfType operator--( int )
        {
            SelfType temp( *this );
            --*this;
            return temp;
        }

        SelfType &operator--()
        {
            decrement();
            return *this;
        }

        SelfType &operator++()
        {
            increment();
            return *this;
        }

        reference operator *() const
        {
            return deref();
        }
    };

    class ValueIterator : public ValueIteratorBase
    {
        friend class Value;
    public:
        typedef unsigned int size_t;
        typedef int difference_type;
        typedef Value &reference;
        typedef Value *pointer;
        typedef ValueIterator SelfType;

        ValueIterator();
        ValueIterator( const ValueConstIterator &other );
        ValueIterator( const ValueIterator &other );

    private:

        explicit ValueIterator( const Value::ObjectValues::iterator &current );

    public:

        SelfType &operator =( const SelfType &other );

        SelfType operator++( int )
        {
            SelfType temp( *this );
            ++*this;
            return temp;
        }

        SelfType operator--( int )
        {
            SelfType temp( *this );
            --*this;
            return temp;
        }

        SelfType &operator--()
        {
            decrement();
            return *this;
        }

        SelfType &operator++()
        {
            increment();
            return *this;
        }

        reference operator *() const
        {
            return deref();
        }
    };
} // namespace Json


#endif // CPPTL_JSON_H_INCLUDED
