#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtSpinAmp.hh"
#include <cstdlib>

using std::endl;

std::ostream&
operator<<( std::ostream& os, const EvtSpinAmp& amp )
{
    vector<int> index = amp.iterinit();
    
    os << ":";
    do {
        os<<"<";
        for(int i=0; i<index.size()-1; ++i) {
            os<<index[i];
        }
        os<<index[index.size()-1]<<">"<<amp(index)<<":";
    } while( amp.iterate( index ) );

    return os;
}

EvtSpinAmp operator*( const EvtComplex& real, const EvtSpinAmp& cont )
{
    EvtSpinAmp ret( cont );

    for( int i=0; i<ret._elem.size(); ++i ) {
        ret._elem[i] *= real;
    }

    return ret;
}

EvtSpinAmp operator*( const EvtSpinAmp& cont, const EvtComplex& real )
{
    return real*cont;
}

EvtSpinAmp operator/( const EvtSpinAmp& cont, const EvtComplex& real )
{
    EvtSpinAmp ret( cont );

    for( int i=0; i<ret._elem.size(); ++i ) {
        ret._elem[i] /= real;
    }

    return ret;
}

vector<int> EvtSpinAmp::calctwospin( const vector<EvtSpinType::spintype>& type  ) const
{
    vector<int> twospin;

    for( int i=0; i<type.size(); ++i ) {
        twospin.push_back( EvtSpinType::getSpin2( type[i] ) );
    } 

    return twospin;
}

EvtSpinAmp::EvtSpinAmp( const vector<EvtSpinType::spintype>& type )
{
    int num = 1;
    _type = type;
    _twospin=calctwospin( type );
    
    for( int i=0; i<_twospin.size(); ++i )
        num*=_twospin[i]+1;

    _elem=vector<EvtComplex>( num );
}

EvtSpinAmp::EvtSpinAmp( const vector<EvtSpinType::spintype>& type, const EvtComplex & val )
{
    int num = 1;
    _type = type;
    _twospin=calctwospin( type );

    for( int i=0; i<_twospin.size(); ++i )
        num*=_twospin[i]+1;

    _elem=vector<EvtComplex>( num, val );
}

EvtSpinAmp::EvtSpinAmp( const vector<EvtSpinType::spintype>& type, 
        const vector<EvtComplex>& elem )
{  
    int num = 1;

    _type = type;
    _twospin=calctwospin( type );
    _elem=elem;
    
    for( int i=0; i<_twospin.size(); ++i )
        num*=_twospin[i]+1;

    if(_elem.size() != num ) {
        report(ERROR,"EvtGen")<<"Wrong number of elements input:"
            <<_elem.size()<<" vs. "<<num<<endl;
       ::abort(); 
    }

}

EvtSpinAmp::EvtSpinAmp( const EvtSpinAmp& copy )
{
    _twospin = copy._twospin;
    _elem = copy._elem;
    _type = copy._type;
}

void EvtSpinAmp::checktwospin( const vector<int>& twospin ) const
{
    if( _twospin == twospin )
        return;

    report( ERROR, "EvtGen" )
        <<"Dimension or order of tensors being operated on does not match"
        <<endl;
    ::abort();
}

void EvtSpinAmp::checkindexargs( const vector<int>& index ) const
{
    if( index.size()==0 ) {
        report(ERROR,"EvtGen") << "EvtSpinAmp can't handle no indices" << endl;
        ::abort();
    }

    if( index.size() != _twospin.size() ) {
        report( ERROR, "EvtGen" ) << "Rank of EvtSpinAmp index does not match: " 
            <<_twospin.size()<<" expected "<<index.size()<<" input."<<endl;
        ::abort();
    }

    for( int i=0; i<_twospin.size(); ++i ) {
        if( _twospin[i]>=abs(index[i]) && _twospin[i]%2==index[i]%2 )
            continue; 
        report(ERROR,"EvtGen")<<"EvtSpinAmp index out of range" << endl;
        report(ERROR,"EvtGen")<<" Index: ";
        for(int j=0; j<_twospin.size(); ++j )
            report(ERROR," ")<<_twospin[j];

        report(ERROR, " ")<<endl<<" Index: ";
        for(int j=0; j<index.size(); ++j )
            report(ERROR," ")<<index[j];
        report(ERROR, " ")<<endl;
        ::abort();
    }
}

int EvtSpinAmp::findtrueindex( const vector<int>& index ) const
{
    int trueindex = 0;

    for( int i = index.size()-1; i>0; --i ) {
        trueindex += (index[i]+_twospin[i])/2;
        trueindex *= _twospin[i-1]+1;
    }
    
    trueindex += (index[0]+_twospin[0])/2;

    return trueindex;
}

EvtComplex & EvtSpinAmp::operator()( const vector<int>& index )
{
    checkindexargs( index );
    
    int trueindex = findtrueindex(index);
    if(trueindex >= _elem.size()) {
        report(ERROR,"EvtGen")<<"indexing error "<<trueindex<<" "<<_elem.size()<<endl;
        for(int i=0; i<_twospin.size(); ++i) {
            report(ERROR,"")<<_twospin[i]<<" ";
        }
        report(ERROR,"")<<endl;

        for(int i=0; i<index.size(); ++i) {
            report(ERROR,"")<<index[i]<<" ";
        }
        report(ERROR,"")<<endl;

        ::abort();
    }

    return _elem[trueindex];
}

const EvtComplex & EvtSpinAmp::operator()( const vector<int>& index ) const
{
    checkindexargs( index );
    
    int trueindex = findtrueindex(index);
    if(trueindex >= _elem.size()) {
        report(ERROR,"EvtGen")<<"indexing error "<<trueindex<<" "<<_elem.size()<<endl;
        for(int i=0; i<_twospin.size(); ++i) {
            report(ERROR,"")<<_twospin[i]<<" ";
        }
        report(ERROR,"")<<endl;

        for(int i=0; i<index.size(); ++i) {
            report(ERROR,"")<<index[i]<<" ";
        }
        report(ERROR,"")<<endl;

        ::abort();
    }
    
    return _elem[trueindex];
}

EvtComplex & EvtSpinAmp::operator()( int i, ... )
{
    va_list ap;
    vector<int> index( _twospin.size() );
    
    va_start(ap, i);
    
    index[0]=i;
    for(int n=1; n<_twospin.size(); ++n )
        index[n]=va_arg( ap, int );

    va_end(ap);

    return (*this)( index );
}

const EvtComplex & EvtSpinAmp::operator()( int i, ... ) const
{
    vector<int> index( _twospin.size() );
    va_list ap;

    va_start(ap, i);

    index[0]=i;
    for(int n=1; n<_twospin.size(); ++n )
        index[n]=va_arg( ap, int );

    va_end(ap);

    return (*this)( index );
}

EvtSpinAmp& EvtSpinAmp::operator=( const EvtSpinAmp& cont ) 
{
    _twospin=cont._twospin;
    _elem=cont._elem;
    _type=cont._type;

    return *this;
}

EvtSpinAmp EvtSpinAmp::operator+( const EvtSpinAmp & cont ) const
{
    checktwospin( cont._twospin );

    EvtSpinAmp ret( cont );
    for( int i=0; i<ret._elem.size(); ++i ) {
        ret._elem[i]+=_elem[i];
    }

    return ret;
}

EvtSpinAmp& EvtSpinAmp::operator+=( const EvtSpinAmp&
        cont ) 
{
    checktwospin( cont._twospin );

    for( int i=0; i<_elem.size(); ++i )
        _elem[i]+=cont._elem[i];

    return *this;
}

EvtSpinAmp EvtSpinAmp::operator-( const EvtSpinAmp & cont ) const 
{
    checktwospin( cont._twospin );

    EvtSpinAmp ret( *this );
    for( int i=0; i<ret._elem.size(); ++i )
        ret._elem[i]-=cont._elem[i];

    return ret;
}

EvtSpinAmp& EvtSpinAmp::operator-=( const EvtSpinAmp& cont )
{
    checktwospin( cont._twospin );

    for( int i=0; i<_elem.size(); ++i )
        _elem[i]-=cont._elem[i];

    return *this;
}

// amp = amp1 * amp2
EvtSpinAmp EvtSpinAmp::operator*( const EvtSpinAmp & amp2 ) const
{
    vector<int> index(rank()+amp2.rank());
    vector<int> index1(rank()), index2(amp2.rank()); 
    EvtSpinAmp amp;
    
    amp._twospin=_twospin;
    amp._type=_type;

    for( int i=0; i<amp2._twospin.size(); ++i ) {
        amp._twospin.push_back( amp2._twospin[i] );
        amp._type.push_back( amp2._type[i] );
    }
    
    amp._elem = vector<EvtComplex>( _elem.size() * amp2._elem.size() );

    for( int i=0; i<index1.size(); ++i )
        index[i]=index1[i]=-_twospin[i];
    
    for( int i=0; i<index2.size(); ++i )
        index[i+rank()]=index2[i]=-amp2._twospin[i];

    while(true) {
        amp( index ) = (*this)( index1 )*amp2( index2 );
        if(!amp.iterate( index )) break;
        
        for( int i=0; i<index1.size(); ++i )
            index1[i]=index[i];

        for( int i=0; i<index2.size(); ++i )
            index2[i]=index[i+rank()];
    }
    
    return amp;
}

EvtSpinAmp& EvtSpinAmp::operator*=( const EvtSpinAmp& cont ) 
{
    EvtSpinAmp ret = (*this)*cont;
    *this=ret;
    return *this;
}

EvtSpinAmp& EvtSpinAmp::operator*=( const EvtComplex& real )
{
    for( int i=0; i<_elem.size(); ++i )
        _elem[i] *= real;

    return *this;
}

EvtSpinAmp& EvtSpinAmp::operator/=( const EvtComplex& real )
{
    for( int i=0; i<_elem.size(); ++i )
        _elem[i] /= real;
    
    return *this;
}

vector<int> EvtSpinAmp::iterinit() const
{
    vector<int> init( _twospin.size() );

    for( int i=0; i<_twospin.size(); ++i )
        init[i]=-_twospin[i];

    return init;
}

bool EvtSpinAmp::iterate( vector<int>& index ) const
{
    int last = _twospin.size() - 1;

    index[0]+=2;
    for( int j=0; j<last; ++j ) {
        if( index[j] > _twospin[j] ) {
            index[j] = -_twospin[j];
            index[j+1]+=2;
        }
    }

    return abs(index[last])<=_twospin[last];
}

// Test whether a particular index is an allowed one (specifically to deal with
// photons and possibly neutrinos)
bool EvtSpinAmp::allowed( const vector<int>& index ) const
{
    if( index.size() != _type.size() ) {
        report(ERROR,"EvtGen")
            <<"Wrong dimensino index input to allowed."<<endl;
        ::abort();
    }
    
    for(int i=0; i<index.size(); ++i) {
        switch(_type[i]) {
            case EvtSpinType::PHOTON: 
                if(abs(index[i])!=2) return false;
                break;
            case EvtSpinType::NEUTRINO:
                report(ERROR,"EvtGen")
                    <<"EvMultibody currently cannot handle neutrinos."<<endl;
                ::abort();
	    default:
	      break;
        }
    }
    
    return true;
}

bool EvtSpinAmp::iterateallowed( vector<int>& index ) const
{
    while(true) {
        if(!iterate( index ))
            return false;
        if(allowed( index )) 
            return true;
    }
}

vector<int> EvtSpinAmp::iterallowedinit() const
{
    vector<int> init = iterinit();
    while(!allowed(init)) {
        iterate(init);
    }

    return init;
}

void EvtSpinAmp::intcont( int a, int b )
{
    int newrank=rank()-2;
    if(newrank<=0) {
        report(ERROR,"EvtGen")<<"EvtSpinAmp can't handle no indices" << endl;
        ::abort();
    }

    if(_twospin[a]!=_twospin[b]) {
        report(ERROR,"EvtGen")
            <<"Contaction called on indices of different dimension" 
            <<endl;
        report(ERROR,"EvtGen")
            <<"Called on "<<_twospin[a]<<" and "<<_twospin[b]
            <<endl;
        ::abort();
    }

    vector<int> newtwospin( newrank );
    vector<EvtSpinType::spintype> newtype( newrank );

    for( int i=0, j=0; i<_twospin.size(); ++i ){
        if(i==a || i==b) continue;

        newtwospin[j] = _twospin[i];
        newtype[j] = _type[i];
        ++j;
    }

    EvtSpinAmp newamp( newtype );
    vector<int> index( rank() ), newindex = newamp.iterinit();

    for( int i=0; i<newrank; ++i )
        newindex[i] = -newtwospin[i];

    while(true) {
        
        for( int i=0, j=0; i<rank(); ++i ) {
            if(i==a || i==b) continue;
            index[i]=newindex[j];
            ++j;
        }
        
        index[b]=index[a]=-_twospin[a];
        newamp(newindex) = (*this)(index);
        for( int i=-_twospin[a]+2; i<=_twospin[a]; i+=2 ) {
            index[b]=index[a]=i;
            newamp(newindex) += (*this)(index);
        }
       
        if(!newamp.iterate(newindex)) break;
    }
    
    *this=newamp;
}

// In A.extcont(B), a is the index in A and b is the index in B - note that this
// routine can be extremely improved!
void EvtSpinAmp::extcont( const EvtSpinAmp& cont, int a, int b )
{
    EvtSpinAmp ret = (*this)*cont;
    ret.intcont( a, rank()+b );

    *this=ret;
}
