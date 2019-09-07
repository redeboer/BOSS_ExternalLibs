#ifndef BES_RAWFILE_EXCEPTIONS_H
#define BES_RAWFILE_EXCEPTIONS_H

#include <string>
#include <stdint.h>

class RawFileException {
   public:
      virtual ~RawFileException() {}

      virtual void print() const;
};

class RawExMessage : public RawFileException {
   public :
      RawExMessage(const char* msg) : m_msg(msg) {}
      virtual ~RawExMessage() {}

      virtual void print() const;

   private :
      std::string m_msg;
};

class InvalidRawFile : public RawFileException {
   public:
      InvalidRawFile(const char* filename) : m_filename(filename) {}
      virtual ~InvalidRawFile() {}

      virtual void print() const;

   private:
      std::string m_filename;
};

class BadInputStream : public RawFileException {
   public:
      BadInputStream(const char* block_name) : m_blockName(block_name) {}
      virtual ~BadInputStream() {}

      virtual void print() const;

   private:
      std::string m_blockName;
};

class WrongMarker : public RawFileException {
   public:
      WrongMarker(uint32_t expect, uint32_t value) : m_expect(expect), m_value(value) {}
      virtual ~WrongMarker() {}

      virtual void print() const;

   private:
      uint32_t m_expect;
      uint32_t m_value;
};

class UnexpectedRecordSize : public RawFileException {
   public:
      UnexpectedRecordSize(const char* block_name, uint32_t expect, uint32_t value)
	 : m_blockName(block_name), m_expect(expect), m_value(value) {
	 }
      virtual ~UnexpectedRecordSize() {}

      virtual void print() const;

   private:
      std::string m_blockName;
      uint32_t m_expect;
      uint32_t m_value;
};

class ReachEndOfFile : public RawFileException {
   public:
      ReachEndOfFile(const char* fileName)
	 : m_fileName(fileName) {
	 }
      virtual ~ReachEndOfFile() {}

      virtual void print() const;
      void print(const char* fname) const;

   private:
      std::string m_fileName;
};

class ReachEndOfFileList : public RawFileException {
   public :
      virtual void print() const;
};

class FailedToWrite : public RawFileException {
   public:
      FailedToWrite(const char* block_name) : m_blockName(block_name) {
      }
      virtual ~FailedToWrite() {}

      virtual void print() const;

   private:
      std::string m_blockName;
};

#endif
