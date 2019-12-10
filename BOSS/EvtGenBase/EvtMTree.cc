#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string>

#include "EvtGenBase/EvtMTree.hh"
#include "EvtGenBase/EvtConst.hh"
#include "EvtGenBase/EvtKine.hh"
#include "EvtGenBase/EvtReport.hh"

// Make sure to include Lineshapes here
#include "EvtGenBase/EvtMTrivialLS.hh"
#include "EvtGenBase/EvtMBreitWigner.hh"

// Make sure to include Parametrizations
#include "EvtGenBase/EvtMHelAmp.hh"

using std::endl;

EvtMTree::EvtMTree( const EvtId * idtbl, int ndaug )
{
    for( int i=0; i<ndaug; ++i ) {
        _lbltbl.push_back( EvtPDL::name( idtbl[i] ) );
    }
}

EvtMTree::~EvtMTree()
{
    for(int i=0; i<_root.size(); ++i) delete _root[i];
}

bool EvtMTree::parsecheck( char arg, const string& chars )
{
    bool ret = false;

    for(int i=0; i<chars.size(); ++i) {
        ret = ret || (chars[i]==arg);
    }

    return ret;
}

vector<EvtMNode *> EvtMTree::makeparticles( const string& strid ) 
{
    vector<EvtMNode *> particles;
    vector<int> labels;
   
    for( int i = 0; i<_lbltbl.size(); ++i ) {
        if( _lbltbl[i] == strid ) labels.push_back( i );
    }
    
    if( labels.size() == 0 ) {
        report(ERROR,"EvtGen")<<"Error unknown particle label "<<strid<<endl;
        ::abort();
    }

    for( int i = 0; i<labels.size(); ++i )
        particles.push_back( new EvtMParticle( labels[i], EvtPDL::getId( strid ) ) );

    return particles;
}

EvtMRes * EvtMTree::makeresonance( const EvtId& id, const string& ls,
        const vector<string>& lsarg, const string& type,
        const vector<EvtComplex>& amps, const vector<EvtMNode *>& children )
{
    EvtMRes * resonance = NULL;
    EvtMLineShape * lineshape = NULL;

    if( ls=="BREITWIGNER" ) {
        lineshape = new EvtMBreitWigner( id, lsarg );
    } else if( ls=="TRIVIAL" ) {
        lineshape = new EvtMTrivialLS( id, lsarg );
    } else {
        report(ERROR,"EvtGen")<<"Lineshape "<<lineshape
                              <<" not recognized."<<endl;
        ::abort();
    }

    if( type=="HELAMP" ) {
        resonance = new EvtMHelAmp( id, lineshape, children, amps );
    } else {
        report(ERROR,"EvtGen")<<"Model "<<type<<" not recognized."<<endl;
        ::abort();
    }

    lineshape->setres( resonance );

    return resonance;
}

void EvtMTree::parseerror( bool flag, ptype& c_iter, ptype& c_begin, 
        ptype& c_end )
{ 
    if(!flag) return;

    string error;
    
    while( c_begin != c_end ) {
        if(c_begin == c_iter) {
            error+='_';
            error+=*c_begin;
            error+='_';
        } else 
            error+=*c_begin;

        ++c_begin;
    }

    report(ERROR,"EvtGen")<<"Parse error at: "<<error<<endl;
    ::abort();
}

string EvtMTree::parseId( ptype& c_iter, ptype& c_begin, ptype& c_end ) 
{
    string strid;

    while(c_iter != c_end) {
        parseerror(parsecheck(*c_iter, ")[],"), c_iter, c_begin, c_end);
        if( *c_iter == '(' ) {
            ++c_iter;
            return strid;
        }

        strid += *c_iter;
        ++c_iter;
    }

    return strid;
}

string EvtMTree::parseKey( ptype& c_iter, ptype& c_begin, ptype& c_end ) 
{
    string key;

    while( *c_iter != ',' ) {
        parseerror(c_iter==c_end || parsecheck(*c_iter, "()[]"),
            c_iter, c_begin, c_end);
        key += *c_iter;
        ++c_iter;
    }

    ++c_iter;

    parseerror(c_iter == c_end, c_iter, c_begin, c_end);
    
    return key;
}

vector<string> EvtMTree::parseArg( ptype &c_iter, ptype &c_begin, ptype &c_end )
{
    vector<string> arg;

    if( *c_iter != '[' ) return arg;
    ++c_iter;

    string temp;
    while(true) {
        parseerror( c_iter == c_end || parsecheck(*c_iter, "[()"),
                c_iter, c_begin, c_end );

        if( *c_iter == ']' ) {
            ++c_iter;
            if(temp.size() > 0) arg.push_back( temp );
            break;
        }

        if( *c_iter == ',') {
            arg.push_back( temp );
            temp.erase();
            ++c_iter;
            continue;
        }

        temp += *c_iter;
        ++c_iter;
    }
    parseerror(c_iter == c_end || *c_iter != ',', c_iter, c_begin, c_end);
    ++c_iter;

    return arg;
}

vector<EvtComplex> EvtMTree::parseAmps( ptype &c_iter, 
        ptype &c_begin, ptype &c_end )
{
    vector<string> parg = parseArg( c_iter, c_begin, c_end );
    parseerror( parg.size() == 0, c_iter, c_begin, c_end );

    // Get parametrization amplitudes
    vector<string>::iterator amp_iter = parg.begin();
    vector<string>::iterator amp_end = parg.end();
    vector<EvtComplex> amps;

    while( amp_iter != amp_end ) {
        const char * nptr;
        char * endptr = NULL;
        double amp=0.0, phase=0.0;

        nptr = (*amp_iter).c_str();
        amp = strtod(nptr, &endptr);
        parseerror( nptr==endptr, c_iter, c_begin, c_end );

        ++amp_iter;
        parseerror( amp_iter == amp_end, c_iter, c_begin, c_end );

        nptr = (*amp_iter).c_str();
        phase = strtod(nptr, &endptr);
        parseerror( nptr==endptr, c_iter, c_begin, c_end );

        amps.push_back( amp*exp(EvtComplex(0.0, phase)) );

        ++amp_iter;
    }

    return amps;
}

vector<EvtMNode *> EvtMTree::duplicate( const vector<EvtMNode *>& list ) const
{
    vector<EvtMNode *> newlist;

    for(int i=0; i<list.size(); ++i)
        newlist.push_back( list[i]->duplicate() );

    return newlist;
}


// XXX Warning it is unsafe to use cl1 after a call to this function XXX
vector< vector<EvtMNode * > > EvtMTree::unionChildren( const string& nodestr,
        vector< vector<EvtMNode * > >& cl1 ) 
{
    vector<EvtMNode *> cl2 = parsenode( nodestr, false );
    vector< vector<EvtMNode * > > cl;
    
    if( cl1.size() == 0 ) {
        for( int i=0; i<cl2.size(); ++i ) {
            vector<EvtMNode *> temp(1, cl2[i]);
            cl.push_back( temp );
        }

        return cl;
    }

    for( int i=0; i<cl1.size(); ++i ) {
        for( int j=0; j<cl2.size(); ++j ) {
            vector<EvtMNode *> temp;
            temp = duplicate( cl1[i] );
            temp.push_back( cl2[j]->duplicate() );

            cl.push_back( temp );
        }
    }
 
    for(int i=0; i<cl1.size(); ++i)
        for(int j=0; j<cl1[i].size(); ++j)
            delete cl1[i][j];
    for(int i=0; i<cl2.size(); ++i)
        delete (cl2[i]);

    return cl;
}

vector< vector<EvtMNode * > > EvtMTree::parseChildren( ptype &c_iter, 
        ptype &c_begin, ptype &c_end ) 
{
    bool test = true;
    int pcount=0;
    string nodestr;
    vector< vector<EvtMNode * > > children;

    parseerror(c_iter == c_end || *c_iter != '[', c_iter, c_begin, c_end );
    ++c_iter;

    while( test ) {
        parseerror( c_iter==c_end || pcount < 0, c_iter, c_begin, c_end );

        switch( *c_iter ) {
            case ')':
                --pcount;
                nodestr += *c_iter;
                break;
            case '(':
                ++pcount;
                nodestr += *c_iter;
                break;
            case ']':
                if( pcount==0 ) {
                    children = unionChildren( nodestr, children );
                    test=false;
                } else {
                    nodestr += *c_iter;
                }
                break;
            case ',':
                if( pcount==0 ) {
                    children = unionChildren( nodestr, children );
                    nodestr.erase();
                } else {
                    nodestr += *c_iter;
                }
                break;
            default:
                nodestr += *c_iter;
                break;
        }

        ++c_iter;
    }

    return children;
}
    
vector<EvtMNode *> EvtMTree::parsenode( const string& args, bool rootnode )
{
    ptype c_iter, c_begin, c_end;

    c_iter=c_begin=args.begin();
    c_end = args.end();

    string strid = parseId( c_iter, c_begin, c_end );

    // Case 1: Particle
    if( c_iter == c_end ) return makeparticles( strid );

    // Case 2: Resonance - parse further
    EvtId id = EvtPDL::getId(strid);
    parseerror(EvtId( -1, -1 )==id, c_iter, c_begin, c_end);
    
    string ls;
    vector<string> lsarg;

    if( rootnode ) {
        ls = "TRIVIAL";
    } else {
        // Get lineshape (e.g. BREITWIGNER)
        ls = parseKey( c_iter, c_begin, c_end );
        lsarg = parseArg( c_iter, c_begin, c_end );
    }

    // Get resonance parametrization type (e.g. HELAMP)
    string type = parseKey( c_iter, c_begin, c_end );
    vector<EvtComplex> amps = parseAmps( c_iter, c_begin, c_end );

    // Children
    vector<vector<EvtMNode * > > children = parseChildren( c_iter, c_begin,
            c_end );
    
    report(ERROR,"EvtGen")<<children.size()<<endl;
    vector<EvtMNode *> resonances;
    for(int i=0; i<children.size(); ++i ) {
        resonances.push_back(makeresonance(id,ls,lsarg,type,amps,children[i]));
    }

    parseerror(c_iter == c_end || *c_iter!=')', c_iter, c_begin, c_end);

    return resonances;
}

bool EvtMTree::validTree( const EvtMNode * root ) const
{
    vector<int> res = root->getresonance();
    vector<bool> check(res.size(), false);

    for( int i=0; i<res.size(); ++i) {
        check[res[i]] = true;
    }

    bool ret = true;

    for( int i=0; i<check.size(); ++i ) {
        ret = ret&&check[i];
    }

    return ret;
}

void EvtMTree::addtree( const string& str )
{
    vector<EvtMNode *> roots = parsenode( str, true );
    _norm = 0;

    for( int i=0; i<roots.size(); ++i ) {
        if( validTree( roots[i] ) ) {
            _root.push_back( roots[i] );
            _norm = _norm + 1;
        } else
            delete roots[i];
    }
    
    _norm = 1.0/sqrt(_norm);
}

EvtSpinAmp EvtMTree::amplitude( const vector<EvtVector4R>&
        product) const
{
    if( _root.size() == 0 ) {
        report(ERROR, "EvtGen")<<"No decay tree present."<<endl;
        ::abort();
    }
    
    EvtSpinAmp amp = _root[0]->amplitude( product );
    for( int i=1; i<_root.size(); ++i ) {
        // Assume that helicity amplitude is returned and rotate to standard
        // amplitude here, do this before adding the amplitudes (different
        // frames?)
        amp += _root[i]->amplitude( product );
    }

    return _norm*amp;
}
