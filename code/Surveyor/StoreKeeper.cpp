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


#include "StoreKeeper.h"
#include "CoalescenceManager.h"

#include <code/VerticesExtractor/Vertex.h>

#include <iostream>
using namespace std;

#include <string.h>

StoreKeeper::StoreKeeper() {

}

StoreKeeper::~StoreKeeper() {

}

void StoreKeeper::receive(Message & message) {

	int tag = message.getTag();


	if(tag == PUSH_SAMPLE_VERTEX) {
		pushSampleVertex(message);

	} else if( tag == CoalescenceManager::DIE) {

		die();

	} else if(CoalescenceManager::SET_KMER_LENGTH) {

		int kmerLength = 0;
		char * buffer = (char*)message.getBufferBytes();
		memcpy(&kmerLength, buffer, sizeof(kmerLength));

		if(m_kmerLength == 0)
			m_kmerLength = kmerLength;

		// cout << "DEBUG m_kmerLength = " << m_kmerLength << endl;

		// the color space mode is an artefact.
		m_colorSpaceMode = false;

		//cout << "DEBUG StoreKeeper SET_KMER_LENGTH ";
		cout << m_kmerLength;
		cout << endl;

	}
}

void StoreKeeper::pushSampleVertex(Message & message) {
	char * buffer = (char*)message.getBufferBytes();
	//int bytes = message.getNumberOfBytes();

	int position = 0;
	Vertex vertex;
	position += vertex.load(buffer + position);

	int sample = -1;
	memcpy(&sample, buffer + position, sizeof(sample));

	printName();
	cout << " DEBUG received ";
	cout << "(from " << message.getSourceActor();
	cout << ") ";
	cout << "vertex for sample " << sample;
	cout << " with sequence ";
	vertex.print(m_kmerLength, m_colorSpaceMode);
	cout << endl;

}

