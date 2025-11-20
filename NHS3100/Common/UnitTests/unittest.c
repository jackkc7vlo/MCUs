/**
 * ****************************(C) COPYRIGHT 2023 Old Man Software *****************
 * @file       unittest.c
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
 *
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
 * Includes
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdint.h>
#include <stdbool.h>
#include "unittest.h"
#include "System.h"
#include <stdio.h>
#include <stdlib.h> /*lint -e129*/
#include <stdarg.h>
#include "uc_elapsed_time.h"
#if USE_PRINTF == 1u
#if USE_SEGGER == 1u
#include <SEGGER_RTT.h>
#endif
#endif
#include <clock_hal.h>
#include <chip_hal.h>
#include <hw_config.h>

#if UNIT_TESTS == 1u

/********************************************************************************
 * Defines
 ********************************************************************************/
#define unittest_define_to_string(x) #x

    /********************************************************************************
     * Typedefs & Enums
     ********************************************************************************/
    static char *testcase_name;

    char ibuffer[sizeof(int32_t) * 8 + 1];
    char fbuffer[sizeof(float) * 8 + 1];
    /********************************************************************************
     * Functions
     ********************************************************************************/

    void unittest_init(void)
    {

        // UCUNIT_WriteString(WHITE);
        System_Init();
        clock_hal_init();
        chip_hal_initialize_IO();
        uc_elapsed_time_init();
    }

    void unittest_start_test(const char *name)
    {
        unittest_printf("  * %s *\n", name);
    }
    void unittest_end_test(void)
    {
    }
    void unittest_start_testcase(const char *name)
    {
        // unittest_printf("\n===\n%s\n===\n", name);
        testcase_name = (char *)name;
        unittest_testcases_failed_checks = unittest_checks_failed;
        uc_elapsed_time_start();
    }

    void unittest_write_pass_message(const char *msg)
    {
        unittest_printf("%s: %s %s: passed %s", testcase_name, GREEN, msg, WHITE);
        unittest_printf(" Time: %ld us\n", uc_elapsed_time_value());
    }

    void unittest_write_failed_message(const char *msg)
    {
        unittest_printf("%s: %s %s: failed %s\n", testcase_name, RED, msg, WHITE);
    }

    static void unittest_check(bool condition, const char *msg)
    {

        if ((condition))
        {
            unittest_write_pass_message(msg);
            unittest_checks_passed++;
        }
        else
        {
            unittest_write_failed_message(msg);
            unittest_checks_failed++;
        }
    }
    void unittest_end_testcase(void)
    {
        uc_elapsed_time_stop();

        unittest_printf("\n===\n");

        if (0 == (unittest_testcases_failed_checks - unittest_checks_failed))
        {
            unittest_printf("passed.\n");
            unittest_testcases_passed++;
        }
        else
        {
            unittest_write_failed_message("End Test\n");
            unittest_testcases_failed++;
        }

        unittest_printf("===\n");
    }

    static void unittest_compare(bool porf, const char *msg, const char *expected, const char *actual)
    {
        unittest_check(porf, msg);
        if (!porf)
        {
            unittest_printf("Expected: %s\n", expected);
            unittest_printf("Actual: %s\n", actual);
        }
    }

    uint32_t unittest_check_equal(const int32_t expected, const int32_t actual)
    {
        uc_elapsed_time_stop();
        unittest_compare(expected == actual, "Is Equal Check", itoa(expected, ibuffer, 16), itoa(actual, ibuffer, 16));
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_not_equal(const int32_t expected, const int32_t actual)
    {
        uc_elapsed_time_stop();
        unittest_compare(expected != actual, "Is !Equal Check", itoa(expected, ibuffer, 16), itoa(actual, ibuffer, 16));
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_float_equal(const float expected, const float actual)
    {
        uc_elapsed_time_stop();
        // unittest_compare(expected == actual, "Is Equal Check", utoa(expected, fbuffer, 16), utoa(actual, fbuffer, 16));
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_float_not_equal(const float expected, const float actual)
    {
        uc_elapsed_time_stop();
        // unittest_compare(expected != actual, "Is !Equal Check", utoa(expected, fbuffer, 16), utoa(actual, fbuffer, 16));
        return uc_elapsed_time_value();
    }

    static void unittest_check_print(const bool porf, const int32_t actual, const int32_t min, const int32_t max, bool hex)
    {
        if (!porf)
        {
            if (!hex)
            {
                unittest_printf("Min:    %ld\n", min);
                unittest_printf("Max:    %ld\n", max);
                unittest_printf("Actual: %ld\n", actual);
            }
            else
            {
                unittest_printf("Min:    %08x\n", min);
                unittest_printf("Max:    %08x\n", min);
                unittest_printf("Actual: %08x\n", actual);
            }
        }
    }

    uint32_t unittest_check_range(const int32_t actual, const int32_t min, const int32_t max, bool hex)
    {
        uc_elapsed_time_stop();
        bool porf = ((actual >= min) && (actual <= max));
        unittest_check(porf, "Range Check");
        unittest_check_print(porf, actual, min, max, hex);
        return uc_elapsed_time_value();
    }

    uint32_t unittest_check_time_range(const int32_t min, const int32_t max, bool hex)
    {
        uc_elapsed_time_stop();
        bool porf = ((uc_elapsed_time_value() >= (uint32_t)min) && (uc_elapsed_time_value() <= (uint32_t)max));
        unittest_check(porf, "Time Range Check");
        unittest_check_print(porf, uc_elapsed_time_value(), min, max, hex);
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_range_float(const float min, const float max, const float actual)
    {
        unittest_printf("check not implemented\n");
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_true(const int32_t condition)
    {
        uc_elapsed_time_stop();
        unittest_check((condition == true), "Is True Check");
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_false(const int32_t condition)
    {
        uc_elapsed_time_stop();
        unittest_check((condition != true), "Is False Check");
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_null(const void *pointer)
    {
        uc_elapsed_time_stop();
        unittest_check(pointer == NULL, "Is Null Check");
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_not_null(const void *pointer)
    {
        uc_elapsed_time_stop();
        unittest_check(pointer != NULL, "Is !Null Check");
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_str_equal(const char *expected, const char *actual)
    {
        unittest_printf("check not implemented\n");
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_str_not_equal(const char *expected, const char *actual)
    {
        unittest_printf("check not implemented\n");
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_str_contains(const char *expected, const char *actual)
    {
        unittest_printf("check not implemented\n");
        return uc_elapsed_time_value();
    }
    uint32_t unittest_check_str_not_contains(const char *expected, const char *actual)
    {
        unittest_printf("check not implemented\n");
        return uc_elapsed_time_value();
    }

    uint32_t unittest_elapsed_time()
    {
        unittest_printf("check not implemented\n");
        return uc_elapsed_time_value();
    }
    void unittest_summary(void)
    {
        unittest_printf("\n***\nCases: failed: %ld\n       passed: %ld\nChecks: failed: %ld\n        passed: %ld\n***\n",
                        unittest_testcases_failed,
                        unittest_testcases_passed,
                        unittest_checks_failed,
                        unittest_checks_passed);
    }
    void unittest_shutdown(void)
    {
    }

    int unittest_printf(const char *sFormat, ...)
    {
        int r = 0;

        va_list ParamList;

        va_start(ParamList, sFormat);
#if USE_PRINTF == 1u
#if USE_SEGGER == 1u
        r = SEGGER_RTT_vprintf(0, sFormat, &ParamList);
#else
        r = vprintf(sFormat, ParamList);
#endif
#endif
        va_end(ParamList);
        return r;
    }
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
