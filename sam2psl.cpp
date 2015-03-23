#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <complex>
#include <algorithm>
#include <string>
#include <vector>
#include <unistd.h>
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atoi */
#include <map>
using namespace std;

struct PSL_st{
  int matches;
  int misMatches;
  int repMatches;
  int nCount;
  int qNumInsert;
  int qBaseInsert;
  int tNumInsert;
  int tBaseInsert;
  string strand;
  string qName;
  int qSize;
  int qStart;
  int qEnd;
  string tName;
  int tSize;
  int tStart;
  int tEnd;
  int blockCount;
  vector<int> blockSizes;
  vector<int> qStarts;
  vector<int> tStarts;
  vector<string> qBlocks; //pslx
  vector<string> tBlocks; //pslx
};

struct PSLComment_st{
  int RI;            // read index
  string RNEXT;
  int PNEXT;
  int TLEN;            // read index
  int MAPQ;          // mapping quality
  int AS;            // mapping score given by software
  int MAPS;          // matches - edit distance
  bool FPAIRED;
  bool FPROPER_PAIR;
  bool FSECONDARY;
  bool FQCFAIL;
  bool FDUP;
  vector<int> qMismatches;
  vector<int> tMismatches;
  vector<int> cMismatches;
};



struct SAM_st{
  string QNAME; // String [!-?A-~]{1,255} Query template NAME
  int FLAG;     // Int [0,216-1] bitwise FLAG
  string RNAME; // String \*|[!-()+-<>-~][!-~]* Reference sequence NAME
  int POS;      // Int [0,231-1] 1-based leftmost mapping POSition
  int MAPQ;     // Int [0,28-1] MAPping Quality
  string CIGAR; // String \*|([0-9]+[MIDNSHPX=])+ CIGAR string
  string RNEXT; // String \*|=|[!-()+-<>-~][!-~]* Ref. name of the mate/next read
  int PNEXT;    // Int [0,231-1] Position of the mate/next read
  int TLEN;     // Int [-231+1,231-1] observed Template LENgth
  string SEQ;   // String \*|[A-Za-z=.]+ segment SEQuence
  string QUAL;  // String [!-~]+ ASCII of Phred-scaled base QUALity+33
  int AS;       // Alignment score generated by aligner
  int NM;       // Edit distance to the reference, including ambiguous bases but excluding clipping
  string MD;    // String for mismatching positions. Regex : [0-9]+(([A-Z]|\^[A-Z]+)[0-9]+)*
} ;
/*
  The MD field aims to achieve SNP/indel calling without looking at the reference. For example, a string ‘10A5^AC6’
  means from the leftmost reference base in the alignment, there are 10 matches followed by an A on the reference which
  is different from the aligned read base; the next 5 reference bases are matches followed by a 2bp deletion from the
  reference; the deleted sequence is AC; the last 6 bases are matches. The MD field ought to match the CIGAR string.
*/

struct FLAG_st{
  bool FPAIRED;
  bool FPROPER_PAIR;
  bool FUNMAP;
  bool FMUNMAP;
  bool FREVERSE;
  bool FMREVERSE;
  bool FREAD1;
  bool FREAD2;
  bool FSECONDARY;
  bool FQCFAIL;
  bool FDUP;
};

/* from bam.h */
/*
  CIGAR operations.
*/
/*! @abstract CIGAR: M = match or mismatch*/
#define BAM_CMATCH      0
/*! @abstract CIGAR: I = insertion to the reference */
#define BAM_CINS        1
/*! @abstract CIGAR: D = deletion from the reference */
#define BAM_CDEL        2
/*! @abstract CIGAR: N = skip on the reference (e.g. spliced alignment) */
#define BAM_CREF_SKIP   3
/*! @abstract CIGAR: S = clip on the read with clipped sequence
  present in qseq */
#define BAM_CSOFT_CLIP  4
/*! @abstract CIGAR: H = clip on the read with clipped sequence trimmed off */
#define BAM_CHARD_CLIP  5
/*! @abstract CIGAR: P = padding */
#define BAM_CPAD        6
/*! @abstract CIGAR: equals = match */
#define BAM_CEQUAL      7
/*! @abstract CIGAR: X = mismatch */
#define BAM_CDIFF       8
#define BAM_CBACK       9
#define BAM_CIGAR_STR  "MIDNSHP=XB"
#define BAM_CIGAR_TYPE 0x3C1A7

/*! @abstract the read is paired in sequencing, no matter whether it is mapped in a pair */
#define BAM_FPAIRED        1
/*! @abstract the read is mapped in a proper pair */
#define BAM_FPROPER_PAIR   2
/*! @abstract the read itself is unmapped; conflictive with BAM_FPROPER_PAIR */
#define BAM_FUNMAP         4
/*! @abstract the mate is unmapped */
#define BAM_FMUNMAP        8
/*! @abstract the read is mapped to the reverse strand */
#define BAM_FREVERSE      16
/*! @abstract the mate is mapped to the reverse strand */
#define BAM_FMREVERSE     32
/*! @abstract this is read1 */
#define BAM_FREAD1        64
/*! @abstract this is read2 */
#define BAM_FREAD2       128
/*! @abstract not primary alignment */
#define BAM_FSECONDARY   256
/*! @abstract QC failure */
#define BAM_FQCFAIL      512
/*! @abstract optical or PCR duplicate */
#define BAM_FDUP        1024

#define BAM_OFDEC          0
#define BAM_OFHEX          1
#define BAM_OFSTR          2


struct POSCIGAR_st{
  int tid;              // reference id if available
  int base;             // 0 or 1
  int pos;              // 0-based position in chromosome
  int qStart;           // 0-based start position in read
  int tStart;           // 0-based start position in reference
  int qEnd;             // 0-based end posiiton in read
  int tEnd;             // 0-based end position in reference
  int qual;             // mapping quality
  int anchor;           // 0-based position in chromosome
  int iclip;            // 0-based index of op of match, M/D/I
  int l_qseq;           // length of qseq
  vector<int> op;       // CIGAR operations
  vector<int> nop;      // number before operation
  vector<int> qop;      // start positions in read
  vector<int> top;      // start positions in target
  POSCIGAR_st(): tid(-1),
		 base(-1),
		 pos(-1),
		 qStart(-1),
		 tStart(-1),
		 qEnd(-1),
		 tEnd(-1),
		 qual(-1),
		 anchor(-1),
		 iclip(-1),
		 l_qseq(-1) {}
};

/* 0-based closed
 */
void resolve_cigar_pos(int POS, string& CIGAR, POSCIGAR_st& m)  
{  
  if ( CIGAR=="*" ) { m.pos=-1; return; }
  
  char numchar[]="99999999999999";
  const string ops=BAM_CIGAR_STR;
  
  m.base=0;
  m.pos=POS;
  m.tStart=POS;
  m.anchor=-1;
  m.iclip=-1;
  m.op.resize(0);
  m.nop.resize(0);
  m.l_qseq=0;
  
  for(int i=0, k=0; i<(int)CIGAR.size(); ++i) {
    if ( CIGAR[i]>='0' && CIGAR[i]<='9'  ) {
      numchar[k]=CIGAR[i];
      ++k;
      if ( k>9 ) { cerr << "CIGAR error:" << CIGAR << endl; exit(1); }
    }
    else {
      size_t cigar_num=ops.find(CIGAR[i]);
      if ( cigar_num==string::npos) { 
	cerr << "Unknown CIGAR:\t" << CIGAR << endl; 
	exit(1); 
      }
      numchar[k]=0;
      m.op.push_back(cigar_num);
      m.nop.push_back(atoi(numchar));
      k=0;
    }
  }
  
  m.qop.resize(m.op.size());
  m.top.resize(m.op.size());
  
  int ns=0;
  int end=0; //position on seq 0-based
  for (int k = 0; k < (int)m.op.size(); ++k) {
    
    int op=m.op[k];
    int l=m.nop[k];
    m.qop[k]=end;
    
    if ( op == BAM_CMATCH || 
	 op == BAM_CINS || 
	 op == BAM_CSOFT_CLIP || 
	 op == BAM_CEQUAL || 
	 op == BAM_CDIFF ) end+=l;
    
    if (  op == BAM_CSOFT_CLIP && l > ns) { ns=l; m.iclip=k; }
    
    if ( m.op[k] == BAM_CMATCH || 
	 m.op[k] == BAM_CDEL || 
	 m.op[k] == BAM_CEQUAL || 
	 m.op[k] == BAM_CDIFF) { if ( m.anchor<0 ) m.anchor=k; }
    
  }
  if ( m.anchor<0 ) { m.pos=0; return; }
  
  // position on reference 0-based
  // BAM_CSOFT_CLIP is added because we need positions of S 
  end = m.pos;  //position on reference
  for (int k = m.anchor; k < (int)m.op.size(); ++k) {
    m.top[k]=end;
    if (  m.op[k] == BAM_CMATCH || 
	  m.op[k] == BAM_CDEL || 
	  m.op[k] == BAM_CREF_SKIP || 
	  m.op[k] == BAM_CSOFT_CLIP ) end += m.nop[k] ;
  }
  end = m.pos;
  for (int k = m.anchor-1; k >= 0; --k) {
    if (  m.op[k] == BAM_CMATCH || 
	  m.op[k] == BAM_CDEL || 
	  m.op[k] == BAM_CREF_SKIP || 
	  m.op[k] == BAM_CSOFT_CLIP )  end -= m.nop[k] ;
    m.top[k]=end;
  }
  
  // get qStart, tStart, qEnd, qEnd
  m.qStart=m.qop[ m.anchor ];
  m.tStart=m.top[ m.anchor ];
  for (int k = (int)m.op.size()-1; k>=0; --k) {
    if ( m.op[k] == BAM_CSOFT_CLIP ||
	 m.op[k] == BAM_CHARD_CLIP ) continue;
    m.qEnd=m.qop[k]+m.nop[k]-1;
    m.tEnd=m.top[k]+m.nop[k]-1;
  }
  if( m.qEnd<=m.qStart || m.tStart!=m.pos ||
      m.tEnd<=m.tStart ) {
    cerr << "Error resolving CIGAR:\t" << CIGAR << "\n"
	 << "tStart=" << m.tStart << "\n"
	 << "qStart=" << m.qStart << "\n"
	 << "qEnd=" << m.qEnd << endl;
    exit(1);
  }
  
  m.l_qseq=m.qop.back()+m.nop.back()*(m.op.back()!=BAM_CHARD_CLIP);
  return;  
}  



int main(int argc, char* argv[])
{
  
  if ( argc>=2 ) {
    for( int i=1; i<argc; ++i) {
      if( string( argv[i]).find("-h")!=string::npos) 
	cout << "matches\tmisMatches\trepMatches\tnCount\tqNumInsert\t"
	     << "qBaseInsert\ttNumInsert\ttBaseInsert\tstrand\tqName\t"
	     << "qSize\tqStart\tqEnd\ttName\ttSize\t"
	     << "tStart\ttEnd\tblockCount\tblockSizes\tqStarts\t"
	     << "tStarts\tqBlocks\ttBlocks\t"
	     << "RI\tRNEXT\tPNEXT\tTLEN\tMAPQ\t"
	     << "AS\tMAPS\tFPAIRED\tFPROPER_PAIR\tFSECONDARY\t"
	     << "FQC\tFDUP"
	     << "\n";
    } 
  }
  
  string line;
  map<string,int> targetLen;  
  
  while( getline(cin, line) ) {
    istringstream iss(line);
    if ( line[0]=='@' ) {
      if ( line.find("@SQ")==string::npos ) continue; 
      string tmps, SN, LN;
      iss >> tmps >> SN >> LN;
      size_t p1=SN.find("SN:");
      if ( p1==string::npos ) continue; 
      SN=SN.substr(p1+3);
      p1=LN.find("LN:");
      if ( p1==string::npos ) continue; 
      LN=LN.substr(p1+3);
      targetLen[SN]=atoi(LN.c_str());
    }
    vector<string> field(0);
    string tmps;
    while( iss>>tmps ) field.push_back(tmps); 
    if( field.size()<11 ) continue;
    if( field[0][0]=='@' ) continue;
    
    PSL_st psl;
    SAM_st sam;
    FLAG_st flag;
    PSLComment_st pslc;
    
    sam.QNAME=field[0];
    sam.FLAG=atoi(field[1].c_str());
    sam.RNAME=field[2];
    sam.POS=atoi(field[3].c_str());
    sam.MAPQ=atoi(field[4].c_str());
    sam.CIGAR=field[5];
    sam.RNEXT=field[6];
    sam.PNEXT=atoi(field[7].c_str());
    sam.TLEN=atoi(field[8].c_str());
    sam.SEQ=field[9];
    sam.QUAL=field[10];
    sam.AS=-sam.SEQ.length();
    sam.NM=sam.SEQ.length();
    sam.MD="";
    int optionalField=0;
    for(int i=11; i<(int)field.size(); ++i) {
      if( field[i].substr(0,3)=="AS:" ) {
	size_t p=field[i].rfind(':');
	sam.AS=atoi( field[i].substr(p+1).c_str() );
	optionalField+=1;
      }
      if( field[i].substr(0,3)=="NM:" ) {
	size_t p=field[i].rfind(':');
	sam.NM=atoi( field[i].substr(p+1).c_str() );
	optionalField+=1;
      }
      if( field[i].substr(0,3)=="MD:" ) {
	size_t p=field[i].rfind(':');
	sam.MD=field[i].substr(p+1);
	optionalField+=1;
      }
      if( optionalField==3 ) break;
    }
    
    flag.FPAIRED = bool(sam.FLAG & BAM_FPAIRED);
    flag.FPROPER_PAIR = bool(sam.FLAG & BAM_FPROPER_PAIR);
    flag.FUNMAP = bool(sam.FLAG & BAM_FUNMAP);
    flag.FMUNMAP = bool(sam.FLAG & BAM_FMUNMAP);
    flag.FREVERSE = bool(sam.FLAG & BAM_FREVERSE); 
    flag.FMREVERSE = bool(sam.FLAG & BAM_FMREVERSE);
    flag.FREAD1 = bool(sam.FLAG & BAM_FREAD1);
    flag.FREAD2 = bool(sam.FLAG & BAM_FREAD2);
    flag.FSECONDARY = bool(sam.FLAG & BAM_FSECONDARY);
    flag.FQCFAIL = bool(sam.FLAG & BAM_FQCFAIL);
    flag.FDUP = bool(sam.FLAG & BAM_FDUP);

    POSCIGAR_st m;
    resolve_cigar_pos(sam.POS-1, sam.CIGAR, m);  // sam is 1-based, psl is 0-based
    if ( sam.SEQ.size() != m.l_qseq && m.l_qseq>0 ) {
      cerr << "SEQ length not calculated correctly\t"
	   << line << endl
	   << sam.SEQ.size() << "\t" << m.l_qseq << endl;
      exit(1);
    }
    
    psl.matches=0;
    psl.misMatches=sam.NM;
    psl.repMatches=0;
    psl.nCount=0;
    psl.qNumInsert=0;
    psl.qBaseInsert=0;
    psl.tNumInsert=0;
    psl.tBaseInsert=0;
    psl.strand= (sam.FLAG & BAM_FREVERSE) > 0 ? "-":"+";
    psl.qName=sam.QNAME;
    psl.qSize=sam.SEQ.size();
    psl.qStart=m.qStart;
    psl.qEnd=m.qEnd+1;  // m is 0-base closed, psl is 0-base half open
    psl.tName=sam.RNAME;
    psl.tSize=targetLen[sam.RNAME];
    psl.tStart=m.tStart;
    psl.tEnd=m.tEnd+1;  // m is 0-base closed, psl is 0-base half open
    psl.blockCount=0;
    psl.blockSizes.resize(0);
    psl.qStarts.resize(0);
    psl.tStarts.resize(0);
    for( int i=0; i<sam.SEQ.size(); ++i) {
      if( sam.SEQ[i]=='N' || sam.SEQ[i]=='n' ) psl.nCount+=1;
    }
    for( int i=0; i<m.op.size(); ++i) {
      if ( m.op[i] == BAM_CMATCH || 
	   m.op[i] == BAM_CDEL || 
	   m.op[i] == BAM_CEQUAL || 
	   m.op[i] == BAM_CDIFF) psl.matches+=m.nop[i];
      
      if ( m.op[i] == BAM_CINS ) {
	psl.qNumInsert+=1;
	psl.qBaseInsert+=m.nop[i];
      }
      
      if ( m.op[i] == BAM_CDEL ) {
	psl.tNumInsert+=1;
	psl.tBaseInsert+=m.nop[i];
      }
      
      if ( m.op[i] == BAM_CMATCH ) {
	psl.blockCount+=1;
	psl.blockSizes.push_back( m.nop[i] );
	psl.qStarts.push_back( m.qop[i] );
	psl.tStarts.push_back( m.top[i] );
      }
    }
    // note so far it is assumed strand is +
    if ( psl.strand=="-" ) {
      psl.qStart=psl.qSize-psl.qEnd;
      psl.qEnd=psl.qSize-psl.qStart;
      for(int i=0; i<psl.blockSizes.size(); ++i) {
	int qEndOnPlus=psl.qStarts[i]+psl.blockSizes[i];
	psl.qStarts[i]=psl.qSize-qEndOnPlus;
	// note tStart remain same
      }
      reverse(psl.blockSizes.begin(), psl.blockSizes.end());
      reverse(psl.qStarts.begin(), psl.qStarts.end());
      reverse(psl.tStarts.begin(), psl.tStarts.end());
    }
    
    pslc.RI=1;
    pslc.RNEXT=sam.RNEXT;
    pslc.PNEXT=sam.PNEXT-1; //SAM is 1-based
    pslc.TLEN=sam.TLEN;
    pslc.MAPQ=sam.MAPQ;
    pslc.AS=sam.AS;
    pslc.MAPS=psl.matches-psl.misMatches;
    pslc.FPAIRED=flag.FPAIRED;
    pslc.FPROPER_PAIR=flag.FPROPER_PAIR;
    pslc.FSECONDARY=flag.FSECONDARY;
    pslc.FQCFAIL=flag.FQCFAIL;
    pslc.FDUP=flag.FDUP;
    if ( !flag.FPAIRED ) pslc.RI=1;
    else {
      pslc.RI=0;
      if( flag.FREAD1 ) pslc.RI=1;
      if( flag.FREAD2 ) pslc.RI=2;
      if( pslc.RI==0 | flag.FREAD1==flag.FREAD2 ) {
	cerr << "check flag" << endl
	     << line << endl;
	exit(1);
      }
    }
    
    cout << "#"<< line << "\n";
    cout << psl.matches << "\t"
	 << psl.misMatches << "\t"
	 << psl.repMatches << "\t"
	 << psl.nCount << "\t"
	 << psl.qNumInsert << "\t"
	 << psl.qBaseInsert << "\t"
	 << psl.tNumInsert << "\t"
	 << psl.tBaseInsert << "\t"
	 << psl.strand << "\t"
	 << psl.qName << "\t"
	 << psl.qSize << "\t"
	 << psl.qStart << "\t"
	 << psl.qEnd << "\t"
	 << psl.tName << "\t"
	 << psl.tSize << "\t"
	 << psl.tStart << "\t"
	 << psl.tEnd << "\t"
	 << psl.blockCount << "\t";
    for(int i=0; i<psl.blockSizes.size(); ++i) cout << psl.blockSizes[i] << ",";
    cout << "\t";
    for(int i=0; i<psl.qStarts.size(); ++i) cout << psl.qStarts[i] << ",";
    cout << "\t";
    for(int i=0; i<psl.tStarts.size(); ++i) cout << psl.tStarts[i] << ",";
    cout << "\t";
    
    cout << "," << "\t";  // reserve for pslx
    cout << "," << "\t";  // reserve for pslx
    
    cout << pslc.RI << "\t"
	 << pslc.RNEXT << "\t"
	 << pslc.PNEXT << "\t"
	 << pslc.TLEN << "\t"
	 << pslc.MAPQ << "\t"
	 << pslc.AS << "\t"
	 << pslc.MAPS << "\t"
	 << "SP"[pslc.FPAIRED] << "\t"
	 << "UP"[pslc.FPROPER_PAIR] << "\t"
	 << "PS"[pslc.FSECONDARY] << "\t"
	 << "PF"[pslc.FQCFAIL] << "\t"
	 << "PD"[pslc.FDUP];
    
    cout << endl;
  }
  
  return 0;
}
  
