#include "RawFile/RawFileUtil.h"
#include "RawFile/raw_ifstream.h"
#include "RawFile/raw_ofstream.h"
#include "IRawFile/RawFileExceptions.h"
#include <cstring>

raw_ifstream& operator>>(raw_ifstream& is, FileStartRecord& record) {
  // read data block
  if (is.read((char*)(&record.m_record), sizeof(file_start_record)).fail()) {
    //std::cerr << "[RawFile] Error occurred while reading files" << std::endl;
    throw BadInputStream("FileStartRecord");
  }

  // data validation checks
  if (record.m_record.marker != 0x1234aaaa) {
    //std::cerr << "[RawFile] Get an invalid record marker" << std::endl;
    throw WrongMarker(0x1234aaaa, record.m_record.marker);
  }

  if (record.m_record.record_size != 8) {
    //std::cerr << "[RawFile] Get an unexpected record size" << std::endl;
    throw UnexpectedRecordSize("FileStartRecord", 8, record.m_record.record_size);
  }

  return is;
}

raw_ifstream& operator>>(raw_ifstream& is, FileNameStrings& record) {
  // read marker
  if (is.read((char*)(&record.m_record.marker), sizeof(uint32_t)).fail()) {
    //std::cerr << "[RawFile] Error occurred while reading files" << std::endl;
    throw BadInputStream("FileNameStrings::marker");
  }

  // marker validation
  if (record.m_record.marker != 0x1234aabb) {
    //std::cerr << "[RawFile] Get an invalid record marker" << std::endl;
    throw WrongMarker(0x1234aabb, record.m_record.marker);
  }

  // read length and strings
  if (is.read((char*)(&record.m_record.length1), sizeof(uint32_t)).fail()) {
    //std::cerr << "[RawFile] Error occurred while reading files" << std::endl;
    throw BadInputStream("FileNameStrings::length1");
  }

  uint32_t length1_word = (record.m_record.length1 + 3) / 4;
  char* appName = new char[length1_word * 4 + 1];
  if (is.read(appName, length1_word*4).fail()) {
    //std::cerr << "[RawFile] Error occurred while reading files" << std::endl;
    throw BadInputStream("FileNameStrings::appName");
  }
  appName[record.m_record.length1] = '\0';
  record.m_appName = appName;
  delete[] appName;

  if (is.read((char*)(&record.m_record.length2), sizeof(uint32_t)).fail()) {
    //std::cerr << "[RawFile] Error occurred while reading files" << std::endl;
    throw BadInputStream("FileNameStrings::length2");
  }

  uint32_t length2_word = (record.m_record.length2 + 3) / 4;
  char* usrTag = new char[length2_word * 4 + 1];
  if (is.read(usrTag, length2_word*4).fail()) {
    //std::cerr << "[RawFile] Error occurred while reading files" << std::endl;
    throw BadInputStream("FileNameStrings::usrTag");
  }
  usrTag[record.m_record.length2] = '\0';
  record.m_usrTag = usrTag;
  delete[] usrTag;

  return is;
}

raw_ifstream& operator>>(raw_ifstream&is, RunParametersRecord& record) {
  // read data block
  if (is.read((char*)(&record.m_record), sizeof(run_parameters_record)).fail()) {
    //std::cerr << "[RawFile] Error occurred while reading files" << std::endl;
    throw BadInputStream("RunParametersRecord");
  }

  // data validation checks
  if (record.m_record.marker != 0x1234bbbb) {
    //std::cerr << "[RawFile] Get an invalid record marker" << std::endl;
    throw WrongMarker(0x1234bbbb, record.m_record.marker);
  }

  if (record.m_record.record_size != 9) {
    //std::cerr << "[RawFile] Get an unexpected record size" << std::endl;
    throw UnexpectedRecordSize("RunParametersRecord", 9, record.m_record.record_size);
  }

  return is;
}

raw_ifstream& operator>>(raw_ifstream& is, DataSeparatorRecord& record) {
  // read data block
  if (is.read((char*)(&record.m_record), sizeof(data_separator_record)).fail()) {
    //std::cerr << "[RawFile] Error occurred while reading file" << std::endl;
    throw BadInputStream("DataSeparatorRecord");
    //throw ReachEndOfFile();
  }

  // data validation checks
  if (record.m_record.marker != 0x1234cccc) {
    if (record.m_record.marker == 0x1234dddd) {
      throw ReachEndOfFile(is.currentFile().c_str());
    }
    //std::cerr << "[RawFile] Get an invalid record marker" << std::endl;
    throw WrongMarker(0x1234cccc, record.m_record.marker);
  }

  if (record.m_record.record_size != 4) {
    //std::cerr << "[RawFile] Get an unexpected record size" << std::endl;
    throw UnexpectedRecordSize("DataSeparatorRecord", 4, record.m_record.record_size);
  }

  return is;
}

raw_ifstream& operator>>(raw_ifstream& is, FileEndRecord& record) {
  // read data block
  if (is.read((char*)(&record.m_record), sizeof(file_end_record)).fail()) {
    //std::cerr << "[RawFile] Error occurred while reading files" << std::endl;
    throw BadInputStream("FileEndRecord");
  }

  // data validation checks
  if (record.m_record.marker != 0x1234dddd) {
    //std::cerr << "[RawFile] Get an invalid record marker" << std::endl;
    throw WrongMarker(0x1234dddd, record.m_record.marker);
  }
  if (record.m_record.end_marker != 0x1234eeee) {
    //std::cerr << "[RawFile] Get an invalid record marker" << std::endl;
    throw WrongMarker(0x1234eeee, record.m_record.end_marker);
  }

  if (record.m_record.record_size != 10) {
    //std::cerr << "[RawFile] Get an unexpected record size" << std::endl;
    throw UnexpectedRecordSize("FileEndRecord", 10, record.m_record.record_size);
  }

  return is;
}

raw_ifstream& makeEndRecord(DataSeparatorRecord& sep, raw_ifstream& is, FileEndRecord& record) {
  // copy file_end_record data that already read by data_separator_record
  memcpy((void*)&record.m_record, (const void*)&(sep.getRecord()), sizeof(data_separator_record));
  // read data block
  if (is.read((char*)(&record.m_record.events_in_file), (
	  sizeof(file_end_record) - sizeof(data_separator_record)
	  )).fail()) {
    //std::cerr << "[RawFile] Error occurred while reading files" << std::endl;
    throw BadInputStream("FileEndRecord");
  }

  // data validation checks
  if (record.m_record.marker != 0x1234dddd) {
    //std::cerr << "[RawFile] Get an invalid record marker" << std::endl;
    throw WrongMarker(0x1234dddd, record.m_record.marker);
  }
  if (record.m_record.end_marker != 0x1234eeee) {
    //std::cerr << "[RawFile] Get an invalid record marker" << std::endl;
    throw WrongMarker(0x1234eeee, record.m_record.end_marker);
  }

  if (record.m_record.record_size != 10) {
    //std::cerr << "[RawFile] Get an unexpected record size" << std::endl;
    throw UnexpectedRecordSize("FileEndRecord", 10, record.m_record.record_size);
  }

  return is;
}

raw_ofstream& operator<<(raw_ofstream& os, FileStartRecord& record) {
  // write data block
  if (os.write((char*)(&record.m_record), sizeof(file_start_record)).fail()) {
    std::cerr << "[RawFile] Error occurred while writing file" << std::endl;
    throw FailedToWrite("FileStartRecord");
  }

  return os;
}

raw_ofstream& operator<<(raw_ofstream& os, FileNameStrings& record) {
  // write data block
  os.write((char*)(&record.m_record), sizeof(uint32_t)*2);
  uint32_t sizebyte = record.m_record.length1;
  uint32_t sizeword = (sizebyte+3)/4;
  os.write(record.m_appName.c_str(), sizeword*4);
  os.write((char*)(&record.m_record.length2), sizeof(uint32_t));
  sizebyte = record.m_record.length2;
  sizeword = (sizebyte+3)/4;
  os.write(record.m_usrTag.c_str(), sizeword*4);
  if ( os.fail() ) {
    std::cerr << "[RawFile] Error occurred while writing file" << std::endl;
    throw FailedToWrite("FileNameStrings");
  }
  return os;
}

raw_ofstream& operator<<(raw_ofstream& os, RunParametersRecord& record) {
  // write data block
  if (os.write((char*)(&record.m_record), sizeof(run_parameters_record)).fail()) {
    std::cerr << "[RawFile] Error occurred while writing file" << std::endl;
    throw FailedToWrite("RunParametersRecord");
  }

  return os;
}

raw_ofstream& operator<<(raw_ofstream& os, DataSeparatorRecord& record) {
  // write data block
  if (os.write((char*)(&record.m_record), sizeof(data_separator_record)).fail()) {
    std::cerr << "[RawFile] Error occurred while writing file" << std::endl;
    throw FailedToWrite("DataSeparatorRecord");
  }

  return os;
}

raw_ofstream& operator<<(raw_ofstream& os, FileEndRecord& record) {
  // write data block
  if (os.write((char*)(&record.m_record), sizeof(file_end_record)).fail()) {
    std::cerr << "[RawFile] Error occurred while writing file" << std::endl;
    throw FailedToWrite("FileEndRecord");
  }

  return os;
}

FileStartRecord::FileStartRecord()
{
   m_record.marker               = 0x1234aaaa;
   m_record.record_size          = 8;
   // following members are not meaningful @ offline
   m_record.version              = 0;
   m_record.file_number          = 0;
   m_record.date                 = 0;
   m_record.time                 = 0;
   m_record.sizeLimit_dataBlocks = 0;
   m_record.sizeLimit_MB         = 0;
}

FileNameStrings::FileNameStrings()
{
   m_record.marker   = 0x1234aabb;
   m_record.length1  = 4;
   m_record.length2  = 7;
   m_appName         = "BOSS";
   m_usrTag          = "offline";
}

RunParametersRecord::RunParametersRecord()
{
   m_record.marker        = 0x1234bbbb;
   m_record.record_size   = 9;
   // following members are not meaningful @ offline
   m_record.run_number    = 0;
   m_record.max_events    = 0;
   m_record.rec_enable    = 0;
   m_record.trigger_type  = 0;
   m_record.detector_mask = 0;
   m_record.beam_type     = 0;
   m_record.beam_energy   = 0;
}

DataSeparatorRecord::DataSeparatorRecord()
{
   m_record.marker        = 0x1234cccc;
   m_record.record_size   = 4;
   //m_record.data_block_number;  //set manually
   //m_record.data_block_size;    //set manually
}

FileEndRecord::FileEndRecord()
{
   m_record.marker          = 0x1234dddd;
   m_record.record_size     = 10;
   // following members are not meaningful @ offline
   m_record.date            = 0;
   m_record.time            = 0;
   //m_record.events_in_file;     //set manually
   m_record.data_in_file    = 0;
   m_record.events_in_run   = 0;
   m_record.data_in_run     = 0;
   m_record.status          = 1;
   m_record.end_marker      = 0x1234eeee;
}

void FileStartRecord::dump(std::ostream& os) const {
  os << "[RawFile] FileStartRecord:" << std::endl << std::hex
     << "[RawFile] \tmarker               : 0x" << m_record.marker << std::endl
     << "[RawFile] \trecord_size          : 0x" << m_record.record_size << std::endl
     << "[RawFile] \tversion              : 0x" << m_record.version << std::endl
     << "[RawFile] \tfile_number          : 0x" << m_record.file_number << std::endl
     << "[RawFile] \tdate                 : 0x" << m_record.date << std::endl
     << "[RawFile] \ttime                 : 0x" << m_record.time << std::endl
     << "[RawFile] \tsizeLimit_dataBlocks : 0x" << m_record.sizeLimit_dataBlocks << std::endl
     << "[RawFile] \tsizeLimit_MB         : 0x" << m_record.sizeLimit_MB
     << std::dec << std::endl;
}

void FileNameStrings::dump(std::ostream& os) const {
  os << "[RawFile] FileNameStrings:" << std::endl << std::hex
     << "[RawFile] \tmarker               : 0x" << m_record.marker << std::endl
     << "[RawFile] \tAppName length       : 0x" << m_record.length1 << std::endl
     << "[RawFile] \tAppName              : " << m_appName << std::endl
     << "[RawFile] \tUsrTag  length       : 0x" << m_record.length2 << std::endl
     << "[RawFile] \tUsrTag               : " << m_usrTag
     << std::dec << std::endl;
}

void RunParametersRecord::dump(std::ostream& os) const {
  os << "[RawFile] RunParametersRecord:" << std::endl << std::hex
     << "[RawFile] \tmarker               : 0x" << m_record.marker << std::endl
     << "[RawFile] \trecord_size          : 0x" << m_record.record_size << std::endl
     << "[RawFile] \trun_number           : 0x" << m_record.run_number << std::endl
     << "[RawFile] \tmax_events           : 0x" << m_record.max_events << std::endl
     << "[RawFile] \trec_enable           : 0x" << m_record.rec_enable << std::endl
     << "[RawFile] \ttrigger_type         : 0x" << m_record.trigger_type << std::endl
     << "[RawFile] \tdetector_mask        : 0x" << m_record.detector_mask << std::endl
     << "[RawFile] \tbeam_type            : 0x" << m_record.beam_type << std::endl
     << "[RawFile] \tbeam_energy          : 0x" << m_record.beam_energy
     << std::dec << std::endl;
}

void DataSeparatorRecord::dump(std::ostream& os) const {
  os << "[RawFile] DataSeparatorRecord:" << std::endl << std::hex
     << "[RawFile] \tmarker               : 0x" << m_record.marker << std::endl
     << "[RawFile] \trecord_size          : 0x" << m_record.record_size << std::endl
     << "[RawFile] \tdata_block_number    : 0x" << m_record.data_block_number << std::endl
     << "[RawFile] \tdata_block_size      : 0x" << m_record.data_block_size
     << std::dec << std::endl;
}

void FileEndRecord::dump(std::ostream& os) const {
  os << "[RawFile] FileEndRecord:" << std::endl << std::hex
     << "[RawFile] \tmarker               : 0x" << m_record.marker << std::endl
     << "[RawFile] \trecord_size          : 0x" << m_record.record_size << std::endl
     << "[RawFile] \tdate                 : 0x" << m_record.date << std::endl
     << "[RawFile] \ttime                 : 0x" << m_record.time << std::endl
     << "[RawFile] \tevents_in_file       : 0x" << m_record.events_in_file << std::endl
     << "[RawFile] \tdata_in_file         : 0x" << m_record.data_in_file << std::endl
     << "[RawFile] \tevents_in_run        : 0x" << m_record.events_in_run << std::endl
     << "[RawFile] \tdata_in_run          : 0x" << m_record.data_in_run << std::endl
     << "[RawFile] \tstatus               : 0x" << m_record.status << std::endl
     << "[RawFile] \tend_marker           : 0x" << m_record.end_marker
     << std::dec << std::endl;
}
