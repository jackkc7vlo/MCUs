/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_DWT     Data Watchpoint and Trace (DWT)
  \brief    Type definitions for the Data Watchpoint and Trace (DWT)
  @{
 */

/**
  \brief  Structure type to access the Data Watchpoint and Trace Register (DWT).
 */
typedef struct
{
    volatile uint32_t       CTRL;      /*!< Offset: 0x000 (R/W)  Control Register */
    volatile uint32_t       CYCCNT;    /*!< Offset: 0x004 (R/W)  Cycle Count Register */
    volatile uint32_t       CPICNT;    /*!< Offset: 0x008 (R/W)  CPI Count Register */
    volatile uint32_t       EXCCNT;    /*!< Offset: 0x00C (R/W)  Exception Overhead Count Register */
    volatile uint32_t       SLEEPCNT;  /*!< Offset: 0x010 (R/W)  Sleep Count Register */
    volatile uint32_t       LSUCNT;    /*!< Offset: 0x014 (R/W)  LSU Count Register */
    volatile uint32_t       FOLDCNT;   /*!< Offset: 0x018 (R/W)  Folded-instruction Count Register */
    volatile const uint32_t PCSR;      /*!< Offset: 0x01C (R/ )  Program Counter Sample Register */
    volatile uint32_t       COMP0;     /*!< Offset: 0x020 (R/W)  Comparator Register 0 */
    volatile uint32_t       MASK0;     /*!< Offset: 0x024 (R/W)  Mask Register 0 */
    volatile uint32_t       FUNCTION0; /*!< Offset: 0x028 (R/W)  Function Register 0 */
    uint32_t                RESERVED0[1U];
    volatile uint32_t       COMP1;     /*!< Offset: 0x030 (R/W)  Comparator Register 1 */
    volatile uint32_t       MASK1;     /*!< Offset: 0x034 (R/W)  Mask Register 1 */
    volatile uint32_t       FUNCTION1; /*!< Offset: 0x038 (R/W)  Function Register 1 */
    uint32_t                RESERVED1[1U];
    volatile uint32_t       COMP2;     /*!< Offset: 0x040 (R/W)  Comparator Register 2 */
    volatile uint32_t       MASK2;     /*!< Offset: 0x044 (R/W)  Mask Register 2 */
    volatile uint32_t       FUNCTION2; /*!< Offset: 0x048 (R/W)  Function Register 2 */
    uint32_t                RESERVED2[1U];
    volatile uint32_t       COMP3;     /*!< Offset: 0x050 (R/W)  Comparator Register 3 */
    volatile uint32_t       MASK3;     /*!< Offset: 0x054 (R/W)  Mask Register 3 */
    volatile uint32_t       FUNCTION3; /*!< Offset: 0x058 (R/W)  Function Register 3 */
} DWT_Type;

/* DWT Control Register Definitions */
#define DWT_CTRL_NUMCOMP_Pos 28U                             /*!< DWT CTRL: NUMCOMP Position */
#define DWT_CTRL_NUMCOMP_Msk (0xFUL << DWT_CTRL_NUMCOMP_Pos) /*!< DWT CTRL: NUMCOMP Mask */

#define DWT_CTRL_NOTRCPKT_Pos 27U                              /*!< DWT CTRL: NOTRCPKT Position */
#define DWT_CTRL_NOTRCPKT_Msk (0x1UL << DWT_CTRL_NOTRCPKT_Pos) /*!< DWT CTRL: NOTRCPKT Mask */

#define DWT_CTRL_NOEXTTRIG_Pos 26U                               /*!< DWT CTRL: NOEXTTRIG Position */
#define DWT_CTRL_NOEXTTRIG_Msk (0x1UL << DWT_CTRL_NOEXTTRIG_Pos) /*!< DWT CTRL: NOEXTTRIG Mask */

#define DWT_CTRL_NOCYCCNT_Pos 25U                              /*!< DWT CTRL: NOCYCCNT Position */
#define DWT_CTRL_NOCYCCNT_Msk (0x1UL << DWT_CTRL_NOCYCCNT_Pos) /*!< DWT CTRL: NOCYCCNT Mask */

#define DWT_CTRL_NOPRFCNT_Pos 24U                              /*!< DWT CTRL: NOPRFCNT Position */
#define DWT_CTRL_NOPRFCNT_Msk (0x1UL << DWT_CTRL_NOPRFCNT_Pos) /*!< DWT CTRL: NOPRFCNT Mask */

#define DWT_CTRL_CYCEVTENA_Pos 22U                               /*!< DWT CTRL: CYCEVTENA Position */
#define DWT_CTRL_CYCEVTENA_Msk (0x1UL << DWT_CTRL_CYCEVTENA_Pos) /*!< DWT CTRL: CYCEVTENA Mask */

#define DWT_CTRL_FOLDEVTENA_Pos 21U                                /*!< DWT CTRL: FOLDEVTENA Position */
#define DWT_CTRL_FOLDEVTENA_Msk (0x1UL << DWT_CTRL_FOLDEVTENA_Pos) /*!< DWT CTRL: FOLDEVTENA Mask */

#define DWT_CTRL_LSUEVTENA_Pos 20U                               /*!< DWT CTRL: LSUEVTENA Position */
#define DWT_CTRL_LSUEVTENA_Msk (0x1UL << DWT_CTRL_LSUEVTENA_Pos) /*!< DWT CTRL: LSUEVTENA Mask */

#define DWT_CTRL_SLEEPEVTENA_Pos 19U                                 /*!< DWT CTRL: SLEEPEVTENA Position */
#define DWT_CTRL_SLEEPEVTENA_Msk (0x1UL << DWT_CTRL_SLEEPEVTENA_Pos) /*!< DWT CTRL: SLEEPEVTENA Mask */

#define DWT_CTRL_EXCEVTENA_Pos 18U                               /*!< DWT CTRL: EXCEVTENA Position */
#define DWT_CTRL_EXCEVTENA_Msk (0x1UL << DWT_CTRL_EXCEVTENA_Pos) /*!< DWT CTRL: EXCEVTENA Mask */

#define DWT_CTRL_CPIEVTENA_Pos 17U                               /*!< DWT CTRL: CPIEVTENA Position */
#define DWT_CTRL_CPIEVTENA_Msk (0x1UL << DWT_CTRL_CPIEVTENA_Pos) /*!< DWT CTRL: CPIEVTENA Mask */

#define DWT_CTRL_EXCTRCENA_Pos 16U                               /*!< DWT CTRL: EXCTRCENA Position */
#define DWT_CTRL_EXCTRCENA_Msk (0x1UL << DWT_CTRL_EXCTRCENA_Pos) /*!< DWT CTRL: EXCTRCENA Mask */

#define DWT_CTRL_PCSAMPLENA_Pos 12U                                /*!< DWT CTRL: PCSAMPLENA Position */
#define DWT_CTRL_PCSAMPLENA_Msk (0x1UL << DWT_CTRL_PCSAMPLENA_Pos) /*!< DWT CTRL: PCSAMPLENA Mask */

#define DWT_CTRL_SYNCTAP_Pos 10U                             /*!< DWT CTRL: SYNCTAP Position */
#define DWT_CTRL_SYNCTAP_Msk (0x3UL << DWT_CTRL_SYNCTAP_Pos) /*!< DWT CTRL: SYNCTAP Mask */

#define DWT_CTRL_CYCTAP_Pos 9U                             /*!< DWT CTRL: CYCTAP Position */
#define DWT_CTRL_CYCTAP_Msk (0x1UL << DWT_CTRL_CYCTAP_Pos) /*!< DWT CTRL: CYCTAP Mask */

#define DWT_CTRL_POSTINIT_Pos 5U                               /*!< DWT CTRL: POSTINIT Position */
#define DWT_CTRL_POSTINIT_Msk (0xFUL << DWT_CTRL_POSTINIT_Pos) /*!< DWT CTRL: POSTINIT Mask */

#define DWT_CTRL_POSTPRESET_Pos 1U                                 /*!< DWT CTRL: POSTPRESET Position */
#define DWT_CTRL_POSTPRESET_Msk (0xFUL << DWT_CTRL_POSTPRESET_Pos) /*!< DWT CTRL: POSTPRESET Mask */

#define DWT_CTRL_CYCCNTENA_Pos 0U                                    /*!< DWT CTRL: CYCCNTENA Position */
#define DWT_CTRL_CYCCNTENA_Msk (0x1UL /*<< DWT_CTRL_CYCCNTENA_Pos*/) /*!< DWT CTRL: CYCCNTENA Mask */

/* DWT CPI Count Register Definitions */
#define DWT_CPICNT_CPICNT_Pos 0U                                    /*!< DWT CPICNT: CPICNT Position */
#define DWT_CPICNT_CPICNT_Msk (0xFFUL /*<< DWT_CPICNT_CPICNT_Pos*/) /*!< DWT CPICNT: CPICNT Mask */

/* DWT Exception Overhead Count Register Definitions */
#define DWT_EXCCNT_EXCCNT_Pos 0U                                    /*!< DWT EXCCNT: EXCCNT Position */
#define DWT_EXCCNT_EXCCNT_Msk (0xFFUL /*<< DWT_EXCCNT_EXCCNT_Pos*/) /*!< DWT EXCCNT: EXCCNT Mask */

/* DWT Sleep Count Register Definitions */
#define DWT_SLEEPCNT_SLEEPCNT_Pos 0U                                        /*!< DWT SLEEPCNT: SLEEPCNT Position */
#define DWT_SLEEPCNT_SLEEPCNT_Msk (0xFFUL /*<< DWT_SLEEPCNT_SLEEPCNT_Pos*/) /*!< DWT SLEEPCNT: SLEEPCNT Mask */

/* DWT LSU Count Register Definitions */
#define DWT_LSUCNT_LSUCNT_Pos 0U                                    /*!< DWT LSUCNT: LSUCNT Position */
#define DWT_LSUCNT_LSUCNT_Msk (0xFFUL /*<< DWT_LSUCNT_LSUCNT_Pos*/) /*!< DWT LSUCNT: LSUCNT Mask */

/* DWT Folded-instruction Count Register Definitions */
#define DWT_FOLDCNT_FOLDCNT_Pos 0U                                      /*!< DWT FOLDCNT: FOLDCNT Position */
#define DWT_FOLDCNT_FOLDCNT_Msk (0xFFUL /*<< DWT_FOLDCNT_FOLDCNT_Pos*/) /*!< DWT FOLDCNT: FOLDCNT Mask */

/* DWT Comparator Mask Register Definitions */
#define DWT_MASK_MASK_Pos 0U                                /*!< DWT MASK: MASK Position */
#define DWT_MASK_MASK_Msk (0x1FUL /*<< DWT_MASK_MASK_Pos*/) /*!< DWT MASK: MASK Mask */

/* DWT Comparator Function Register Definitions */
#define DWT_FUNCTION_MATCHED_Pos 24U                                 /*!< DWT FUNCTION: MATCHED Position */
#define DWT_FUNCTION_MATCHED_Msk (0x1UL << DWT_FUNCTION_MATCHED_Pos) /*!< DWT FUNCTION: MATCHED Mask */

#define DWT_FUNCTION_DATAVADDR1_Pos 16U                                    /*!< DWT FUNCTION: DATAVADDR1 Position */
#define DWT_FUNCTION_DATAVADDR1_Msk (0xFUL << DWT_FUNCTION_DATAVADDR1_Pos) /*!< DWT FUNCTION: DATAVADDR1 Mask */

#define DWT_FUNCTION_DATAVADDR0_Pos 12U                                    /*!< DWT FUNCTION: DATAVADDR0 Position */
#define DWT_FUNCTION_DATAVADDR0_Msk (0xFUL << DWT_FUNCTION_DATAVADDR0_Pos) /*!< DWT FUNCTION: DATAVADDR0 Mask */

#define DWT_FUNCTION_DATAVSIZE_Pos 10U                                   /*!< DWT FUNCTION: DATAVSIZE Position */
#define DWT_FUNCTION_DATAVSIZE_Msk (0x3UL << DWT_FUNCTION_DATAVSIZE_Pos) /*!< DWT FUNCTION: DATAVSIZE Mask */

#define DWT_FUNCTION_LNK1ENA_Pos 9U                                  /*!< DWT FUNCTION: LNK1ENA Position */
#define DWT_FUNCTION_LNK1ENA_Msk (0x1UL << DWT_FUNCTION_LNK1ENA_Pos) /*!< DWT FUNCTION: LNK1ENA Mask */

#define DWT_FUNCTION_DATAVMATCH_Pos 8U                                     /*!< DWT FUNCTION: DATAVMATCH Position */
#define DWT_FUNCTION_DATAVMATCH_Msk (0x1UL << DWT_FUNCTION_DATAVMATCH_Pos) /*!< DWT FUNCTION: DATAVMATCH Mask */

#define DWT_FUNCTION_CYCMATCH_Pos 7U                                   /*!< DWT FUNCTION: CYCMATCH Position */
#define DWT_FUNCTION_CYCMATCH_Msk (0x1UL << DWT_FUNCTION_CYCMATCH_Pos) /*!< DWT FUNCTION: CYCMATCH Mask */

#define DWT_FUNCTION_EMITRANGE_Pos 5U                                    /*!< DWT FUNCTION: EMITRANGE Position */
#define DWT_FUNCTION_EMITRANGE_Msk (0x1UL << DWT_FUNCTION_EMITRANGE_Pos) /*!< DWT FUNCTION: EMITRANGE Mask */

#define DWT_FUNCTION_FUNCTION_Pos 0U                                       /*!< DWT FUNCTION: FUNCTION Position */
#define DWT_FUNCTION_FUNCTION_Msk (0xFUL /*<< DWT_FUNCTION_FUNCTION_Pos*/) /*!< DWT FUNCTION: FUNCTION Mask */

/*@}*/ /* end of group CMSIS_DWT */