#ifndef BES_RAWFILE_UTIL_H
#define BES_RAWFILE_UTIL_H

#include <iostream>
#include <string>
#include <stdint.h>  //uint32_t

class raw_ifstream;
class raw_ofstream;


/*************************************************
 * the following structs and classes are assistant
 * data structure of the RawFileReader/Writer:
 *  ------------------------+---------------------
 *   struct name            | class name
 *  ------------------------+---------------------
 *   file_start_record      | FileStartRecord
 *   file_name_strings      | FileNameStrings
 *   run_parameters_record  | RunParametersRecord
 *   data_separator_record  | DataSeparatorRecord
 *   file_end_record        | FileEndRecord
 ************************************************/

//------------------------------------------------
// Data structure of the file start record
//------------------------------------------------
typedef struct {
  uint32_t marker;  //0x1234aaaa
  uint32_t record_size;  //words
  uint32_t version;
  uint32_t file_number;
  uint32_t date;
  uint32_t time;
  uint32_t sizeLimit_dataBlocks;
  uint32_t sizeLimit_MB;  //MB
} file_start_record;
//------------------------------------------------
class FileStartRecord {
public:
  friend raw_ifstream& operator>>(raw_ifstream& is, FileStartRecord& record);
  friend raw_ofstream& operator<<(raw_ofstream& os, FileStartRecord& record);

  FileStartRecord();

  const file_start_record& getRecord() const { return m_record; }
  void dump(std::ostream& os = std::cout) const;

  void setFileNumber(uint32_t fn) { m_record.file_number = fn; }

private:
  file_start_record m_record;
};

//------------------------------------------------
// Data structure of the file name strings
//------------------------------------------------
typedef struct {
  uint32_t marker;  //0x1234aabb
  uint32_t length1;  //bytes
  uint32_t length2;  //bytes
} file_name_strings;
//------------------------------------------------
class FileNameStrings {
public:
  friend raw_ifstream& operator>>(raw_ifstream&is, FileNameStrings& record);
  friend raw_ofstream& operator<<(raw_ofstream&os, FileNameStrings& record);

  FileNameStrings();

  const file_name_strings& getRecord() const { return m_record; }

  const std::string& gerAppName() const { return m_appName; }
  const std::string& gerUsrTag() const { return m_usrTag; }

  void dump(std::ostream& os = std::cout) const;

private:
  file_name_strings m_record;
  std::string  m_appName;
  std::string  m_usrTag;
};

//------------------------------------------------
// Data structure of the run parameters record
//------------------------------------------------
typedef struct {
  uint32_t marker;  //0x1234bbbb
  uint32_t record_size;  //words
  uint32_t run_number;
  uint32_t max_events;
  uint32_t rec_enable;
  uint32_t trigger_type;
  uint32_t detector_mask;
  uint32_t beam_type;
  uint32_t beam_energy;
} run_parameters_record;
//------------------------------------------------
class RunParametersRecord {
public:
  friend raw_ifstream& operator>>(raw_ifstream&is, RunParametersRecord& record);
  friend raw_ofstream& operator<<(raw_ofstream&os, RunParametersRecord& record);

  RunParametersRecord();

  const run_parameters_record& getRecord() const { return m_record; }
  void dump(std::ostream& os = std::cout) const;

  void setRunNumber(uint32_t runId) { m_record.run_number = runId; }

private:
  run_parameters_record m_record;
};

//------------------------------------------------
// Data structure of the data separator record
//------------------------------------------------
typedef struct {
  uint32_t marker;  //0x1234cccc
  uint32_t record_size;  //words
  uint32_t data_block_number;  //event id in run
  uint32_t data_block_size;  //bytes
} data_separator_record;
//------------------------------------------------
class DataSeparatorRecord {
public:
  friend raw_ifstream& operator>>(raw_ifstream&is, DataSeparatorRecord& record);
  friend raw_ofstream& operator<<(raw_ofstream&os, DataSeparatorRecord& record);

  DataSeparatorRecord();

  const data_separator_record& getRecord() const { return m_record; }
  void dump(std::ostream& os = std::cout) const;

  void setDataBlockNumber(uint32_t dn) { m_record.data_block_number = dn; }
  void setDataBlockSize(uint32_t ds) { m_record.data_block_size = ds; }

private:
  data_separator_record m_record;
};

//------------------------------------------------
// Data structure of the file end record
//------------------------------------------------
typedef struct {
  uint32_t marker;  //0x1234dddd
  uint32_t record_size;  //words
  uint32_t date;
  uint32_t time;
  uint32_t events_in_file;  //events number in file
  uint32_t data_in_file;    //MB file size
  uint32_t events_in_run;   //events number in run
  uint32_t data_in_run;     //MB run size
  uint32_t status;          // !0 ==> the last file of run
  uint32_t end_marker;  //0x1234eeee
} file_end_record;
//------------------------------------------------
class FileEndRecord {
public:
  friend raw_ifstream& operator>>(raw_ifstream&is, FileEndRecord& record);
  friend raw_ifstream& makeEndRecord(DataSeparatorRecord& sep, raw_ifstream& is, FileEndRecord& record);
  friend raw_ofstream& operator<<(raw_ofstream&os, FileEndRecord& record);

  FileEndRecord();

  const file_end_record& getRecord() const { return m_record; }
  void dump(std::ostream& os = std::cout) const;

  void setEventsInFile(uint32_t file_nevt) { m_record.events_in_file = file_nevt; }
  void setDataInFile(uint32_t file_size) { m_record.data_in_file = file_size; }
  void setEventsInRun(uint32_t run_nevt) { m_record.events_in_run = run_nevt; }
  void setDataInRun(uint32_t run_size) { m_record.data_in_run = run_size; }
  void setStatus(uint32_t status) { m_record.status = status; }

private:
  file_end_record m_record;
};
#endif
