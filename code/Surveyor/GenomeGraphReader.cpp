/*
    Copyright 2013 Sébastien Boisvert
    Copyright 2013 Université Laval
    Copyright 2013 Centre Hospitalier Universitaire de Québec

    This file is part of Ray Surveyor.

    Ray Surveyor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    Ray Surveyor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ray Surveyor.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "GenomeGraphReader.h"
#include "CoalescenceManager.h"

#include <code/Mock/constants.h>
#include <code/Mock/common_functions.h>
#include <code/KmerAcademyBuilder/Kmer.h>
#include <code/VerticesExtractor/Vertex.h>

#include <iostream>
#include <sstream>
using namespace std;

#include <string.h>

GenomeGraphReader::GenomeGraphReader() {

}

GenomeGraphReader::~GenomeGraphReader() {

}

void GenomeGraphReader::receive(Message & message) {

	int type = message.getTag();

	/*
	printName();
	cout << "received tag " << type << endl;
*/

	if(type == START_PARTY) {

		startParty(message);

	} else if(type == CoalescenceManager::PAYLOAD_RESPONSE) {

		/*
		printName();
		cout << " DEBUG readLine because PAYLOAD_RESPONSE" << endl;
		*/
		// read the next line now !
		readLine();
	}
}

void GenomeGraphReader::startParty(Message & message) {

	char * buffer = (char*) message.getBufferBytes();

	memcpy(&m_aggregator, buffer, sizeof(int));
	//m_aggregator = *(int*)(message.getBufferBytes());

	m_reader.open(m_fileName.c_str());
	m_loaded = 0;

	m_parent = message.getSourceActor();

	/*
	printName();
	cout << "DEBUG startParty" << endl;
	cout << " bytes in message: " << message.getNumberOfBytes();
	cout << " must send messages to aggregator " << m_aggregator;
	cout << endl;
*/

	int source = message.getSourceActor();
	Message response;
	response.setTag(START_PARTY_OK);

	send(source, response);

	readLine();
}

// DONE 2013-10-16: add a BufferedLineReader class in RayPlatform
// and use it here.
void GenomeGraphReader::readLine() {

	char buffer[1024];
	buffer[0] = '\0';

	while(!m_reader.eof()) {
		m_reader.getline(buffer, 1024);

		// skip comment
		if(strlen(buffer) > 0 && buffer[0] == '#')
			continue;

		break;
	}

	if(m_reader.eof()) {

		m_reader.close();

		printName();
		cout << " finished reading file " << m_fileName;
		cout << " got " << m_loaded << " objects" << endl;

		Message finishedMessage;
		finishedMessage.setTag(DONE);

		send(m_parent, finishedMessage);

		die();
	} else {

		// AGCTGTGAAACTGGTGCAAGCTACCAGAATC;36;A;C
		string sequence;
		CoverageDepth coverage;
		string parents;
		string children;

		for(int i = 0 ; i < (int) strlen(buffer) ; ++i) {
			if(buffer[i] == ';')
				buffer[i] = ' ';
		}

		istringstream stringBuffer(buffer);

		stringBuffer >> sequence;
		stringBuffer >> coverage;
		stringBuffer >> parents;
		stringBuffer >> children;

#if 0
		cout << "DEBUG " << sequence << " with " << coverage << endl;
#endif

		// if this is the first one, send the k-mer length too
		if(m_loaded == 0) {

			Message aMessage;
			aMessage.setTag(CoalescenceManager::SET_KMER_LENGTH);

			int length = sequence.length();
			aMessage.setBuffer(&length);
			aMessage.setNumberOfBytes(sizeof(length));

			send(m_aggregator, aMessage);
		}

		Kmer kmer;
		kmer.loadFromTextRepresentation(sequence.c_str());

		Vertex vertex;
		vertex.setKey(kmer);
		vertex.setCoverageValue(coverage);

		// add parents
		for(int i = 0 ; i < (int)parents.length() ; ++i) {

			string parent = sequence;
			for(int j = 0 ; j < (int) parent.length()-1 ; ++j) {
				parent[j + 1] = parent[j];
			}
			parent[0] = parents[i];

			Kmer parentKmer;
			parentKmer.loadFromTextRepresentation(parent.c_str());

			vertex.addIngoingEdge(&kmer, &parentKmer, sequence.length());
		}

		// add children
		for(int i = 0 ; i < (int)children.length() ; ++i) {

			string child = sequence;
			for(int j = 0 ; j < (int) child.length()-1 ; ++j) {
				child[j] = child[j + 1];
			}
			child[child.length() - 1] = children[i];

			Kmer childKmer;
			childKmer.loadFromTextRepresentation(child.c_str());

			vertex.addOutgoingEdge(&kmer, &childKmer, sequence.length());
		}

		char messageBuffer[100];
		int position = 0;

		position += vertex.dump(messageBuffer + position);

// TODO: accumulate many objects before flushing it.
// we can go up to MAXIMUM_MESSAGE_SIZE_IN_BYTES bytes.

		/*
		printName();
		cout << " got data line " << buffer;
		cout << " sending PAYLOAD to " << m_aggregator << endl;
*/
		Message message;
		message.setTag(CoalescenceManager::PAYLOAD);
		message.setBuffer(messageBuffer);
		message.setNumberOfBytes(position);

#if 0
		printName();
		cout << "DEBUG sending PAYLOAD to " << m_aggregator;
		cout << " with " << position << " bytes ";
		vertex.print(sequence.length(), false);
		cout << endl;
#endif

		if(m_loaded % 1000000 == 0) {
			printName();
			cout << " loaded " << m_loaded << " sequences" << endl;

		}
		m_loaded ++;
		send(m_aggregator, message);
	}
}

void GenomeGraphReader::setFileName(string & fileName) {

	m_fileName = fileName;

#if 0
	printName();
	cout << " DEBUG setFileName " << m_fileName << endl;
#endif
}
