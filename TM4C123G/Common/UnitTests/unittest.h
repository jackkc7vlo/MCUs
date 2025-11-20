/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       unittest.h
 * @author     Jack Wilson
 * @brief
 *
 * @note
 * @history:
 *   Version   Date            Author          Modification    Email
 *   V1.0.0    Nov-14-2023     Jack Wilson                     jackkc7vlo@gmail.com
 *
 * @verbatim
 * ==============================================================================
 * This code is based on the uCUnit framework by Sven Stefan Krauss, but
 * in non-macro form and utilizing RTT for printing.
 *  uCUnit - A unit testing framework for microcontrollers                   *
 *                                                                           *
 *  (C) 2007 - 2008 Sven Stefan Krauss                                       *
 *                  https://www.ucunit.org                                   *
 *                                                                           *
 *  Description : Macros for Unit-Testing                                    *
 *  Author      : Sven Stefan Krauss                                         *
 *  Contact     : www.ucunit.org
 * ==============================================================================
 * @endverbatim
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************/

/********************************************************************************
 * Multiple include protection
 ********************************************************************************/
#ifndef unittest_h_
#define unittest_h_

/********************************************************************************
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdint.h>
#include <hw_config.h>

#if UNIT_TESTS == 1u
    /********************************************************************************
     * Defines
     ********************************************************************************/
/**
 * Verbose Mode.
 * UCUNIT_MODE_SILENT: Checks are performed silently.
 * UCUNIT_MODE_NORMAL: Only checks that fail are displayed
 * UCUNIT_MODE_VERBOSE: Passed and failed checks are displayed
 */
// #define UCUNIT_MODE_NORMAL
#define UCUNIT_MODE_VERBOSE

/**
 * Max. number of checkpoints. This may depend on your application
 * or limited by your RAM.
 */
#define UCUNIT_MAX_TRACEPOINTS 16
#define UCUNIT_ACTION_WARNING 0   /* Goes through the checks \
                                     with message depending on level */
#define UCUNIT_ACTION_SHUTDOWN 1  /* Stops on the end of the checklist \
                                     if any check has failed */
#define UCUNIT_ACTION_SAFESTATE 2 /* Goes in safe state if check fails */

/**
 * Color codes for printing
 */
#if USE_SEGGER == 1
#define RED "\x1B[2;31m"
#define GREEN "\x1B[2;32m"
#define WHITE "\x1B[2;37m"
#else
#define RED "\x1B[91m"
#define GREEN "\x1B[92m"
#define WHITE "\x1B[97m"
#endif
#define YELLOW "\x1B[2;33m"
#define BLUE "\x1B[2;34m"
#define MAGENTA "\x1B[2;35m"
#define CYAN "\x1B[2;36m"

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    /*lint -e843*/
    static int32_t unittest_checks_failed = 0; /* Number of failed checks */
    static int32_t unittest_checks_passed = 0; /* Number of passed checks */

    static int32_t unittest_testcases_failed = 0;                 /* Number of failed test cases */
    static int32_t unittest_testcases_passed = 0;                 /* Number of passed test cases */
    static int32_t unittest_testcases_failed_checks = 0;          /* Number of failed checks in a testcase */
    static int32_t unittest_checklist_failed_checks = 0;          /* Number of failed checks in a checklist */
    static const int32_t unittest_action = UCUNIT_ACTION_WARNING; /* Action to take if a check fails */
    static int32_t unittest_checkpoints[UCUNIT_MAX_TRACEPOINTS];  /* Max. number of tracepoints */
    static int32_t unittest_index = 0;                            /* Tracepoint index */
#pragma GCC diagnostic pop
    /********************************************************************************
     * Function Prototypes
     ********************************************************************************/

    void unittest_init(void);
    void unittest_start_test(const char *name);
    void unittest_end_test(void);
    void unittest_start_testcase(const char *name);
    void unittest_end_testcase(void);

    uint32_t unittest_check_equal(const int32_t expected, const int32_t actual);
    uint32_t unittest_check_not_equal(const int32_t expected, const int32_t actual);
    uint32_t unittest_check_float_equal(const float expected, const float actual);
    uint32_t unittest_check_float_not_equal(const float expected, const float actual);
    uint32_t unittest_check_range(const int32_t actual, const int32_t min, const int32_t max, bool hex);
    uint32_t unittest_check_time_range(const int32_t min, const int32_t max, bool hex);
    uint32_t unittest_check_range_float(const float min, const float max, const float actual);
    uint32_t unittest_check_true(const int32_t condition);
    uint32_t unittest_check_false(const int32_t condition);
    uint32_t unittest_check_null(const void *pointer);
    uint32_t unittest_check_not_null(const void *pointer);
    uint32_t unittest_check_str_equal(const char *expected, const char *actual);
    uint32_t unittest_check_str_not_equal(const char *expected, const char *actual);
    uint32_t unittest_check_str_contains(const char *expected, const char *actual);
    uint32_t unittest_check_str_not_contains(const char *expected, const char *actual);

    int unittest_printf(const char *sFormat, ...);
    void unittest_summary(void);
    void unittest_shutdown(void);
    uint32_t unittest_elapsed_time(void);
#endif /* UNIT_TESTS */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* unittest_h_ */
