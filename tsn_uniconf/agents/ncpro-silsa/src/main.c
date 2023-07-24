/*
 * Copyright (c) 2023 Texas Instruments Incorporated
 * Copyright (c) 2023 Excelfore Corporation (https://excelfore.com)
 *
 * All rights reserved not granted herein.
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free,
 * non-exclusive license under copyrights and patents it now or hereafter
 * owns or controls to make, have made, use, import, offer to sell and sell ("Utilize")
 * this software subject to the terms herein. With respect to the foregoing patent
 * license, such license is granted solely to the extent that any such patent is necessary
 * to Utilize the software alone. The patent license shall not apply to any combinations which
 * include this software, other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license (including the
 * above copyright notice and the disclaimer and (if applicable) source code license limitations below)
 * in the documentation and/or other materials provided with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided that the following
 * conditions are met:
 *
 * * No reverse engineering, decompilation, or disassembly of this software is permitted with respect to any
 * software provided in binary form.
 * * any redistribution and use are licensed by TI for use only with TI Devices.
 * * Nothing shall obligate TI to provide you with source code for the software licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the source code are permitted
 * provided that the following conditions are met:
 *
 * * any redistribution and use of the source code, including any resulting derivative works, are licensed by
 * TI for use only with TI Devices.
 * * any redistribution and use of any object code compiled from the source code and any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers may be used to endorse or
 * promote products derived from this software without specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI"S LICENSORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI"S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/
/**
 * @file        main.c
 * @author      Allan Co<allanrodrigo.co@excelfore.com>
 * @copyright   Copyright (C) 2023 Excelfore Corporation
 *
 * @brief       Excelfore UNICONF and Yumaworks SIL-SA+DB-API Integration
 *              app main.
 */

/*=============================================================================
 * Include Files
 *============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>

/* YumaPro SDK includes */
#include <yumapro/platform/procdefs.h>
#include <yumapro/agt/agt_sil_lib.h>
#include <yumapro/agt/agt_cb.h>
#include <yumapro/db-api/db_api.h>
#include <yumapro/ncx/log.h>
#include <yumapro/sil-sa/sil_sa.h>
#include <yumapro/ncx/status_enum.h>
#include <yumapro/ycontrol/ycontrol.h>

/* libxml2 includes */
#include <libxml/tree.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

/* Local includes */
#include "ucclient.h"

/*=============================================================================
 * Macros and Constants
 *============================================================================*/

#define XL4_SIL_SA_BUNDLE           x4ucsilsa
#define XL4_SIL_SA_BUNDLE_STR       "x4ucsilsa"
#define XL4_SIL_SA_INIT_CB_DEF(N)   y_##N##_init, y_##N##_init2, y_##N##_cleanup
#define XL4_SIL_SA_INIT_CB_PRM(N)   XL4_SIL_SA_INIT_CB_DEF(N)
#define XL4_SIL_SA_EXTERN(N)        AGT_SIL_LIB_EXTERN(N)

/*=============================================================================
 * Typedefs, Structures, and Enums
 *============================================================================*/

typedef struct uniconfopt{
    char *subsys;
    bool withdblock;
    ucclientopt_t ucopt;
    bool ishelp;
} uniconfopt_t;

/*=============================================================================
 * Function Prototypes
 *============================================================================*/

static int ypservices_init(uniconfopt_t *optd, int argc, char *argv[]);
static status_t static_silsa_init (void);
static void ypservices_cleanup(void);
static int set_options(uniconfopt_t *optd, int argc, char *argv[]);
static void print_usage(char *pname, uniconfopt_t *optd);
static void init_sighdlr(struct sigaction *act);
static void sighandler(int signo);
static void main_loop(void *args);

/* extern definitions for the 3 expected callbacks */
XL4_SIL_SA_EXTERN(XL4_SIL_SA_BUNDLE)

/*=============================================================================
 * Global Variables
 *============================================================================*/

volatile sig_atomic_t app_stop_flag = 0;

/*=============================================================================
 * Function Definitions
 *============================================================================*/

static void send_config_edits_to_server (const xmlChar *path_str, xmlNodePtr configNode)
{
    const xmlChar *operation_str = (const xmlChar *)"create";
    const xmlChar *patch_id_str = NULL;
    const xmlChar *insert_point = NULL;
    const xmlChar *insert_where = NULL;
    boolean system_edit = true;
    status_t res;

    xmlBufferPtr xmlBuffer=NULL;
    int size_ret;

    /* create xmlBuffer for storing the ptp config val */
    xmlBuffer = xmlBufferCreate();
    if (NULL==xmlBuffer) {
        log_error("\nERROR: XML Buffer Creation Failed");
        goto erexit;
    }

    size_ret =xmlNodeDump(xmlBuffer, NULL, configNode, 0, 1);
    if (-1 == size_ret) {
        log_error("\nERROR: Failed to Dump Node to Buffer");
        goto erexit;
    }

    res = db_api_send_edit_full2(path_str,
                                 operation_str,
                                 xmlBuffer->content,
                                 patch_id_str,
                                 system_edit,
                                 insert_point,
                                 insert_where,
                                 true);
    if (res != NO_ERR) {
        log_error("\nERROR: Send test edit failed %s %s =\n%s\n(MSG: %s)",
                   operation_str, path_str, xmlBuffer->content,
                   get_error_string(res));
    } else if (LOGDEBUG) {
        log_debug("\nSend test edit OK  %s %s = %s\n",
                  operation_str, path_str, xmlBuffer->content);
    }

erexit:
    if (xmlBuffer) {
        xmlBufferFree(xmlBuffer);
    }
}

static status_t static_silsa_init (void)
{
    status_t res =
        agt_sil_lib_register_statlib((const xmlChar *)XL4_SIL_SA_BUNDLE_STR,
                                     XL4_SIL_SA_INIT_CB_PRM(XL4_SIL_SA_BUNDLE));

    return res;
}

static int ypservices_init(uniconfopt_t *optd, int argc, char *argv[])
{
    status_t res;
    int ret=0;

    /* 1) setup yumapro messaging service profile */
    res = ycontrol_init(argc, argv, (const xmlChar *)optd->subsys);
    if (NO_ERR != res) {
        log_error("\nycontrol_init() failed [res=%u]", (uint32_t)res);
        ret = -1;
    }

    /* 2) set modules/bundles that this subagent handles */
    if (0 == ret) {
        res = sil_sa_add_library_parm((const xmlChar *)XL4_SIL_SA_BUNDLE_STR);
        if (NO_ERR != res) {
            log_error("\nsil_sa_add_library_parm() failed [res=%u]", (uint32_t)res);
            ret = -1;
        }
    }

    /* 3) register to the sil_sa services */
    if (0 == ret) {
        res = sil_sa_register_service();
        if (NO_ERR != res) {
            log_error("\nsil_sa_register_service() failed [res=%u]", (uint32_t)res);
            ret = -1;
        }
    }

    /* 4) register to the db_api services */
    if (0 == ret) {
        res = db_api_register_service_ex(optd->withdblock);
        if (NO_ERR != res) {
            log_error("\ndb_api_register_service_ex() failed [res=%u]", (uint32_t)res);
            ret = -1;
        }
    }

    /* 5)  Register the static SIL-SA library */
    if (0 == ret) {
        res = static_silsa_init();
        if (NO_ERR != res) {
            log_error("\nstatic_silsa_init() failed [res=%u]", (uint32_t)res);
            ret = -1;
        }
    }

    /* 6) do 2nd stage init of the control manager (connect to server) */
    if (0 == ret) {
        res = ycontrol_init2();
        if (NO_ERR != res) {
            log_error("\nycontrol_init2() failed [res=%u]", (uint32_t)res);
            ret = -1;
        }
    }

    return ret;
}

static void ypservices_cleanup(void)
{
    ycontrol_cleanup();
}


static int set_options(uniconfopt_t *optd, int argc, char *argv[])
{
    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"subsys", required_argument, 0, 's'},
        {"dbname", required_argument, 0, 'p'},
        {NULL, 0, 0, 0},
    };
    int ret=0;
    int opt=0;

    while((opt=getopt_long(argc, argv, "hs:p:", long_options, NULL))!=-1){
        switch (opt) {
            case 's':
                optd->subsys=optarg;
                break;
            case 'p':
                optd->ucopt.dbname=optarg;
                break;
            case 'h':
                optd->ishelp=true;
                break;
            default:
                break;
        }
    }

    if (0 != ret || true == optd->ishelp) {
        print_usage(argv[0], optd);
    }
    return ret;
}

static void print_usage(char *pname, uniconfopt_t *optd)
{
    char *s;
    s=strrchr(pname,'/');
    if(!s++){s=pname;}
    printf("%s [options]\n", s);
    printf("-h|--help: this help\n");
    printf("-m|--subsys <subsystem id>:  default=%s\n", optd->subsys);
    printf("-p|--dbname uniconf db filename(where config data is saved) default=%s\n",
            optd->ucopt.dbname);
}

static void sighandler(int signo)
{
    app_stop_flag = signo;
}

static void init_sighdlr(struct sigaction *act)
{
    act->sa_handler = sighandler;
    sigaction(SIGINT,  act, 0);
    sigaction(SIGTERM, act, 0);
}

static void main_loop(void *args)
{
    uniconfopt_t *optd = (uniconfopt_t *)args;
    xmlNodePtr root_node=NULL;
    xmlNodePtr child=NULL;
    uint32_t loopnum = 0;
    boolean ncsend_init_flag = false;
    boolean ncsend_init_ok = false;
    boolean ncsend_init_done = false;
    const xmlChar *error_msg = NULL;
    status_t res;

    root_node=xmlDocGetRootElement(optd->ucopt.uccfgxml);
    if (NULL==root_node) {
        log_error("\nsil-sa-app: failed to get root node\n");
        app_stop_flag=1;
    }
    child=root_node->children;

    while (!app_stop_flag) {
        if ((loopnum % 10000) == 0) {
            log_debug("\nsil-sa-app: checking ycontrol IO %d\n", loopnum++);
        } else {
            loopnum++;
        }

        res = ycontrol_check_io();
        if (NO_ERR != res) {
            continue;
        }

        if (ycontrol_shutdown_now()) {
            // YControl has received a <shutdown-event>
            // from the server subsystem is no longer active
            // need to  shut down YControl IO loop
            app_stop_flag=1;
            log_info("\nShutdown Event Received!!!\n");
            continue;
        }

        if (0 == app_stop_flag) {
            (void)usleep(10000);
        }

        if (db_api_service_ready() && ncsend_init_done == false && loopnum > 100) {
            char buf[128];

            log_info("\n\n---------------------------");
            log_info("\nDB API Service Ready!!!");

            if (NULL != child) {
                xmlChar *path_str=(xmlChar *)(&buf[0]);

                memset(buf, 0, sizeof(buf));
                sprintf(buf, "/%s", child->name);

                if (false==ncsend_init_flag)
                {
                    log_info("\nSending [%s] Edit to server...\n", child->name);
                    send_config_edits_to_server(path_str, child);
                    ncsend_init_flag = true;
                }
                else if (false == ncsend_init_ok)
                {
                    /* check the test edit */
                    res = db_api_check_edit_ex(&error_msg);
                    if (res == NO_ERR) {
                        ncsend_init_ok=true;
                    } else {
                        if (res == ERR_NCX_SKIPPED) {
                            res = ERR_NCX_OPERATION_FAILED;
                        }
                        log_info("\nTest failed with error: %d '%s'\n",
                                 res, error_msg ? error_msg :
                                 (const xmlChar *)get_error_string(res));
                        ncsend_init_ok=false;
                    }

                    log_info("\nSend [%s] Edit Results: %s\n", child->name,
                             ncsend_init_ok ? "OK" : "NG");

                    /* send next child */
                    child=child->next;
                    if (NULL != child) {
                        if (ncsend_init_ok) {
                            /* reset flags for next config */
                            ncsend_init_flag=false;
                            ncsend_init_ok=false;
                        } else {
                            log_warn("\nInit of [%s] config skipped due to previous error",
                                     child->name);
                            ncsend_init_done=true;
                        }
                    } else {
                        ncsend_init_done=true;
                    }
                }
            }
        }
    }
    log_info("\nMain Loop Ended!!!\n");
}

int main(int argc, char *argv[])
{
    uniconfopt_t optd;
    struct sigaction sigact;
    int ret=0;

    memset(&optd, 0, sizeof(optd));
    memset(&sigact, 0, sizeof(sigact));

    init_sighdlr(&sigact);
    ret = set_options(&optd, argc, argv);
    if (0 == ret && false == optd.ishelp) {
        optd.ucopt.ucmode = 0u; // use process mode
        ret = ucclient_init(&optd.ucopt);
        if (0 == ret) {
            ret = ucclient_get_initial_config(&optd.ucopt);
        }
        if (0 == ret) {
            ret = ypservices_init(&optd, argc, argv);
        }
        if (0 == ret) {
            main_loop(&optd);
            ypservices_cleanup();
        }
        ucclient_cleanup(&optd.ucopt);
    }
    return ret;
}

