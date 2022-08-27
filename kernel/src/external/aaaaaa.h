typedef struct SATA_ident
{
      unsigned short   config;      /* lots of obsolete bit flags */
      unsigned short   cyls;      /* obsolete */
      unsigned short   reserved2;   /* special config */
      unsigned short   heads;      /* "physical" heads */
      unsigned short   track_bytes;   /* unformatted bytes per track */
      unsigned short   sector_bytes;   /* unformatted bytes per sector */
      unsigned short   sectors;   /* "physical" sectors per track */
      unsigned short   vendor0;   /* vendor unique */
      unsigned short   vendor1;   /* vendor unique */
      unsigned short   vendor2;   /* vendor unique */
      unsigned char   serial_no[20];   /* 0 = not_specified */
      unsigned short   buf_type;
      unsigned short   buf_size;   /* 512 byte increments; 0 = not_specified */
      unsigned short   ecc_bytes;   /* for r/w long cmds; 0 = not_specified */
      unsigned char   fw_rev[8];   /* 0 = not_specified */
      unsigned char   model[40];   /* 0 = not_specified */
      unsigned short  multi_count; /* Multiple Count */
      unsigned short   dword_io;   /* 0=not_implemented; 1=implemented */
      unsigned short   capability1;   /* vendor unique */
      unsigned short   capability2;   /* bits 0:DMA 1:LBA 2:IORDYsw 3:IORDYsup word: 50 */
      unsigned char   vendor5;   /* vendor unique */
      unsigned char   tPIO;      /* 0=slow, 1=medium, 2=fast */
      unsigned char   vendor6;   /* vendor unique */
      unsigned char   tDMA;      /* 0=slow, 1=medium, 2=fast */
      unsigned short   field_valid;   /* bits 0:cur_ok 1:eide_ok */
      unsigned short   cur_cyls;   /* logical cylinders */
      unsigned short   cur_heads;   /* logical heads word 55*/
      unsigned short   cur_sectors;   /* logical sectors per track */
      unsigned short   cur_capacity0;   /* logical total sectors on drive */
      unsigned short   cur_capacity1;   /*  (2 words, misaligned int)     */
      unsigned char   multsect;   /* current multiple sector count */
      unsigned char   multsect_valid;   /* when (bit0==1) multsect is ok */
      unsigned int   lba_capacity;   /* total number of sectors */
      unsigned short   dma_1word;   /* single-word dma info */
      unsigned short   dma_mword;   /* multiple-word dma info */
      unsigned short  eide_pio_modes; /* bits 0:mode3 1:mode4 */
      unsigned short  eide_dma_min;   /* min mword dma cycle time (ns) */
      unsigned short  eide_dma_time;   /* recommended mword dma cycle time (ns) */
      unsigned short  eide_pio;       /* min cycle time (ns), no IORDY  */
      unsigned short  eide_pio_iordy; /* min cycle time (ns), with IORDY */
      unsigned short   words69_70[2];   /* reserved words 69-70 */
      unsigned short   words71_74[4];   /* reserved words 71-74 */
      unsigned short  queue_depth;   /*  */
      unsigned short  sata_capability;   /*  SATA Capabilities word 76*/
      unsigned short  sata_additional;   /*  Additional Capabilities */
      unsigned short  sata_supported;   /* SATA Features supported  */
      unsigned short  features_enabled;   /* SATA features enabled */
      unsigned short  major_rev_num;   /*  Major rev number word 80 */
      unsigned short  minor_rev_num;   /*  */
      unsigned short  command_set_1;   /* bits 0:Smart 1:Security 2:Removable 3:PM */
      unsigned short  command_set_2;   /* bits 14:Smart Enabled 13:0 zero */
      unsigned short  cfsse;      /* command set-feature supported extensions */
      unsigned short  cfs_enable_1;   /* command set-feature enabled */
      unsigned short  cfs_enable_2;   /* command set-feature enabled */
      unsigned short  csf_default;   /* command set-feature default */
      unsigned short  dma_ultra;   /*  */
      unsigned short   word89;      /* reserved (word 89) */
      unsigned short   word90;      /* reserved (word 90) */
      unsigned short   CurAPMvalues;   /* current APM values */
      unsigned short   word92;         /* reserved (word 92) */
      unsigned short   comreset;      /* should be cleared to 0 */
      unsigned short  accoustic;      /*  accoustic management */
      unsigned short  min_req_sz;      /* Stream minimum required size */
      unsigned short  transfer_time_dma;   /* Streaming Transfer Time-DMA */
      unsigned short  access_latency;      /* Streaming access latency-DMA & PIO WORD 97*/
      unsigned int    perf_granularity;   /* Streaming performance granularity */
      unsigned int   total_usr_sectors[2]; /* Total number of user addressable sectors */
      unsigned short   transfer_time_pio;    /* Streaming Transfer time PIO */
      unsigned short   reserved105;       /* Word 105 */
      unsigned short   sector_sz;          /* Puysical Sector size / Logical sector size */
      unsigned short   inter_seek_delay;   /* In microseconds */
      unsigned short   words108_116[9];    /*  */
      unsigned int   words_per_sector;    /* words per logical sectors */
      unsigned short   supported_settings; /* continued from words 82-84 */
      unsigned short   command_set_3;       /* continued from words 85-87 */
      unsigned short  words121_126[6];   /* reserved words 121-126 */
      unsigned short   word127;         /* reserved (word 127) */
      unsigned short   security_status;   /* device lock function
                   * 15:9   reserved
                   * 8   security level 1:max 0:high
                   * 7:6   reserved
                   * 5   enhanced erase
                   * 4   expire
                   * 3   frozen
                   * 2   locked
                   * 1   en/disabled
                   * 0   capability
                   */
      unsigned short  csfo;      /* current set features options
                   * 15:4   reserved
                   * 3   auto reassign
                   * 2   reverting
                   * 1   read-look-ahead
                   * 0   write cache
                   */
      unsigned short   words130_155[26];/* reserved vendor words 130-155 */
      unsigned short   word156;
      unsigned short   words157_159[3];/* reserved vendor words 157-159 */
      unsigned short   cfa; /* CFA Power mode 1 */
      unsigned short   words161_175[15]; /* Reserved */
      unsigned char   media_serial[60]; /* words 176-205 Current Media serial number */
      unsigned short   sct_cmd_transport; /* SCT Command Transport */
      unsigned short   words207_208[2]; /* reserved */
      unsigned short   block_align; /* Alignement of logical blocks in larger physical blocks */
      unsigned int   WRV_sec_count; /* Write-Read-Verify sector count mode 3 only */
      unsigned int      verf_sec_count; /* Verify Sector count mode 2 only */
      unsigned short   nv_cache_capability; /* NV Cache capabilities */
      unsigned short   nv_cache_sz; /* NV Cache size in logical blocks */
      unsigned short   nv_cache_sz2; /* NV Cache size in logical blocks */
      unsigned short   rotation_rate; /* Nominal media rotation rate */
      unsigned short   reserved218; /*  */
      unsigned short   nv_cache_options; /* NV Cache options */
      unsigned short   words220_221[2]; /* reserved */
      unsigned short   transport_major_rev; /*  */
      unsigned short   transport_minor_rev; /*  */
      unsigned short   words224_233[10]; /* Reserved */
      unsigned short   min_dwnload_blocks; /* Minimum number of 512byte units per DOWNLOAD MICROCODE
                                  command for mode 03h */
      unsigned short   max_dwnload_blocks; /* Maximum number of 512byte units per DOWNLOAD MICROCODE
                                  command for mode 03h */
      unsigned short   words236_254[19];   /* Reserved */
      unsigned short   integrity;          /* Cheksum, Signature */
} SATA_ident_t;