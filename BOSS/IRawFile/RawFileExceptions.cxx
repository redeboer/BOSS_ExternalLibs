#include "IRawFile/RawFileExceptions.h"
#include <iostream>

void RawFileException::print() const {
   std::cout << "[RawFile] RawFileException::print()" << std::endl;
}

void RawExMessage::print() const {
   std::cout << m_msg << std::endl;
}

void InvalidRawFile::print() const {
   std::cout << "[RawFile] The file \"" << m_filename << "\" is invalid!" << std::endl;
}

void BadInputStream::print() const {
   std::cout << "[RawFile] Failed to read the block: " << m_blockName << std::endl;
}

void WrongMarker::print() const {
   std::cout << "[RawFile] Get an invalid marker, expect 0x" << std::hex << m_expect
      << ", but got 0x" << m_value << std::dec << std::endl;
}

void UnexpectedRecordSize::print() const {
   std::cout << "[RawFile] The size of block \"" << m_blockName << "\" is expected as "
      << m_expect << ", but got " << m_value << std::endl;
}

void ReachEndOfFile::print() const {
   std::cout << "[RawFile] Reach end of data file: " << m_fileName << std::endl;
}
void ReachEndOfFile::print(const char* fname) const {
   std::cout << "[RawFile] Reach end of data file: " << fname << std::endl;
}

void ReachEndOfFileList::print() const {
   std::cout << "[RawFile] Congratulations. Reach end of data file list !" << std::endl;
}

void FailedToWrite::print() const {
   std::cout << "[RawFile] Error occurred while writing block: " << m_blockName << std::endl;
}
