#ifndef DIST_BOSS_CODE_H
#define DIST_BOSS_CODE_H

class DistBossCode {

   public :

      enum ClientCode {
	 GetEvent       = 0x80000001,
	 GetFileName    = 0x80000002,
	 StatusFinalize = 0x80000003,
	 ClientReady    = 0x80000004,
	 RetryEvent     = 0x80000005,
	 ClientError    = 0x80000100
      };

      enum ServerCode {
	 NoMoreEvents   = 0x40000001,
	 StatusSuccess  = 0x40000002,
	 ServerFinalize = 0x40000003,
	 ServerError    = 0x40000100,
	 ServerTimeout  = 0x40000101
      };
};

#endif
