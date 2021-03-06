/*
 *  Ray -- Parallel genome assemblies for parallel DNA sequencing
 *  Copyright (C) 2013 Sébastien Boisvert
 *
 *  http://DeNovoAssembler.SourceForge.Net/
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You have received a copy of the GNU General Public License
 *  along with this program (gpl-3.0.txt).
 *  see <http://www.gnu.org/licenses/>
 */

#include "PathHandle.h"

#include <fstream>
#include <iostream>
using namespace std;

#include <string.h>
#include <stdint.h>

void PathHandle::operator=(const PathHandle &b) {
	m_value = b.m_value;
}

bool PathHandle::operator<(const PathHandle & b)const {
	return m_value < b.m_value;
}

bool PathHandle::operator!=(const PathHandle & b)const {
	return m_value != b.m_value;
}

bool PathHandle::operator==(const PathHandle & b)const {
	return m_value == b.m_value;
}

void PathHandle::operator=(const uint64_t & value) {
	m_value = value;
}

PathHandle::PathHandle(const uint64_t & value) {
	m_value = value;
}

const uint64_t & PathHandle::operator () () const {
	return m_value;
}

uint64_t & PathHandle::operator () () {
	return m_value;
}

PathHandle::PathHandle() {
	m_value = 0;
}

uint64_t PathHandle::operator / (uint64_t value) {
	return m_value / value;
}

void PathHandle::setValue(uint64_t value) {
	m_value = value;
}

istream & operator >>(istream & stream, PathHandle & handle) {

	uint64_t myValue = 0;
	stream >> myValue;
	handle.setValue(myValue);

	return stream;
}

ostream & operator <<(ostream & stream, const PathHandle & handle) {

	stream << handle.getValue();

	return stream;
}

uint64_t PathHandle::operator * (uint64_t value) {
	return m_value * value;
}

uint64_t PathHandle::operator - (uint64_t value) {
	return m_value - value;
}

uint64_t PathHandle::operator + (uint64_t value) {
	return m_value + value;
}

uint64_t PathHandle::operator % (uint64_t value) {
	return m_value % value;
}


const uint64_t & PathHandle::getValue() const {
	return m_value;
}

bool PathHandle::operator>(const PathHandle & b)const {
	return m_value > b.m_value;
}

bool PathHandle::operator<=(const PathHandle & b)const {
	return m_value <= b.m_value;
}

bool PathHandle::operator>=(const PathHandle & b)const {
	return m_value >= b.m_value;
}

uint64_t & PathHandle::getValue() {
	return m_value;
}

int PathHandle::load(const char * buffer) {
	int size = sizeof(uint64_t);
	memcpy(&m_value, buffer, size);
	return size;
}

int PathHandle::dump(char * buffer) const {

	int size = sizeof(uint64_t);
	memcpy(buffer, &m_value, size);
	return size;
}


int PathHandle::getRequiredNumberOfBytes() const {
	return sizeof(uint64_t);
}
