/*
 	Ray
    Copyright (C)  2010, 2011  Sébastien Boisvert

	http://DeNovoAssembler.SourceForge.Net/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You have received a copy of the GNU General Public License
    along with this program (COPYING).  
	see <http://www.gnu.org/licenses/>
*/

#include<assert.h>
#include<common_functions.h>
#include<Read.h>
#include<cstdlib>
#include<iostream>
#include<cstring>
using namespace  std;

// #define __READ_VERBOSITY

char*Read::trim(char*buffer,const char*sequence){
	int theLen=strlen(sequence);
	strcpy(buffer,sequence);
	for(int i=0;i<theLen;i++){
		if(buffer[i]=='a')
			buffer[i]='A';
		else if(buffer[i]=='t')
			buffer[i]='T';
		else if(buffer[i]=='c')
			buffer[i]='C';
		else if(buffer[i]=='g')
			buffer[i]='G';
	}
	// discard N at the beginning and end of the read.
	// find the first symbol that is a A,T,C or G
	int first=0;
	while(buffer[first]!='A' and buffer[first]!='T' and buffer[first]!='C' and buffer[first]!='G' and first<theLen){
		first++;
	}
	char*corrected=buffer+first;
	// find the last symbol that is a A,T,C, or G
	int last=0;
	for(int i=0;i<(int)strlen(corrected);i++){
		if(corrected[i]=='A' or corrected[i]=='T' or corrected[i]=='C' or corrected[i]=='G'){
			last=i;
		}
	}
	last++;
	// only junk awaits beyond <last>
	corrected[last]='\0';
	return corrected;
}

void Read::constructorWithRawSequence(const char*seq,uint8_t*raw,bool flag){
	m_type=TYPE_SINGLE_END;
	m_length=strlen(seq);
	m_sequence=raw;
}

void Read::constructor(const char*sequence,MyAllocator*seqMyAllocator,bool trimFlag){
	m_type=TYPE_SINGLE_END;
	#ifdef __READ_VERBOSITY
	cout<<"In="<<sequence<<endl;
	#endif
	if(trimFlag && strlen(sequence)<4096){
		char buffer[4096];
		sequence=trim(buffer,sequence);
	}
	int length=strlen(sequence);
	m_length=length;
	int requiredBytes=getRequiredBytes();

	uint8_t workingBuffer[4096];
	for(int i=0;i<requiredBytes;i++){
		workingBuffer[i]=0;
	}

	for(int position=0;position<length;position++){
		char nucleotide=sequence[position];
		if(nucleotide!='A'&&nucleotide!='T'&&nucleotide!='C'&&nucleotide!='G'){
			nucleotide='A';
		}
		uint8_t code=charToCode(nucleotide);
		#ifdef __READ_VERBOSITY
		if(position%4==0){
			cout<<"|";
		}
		cout<<" "<<(int)code;
		#endif
		int positionInWorkingBuffer=position/4;
		int codePositionInWord=position%4;
		uint8_t wordToUpdate=workingBuffer[positionInWorkingBuffer];
		// shift the code and or with the word to update
		code=(code<<(codePositionInWord*2));
		wordToUpdate=wordToUpdate|code;
		workingBuffer[positionInWorkingBuffer]=wordToUpdate;
	}
	#ifdef __READ_VERBOSITY
	cout<<endl;
	for(int i=0;i<requiredBytes;i++){
		cout<<" "<<(int)workingBuffer[i];
	}

	cout<<endl;
	#endif

	if(requiredBytes==0){
		m_sequence=NULL;
	}else{
		m_sequence=(uint8_t*)seqMyAllocator->allocate(requiredBytes*sizeof(uint8_t));
		memcpy(m_sequence,workingBuffer,requiredBytes);
	}

	#ifdef __READ_VERBOSITY
	cout<<"Out="<<getSeq()<<endl;
	cout<<endl;
	#endif
}

void Read::getSeq(char*workingBuffer) const{
	for(int position=0;position<m_length;position++){
		int positionInWorkingBuffer=position/4;
		uint8_t word=m_sequence[positionInWorkingBuffer];
		int codePositionInWord=position%4;
		uint8_t code=(word<<(6-codePositionInWord*2));//eliminate bits before
		code=(code>>6);
		char nucleotide=codeToChar(code);
		workingBuffer[position]=nucleotide;
	}
	workingBuffer[m_length]='\0';
}

int Read::length()const{
	return m_length;
}

/*                      
 *           -----------------------------------
 *           -----------------------------------
 *                     p p-1 p-2               0
 */
uint64_t Read::getVertex(int pos,int w,char strand,bool color) const {
	char buffer[4000];
	getSeq(buffer);
	return kmerAtPosition(buffer,pos,w,strand,color);
}

bool Read::hasPairedRead()const{
	return m_type!=TYPE_SINGLE_END;
}

PairedRead*Read::getPairedRead(){
	if(m_type==TYPE_SINGLE_END){
		return NULL;
	}
	return &m_pairedRead;
}

uint8_t*Read::getRawSequence(){
	return m_sequence;
}

int Read::getRequiredBytes(){
	int requiredBits=2*m_length;
	int modulo=requiredBits%8;
	if(modulo!=0){
		int bitsToAdd=8-modulo;
		requiredBits+=bitsToAdd;
	}

	#ifdef ASSERT
	assert(requiredBits%8==0);
	#endif

	int requiredBytes=requiredBits/8;
	return requiredBytes;
}

void Read::setRawSequence(uint8_t*seq,int length){
	m_sequence=seq;
	m_length=length;
}

void Read::setLeftType(){
	m_type=TYPE_LEFT_END;
}

void Read::setRightType(){
	m_type=TYPE_RIGHT_END;
}

int Read::getType(){
	return m_type;
}

void Read::setType(uint8_t a){
	m_type=a;
}
