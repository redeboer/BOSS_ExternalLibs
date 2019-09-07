#ifndef EVENT_TRIG_MDC_H
#define EVENT_TRIG_MDC_H

#include <stdint.h>
#include <string>
#include <vector>
#include <map>

class TrigMdc {
public:
  TrigMdc(const char* name);
  virtual ~TrigMdc();

  void setMdcTrigData(uint32_t boardId, uint32_t window, uint32_t datasize, const uint32_t* data);

  void initialize();

  inline const std::string& getName() const { return m_name; }
  // hit
  inline int getMdcHitOne(int layer, int wire) const { return m_mdchit_one[layer][wire]; }
  inline int getMdcHitSOne(int layer, int wire) const { 
    if(m_mdchit_sone[layer][wire] != 0) {
      return (m_mdchit_sone[layer][wire] + 1); 
    }
    else return 0;
  }
  inline int getMdcHitPre(int layer, int wire) const { return m_mdchit_pre[layer][wire]; } // first up in windows
  inline int getMdcHitAft(int layer, int wire) const { return m_mdchit_aft[layer][wire]; } // first down in windows after first up
  inline uint32_t getMdcHitWin(int layer, int wire) const { return m_mdchit_win[layer][wire]; } // hit distributions in windows

  // tsf
  inline int getMdcTSFOne(int layer, int wire) const { return m_mdctsf_one[layer][wire]; }
  inline int getMdcTSFSOne(int layer, int wire) const { 
    if(m_mdctsf_sone[layer][wire] != 0) {
      return (m_mdctsf_sone[layer][wire] + 1);
    }
    else return 0;
  }
  inline int getMdcTSFPre(int layer, int wire) const { return m_mdctsf_pre[layer][wire]; } // first up in windows
  inline int getMdcTSFAft(int layer, int wire) const { return m_mdctsf_aft[layer][wire]; } // first down in windows after first up
  inline uint32_t getMdcTSFWin(int layer, int wire) const { return m_mdctsf_win[layer][wire]; } // hit distributions in windows

  // ltrk
  inline int getMdcLtrkOne(int id) const { return m_mdcltrk_one[id]; }
  inline int getMdcLtrkSOne(int id) const { 
    if(m_mdcltrk_sone[id] != 0) {
      return (m_mdcltrk_sone[id] + 1);
    }
    else return 0;
  }
  inline int getMdcLtrkPre(int id) const { return m_mdcltrk_pre[id]; } // first up in windows
  inline int getMdcLtrkAft(int id) const { return m_mdcltrk_aft[id]; } // first down in windows after first up
  inline uint32_t getMdcLtrkWin(int id) const { return m_mdcltrk_win[id]; } // hit distributions in windows

  // strk
  inline int getMdcStrkOne(int id) const { return m_mdcstrk_one[id]; }
  inline int getMdcStrkSOne(int id) const { 
    if(m_mdcstrk_sone[id] != 0) {
      return (m_mdcstrk_sone[id] + 1);
    }
    else return 0;
  }
  inline int getMdcStrkPre(int id) const { return m_mdcstrk_pre[id]; } // first up in windows
  inline int getMdcStrkAft(int id) const { return m_mdcstrk_aft[id]; } // first down in windows after first up
  inline uint32_t getMdcStrkWin(int id) const { return m_mdcstrk_win[id]; } // hit distributions in windows
  
  // ltrk in couter
  inline int getMdcLtrkCoutOne(int id) const { return m_mdcltrkc_one[id]; }
  inline int getMdcLtrkCoutSOne(int id) const { 
    if(m_mdcltrkc_sone[id] != 0) {
      return (m_mdcltrkc_sone[id] + 1);
    }
    else return 0;
  }

  // strk in counter
  inline int getMdcStrkCoutOne(int id) const { return m_mdcstrkc_one[id]; }
  inline int getMdcStrkCoutSOne(int id) const { 
    if(m_mdcstrkc_sone[id] != 0) {
      return (m_mdcstrkc_sone[id] + 1);
    }
    else return 0;
  }

  inline int getMdcTrigCondi(int id) const { return m_mdcTrigCondi[id]; }



  void print();

private:
  std::string m_name;
  std::string filename;
  std::map<uint32_t, std::vector<uint32_t> > m_datamap;

  int **m_mdchit_one;
  int **m_mdchit_sone;
  int **m_mdchit_pre;
  int **m_mdchit_aft;
  uint32_t **m_mdchit_win;

  int **m_mdctsf_one;
  int **m_mdctsf_sone;
  int **m_mdctsf_pre;
  int **m_mdctsf_aft;
  uint32_t **m_mdctsf_win;
  
  int *m_mdcltrk_one;
  int *m_mdcltrk_sone;
  int *m_mdcltrk_pre;
  int *m_mdcltrk_aft;
  uint32_t *m_mdcltrk_win;

  int* m_mdcstrk_one;
  int* m_mdcstrk_sone;
  int *m_mdcstrk_pre;
  int *m_mdcstrk_aft;
  uint32_t *m_mdcstrk_win;

  int* m_mdcltrkc_one;
  int* m_mdcltrkc_sone;

  int* m_mdcstrkc_one;
  int* m_mdcstrkc_sone;

  int* m_mdcTrigCondi;

/*
  int m_mdchit_one[24][256];
  int m_mdctsf_one[6][256];
  int m_mdcltrk_one[128];
  int m_mdcstrk_one[128];
  int m_mdcltrkc_one[132];
  int m_mdcstrkc_one[132];

  int m_mdchit_sone[24][256];
  int m_mdctsf_sone[6][256];
  int m_mdcltrk_sone[128];
  int m_mdcstrk_sone[128];
  int m_mdcltrkc_sone[132];
  int m_mdcstrkc_sone[132];
*/
};

#endif  // EVENT_TRIG_MDC_H
